#include "platform-config.h"
#include "critical/critical.h"
#include "board/transceiver/iphyobserver.h"
#include "desenet/frame.h"
#include "trace/trace.h"
#include "hal/spi.h"
#if (NRFTRANSCEIVER_TRACE_VERBOSE != 0)
	#include <stdarg.h>
	#include <stdio.h>
#endif
#include "nrftransceiver.h"
#include "xf/xfstaticevent.h"

#if (EXCLUDE_BOARD_NRFTRANSCEIVER == 0)

using desenet::Frame;
using board::NrfTransceiver;

static const size_t MaximalPayloadSize = 32;
NrfTransceiver * NrfTransceiver::_pInstance(nullptr);

NrfTransceiver::NrfTransceiver()
 : NrfSpi(Spi::instance(2 /* SPI3 */)),
   _bInitialized(false),
   _channel(2),			// Reset value given by register reset value (RF_CH register)
   _pObserver(NULL),
   _bEnabled(false),
   _autoAck(true)		// Per default enabled by the NrfTransceiver hardware
{
	assert(!_pInstance);		// Only one instance allowed
	_pInstance = this;
}

NrfTransceiver::~NrfTransceiver()
{

}

bool NrfTransceiver::initialize()
{
	if (!_bInitialized)
	{
		_bInitialized = true;

#if !(NRFTRANSCEIVER_TRACE_VERBOSE != 0)
		Trace::outln("Nrf: Verbose Trace disabled");
#endif

		// Initialize SPI used for NrfTransceiver
		NrfSpi::initialize();

		//
		// Now the nRF transceiver can be accessed correctly
		//

		NrfSpi::disableCS();		// Release transceiver
		NrfSpi::disableCE();		// Discard transmission

		// Reset FIFOs
		flushTxFifo();
		flushRxFifo();

		// Reset NRF IRQ status bits
		NrfSpi::writeRegister(NrfSpi::STATUS, (NrfSpi::STATUS_REG_RX_DR | NrfSpi::STATUS_REG_TX_DS | NrfSpi::STATUS_REG_MAX_RT));

		// Set address width used
		setAddressWidth(tAddress::addressSize());
		assert((size_t)(NrfSpi::readRegister(NrfSpi::SETUP_AW) + 2) == tAddress::addressSize());

		activateFeatures(true);
		enableDPL(true, 0x03);		// Enable dynamic payload size for pipe 0 and 1
		assert((NrfSpi::readRegister(NrfSpi::FEATURE) & NrfSpi::FEATURE_REG_EN_DPL) == FEATURE_REG_EN_DPL);

		// Enable feature that gives ability to transmit frames without receiving an acknowledgment
		NrfSpi::enableDynamicTxAcknowledgment(true);

		// Basic configuration for CONFIG register: Enable CRC
		NrfSpi::writeRegister(NrfSpi::CONFIG, NrfSpi::CONFIG_REG_EN_CRC);

		// Initialized pipes according application needs
		_initializePipes();

		// Initialize and enable IRQ for the NRF_IRQ line (GPIO RC9)
		_initNrfTransceiverIrq();

		// Set (again) hardware default values of the Nrf chip
		setRadioChannel(2);

		// Enable transceiver (and enter RX mode)
		setMode(Active);
	}

	return true;
}

void NrfTransceiver::_initNrfTransceiverIrq() const
{

}

void NrfTransceiver::_initializePipes()
{
#if	(DESENET_GATEWAY_NODE != 0)
	// Configure pipe 0
	// Used to receive sensor data
	NrfSpi::enableDataPipe(0x01, true);
	// Set reception address to listen on
	setReceptionAddress(0, GATEWAY_ADDRESS);
	// Disable auto-ack for pipe 0. Gateway does only listen on this pipe.
	NrfSpi::enableAutoAcknowledgementForPipe(0x01, false);

	// Configure pipe 1
	// Disable pipe 1 (enabled by default)
	NrfSpi::enableDataPipe(0x02, false);

	// Set TX address used when sending beacon frame
	setDestinationAddress(SENSOR_ADDRESS);

#else // DESENET_SENSOR_NODE
	// Configure pipe 0
	// Used to receive sensor data
	NrfSpi::enableDataPipe(0x01, true);
	// Set reception address to listen on
	setReceptionAddress(0, SENSOR_ADDRESS);
	// Disable auto-ack for pipe 0. Sensor does only listen on this
	// pipe for beacons, but does not acknowledge them.
	NrfSpi::enableAutoAcknowledgementForPipe(0x01, false);

	// Configure pipe 1
	// Enable pipe 1
	NrfSpi::enableDataPipe(0x02, false);
	// Set reception address to listen on
	setReceptionAddress(1, GATEWAY_ADDRESS);
	// Disable auto-ack for pipe 1. Sensor does only listen on this
	// pipe for sensor data from other sensor.
	NrfSpi::enableAutoAcknowledgementForPipe(0x02, false);

	// Set TX address used when sending sensor data frames
	setDestinationAddress(GATEWAY_ADDRESS);

	enterRXMode();	// Start listening
#endif
}

/**
 * Called out of the interrupt service routine. Is called when
 * nRF24L01 chip is lowering the NRF_IRQ line.
 * The code insight this method is executed in an ISR!
 */
void NrfTransceiver::onIrq()
{
	static XFStaticEvent irqEvent( 0 , this );

	enterISR();
	pushEvent( &irqEvent );
	exitISR();
}

EventStatus NrfTransceiver::processEvent()
{
	unsigned char status = NrfSpi::status();

	if (!NrfSpi::isInitialized())
	{
		return EventStatus::NotConsumed;
	}

	while (!rxFifoEmpty())
	{
		const uint8_t pipe = NrfSpi::getRXFramePipeNumber();

		// Only continue if a pipe is valid
		if (pipe <= 5)
		{
			size_t size = NrfSpi::getRXFramePaylpoadSize(pipe);

			if (size > MaximalPayloadSize)
			{
				// Check size. It must not exceed 32 bytes.

				// As explained in the R_RX_PL_WID section. Flush
				// the RX FIFO if this happen.
				flushRxFifo();
			}
			else if (size == 0)
			{
				flushRxFifo();
			}
			else
			{
				Frame frame(Frame::HEADER_SIZE + size);

				if (frame.isValid())
				{
					// Add RX address to frame
					_rxAddress[pipe].copyTo(frame.buffer());
					// Add payload size to frame (right after RX address)
					frame[Address::addressSize()] = size;

					// Read payload data
					NrfSpi::readFrame(frame);

#if (NRFTRANSCEIVER_TRACE_VERBOSE != 0)	// Not even vall frame.toString()
					traceOut("Nrf: RX Frame (size %d): %s", frame.length(), frame.toString());
#endif // #if (NRFTRANSCEIVER_TRACE_VERBOSE != 0)

					if (_pObserver)
					{
						observer().onReceive(frame);
					}
					else
					{
						traceOut("Nrf Irq: Observer not set");
					}
				}
				else
				{
					// The RX frames needs to be read or removed
					// Otherwise the transceiver will again rise the
					// IRQ line!
					flushRxFifo();
				}
			}
		}

		status = NrfSpi::status();

		if ((status & NrfSpi::STATUS_REG_RX_DR) == NrfSpi::STATUS_REG_RX_DR)
		{
			// Clear RX_DR bit in status register
			status = NrfSpi::writeRegister(NrfSpi::STATUS, (status | NrfSpi::STATUS_REG_RX_DR));
		}
	}

	status = NrfSpi::status();

	// There are rare cases where the RX FIFO is empty and the RX_DR status flag still high.
	if ((status & NrfSpi::STATUS_REG_RX_DR) == NrfSpi::STATUS_REG_RX_DR)
	{
		// Clear RX_DR bit in status register
		status = NrfSpi::writeRegister(NrfSpi::STATUS, (status | NrfSpi::STATUS_REG_RX_DR));
	}

	if ((status & NrfSpi::STATUS_REG_TX_FULL) == NrfSpi::STATUS_REG_TX_FULL)
	{
		// Flush TX FIFO
		flushTxFifo();
		traceOut("irq:FLUSH_TX");
	}

	if ((status & NrfSpi::STATUS_REG_TX_DS) == NrfSpi::STATUS_REG_TX_DS)	// Data send
	{
		traceOut("irq::TX_DS");

		// Clear TX_DS bit in status register
		status = NrfSpi::writeRegister(NrfSpi::STATUS, (status | NrfSpi::STATUS_REG_TX_DS));

		observer().onSendStatus(IPhyObserver::Delivered);

		// Change from TX to RX mode
		enterRXMode();
	}

	if ((status & NrfSpi::STATUS_REG_MAX_RT) == NrfSpi::STATUS_REG_MAX_RT)	// Reached maximum retries
	{
		traceOut("irq:MAX_RT");

		// Discard frame by flushing TX fifo
		// See nRF24L01 datasheet on page 43, last paragraph
		flushTxFifo();

		// Clear max RT
		status = NrfSpi::writeRegister(NrfSpi::STATUS, (status | NrfSpi::STATUS_REG_MAX_RT));

		observer().onSendStatus(IPhyObserver::NotDelivered);

		// Change from TX to RX mode
		enterRXMode();
	}

	if (NrfSpi::rxFifoFull() && ((status & NrfSpi::STATUS_REG_RX_DR) != NrfSpi::STATUS_REG_RX_DR))
	{
		// Flush RX FIFO
		flushRxFifo();
	}

	return EventStatus::Consumed;
}

void NrfTransceiver::setDestinationAddress(tAddress address)
{
	NrfSpi::writeRegister(NrfSpi::TX_ADDR, address);
}

void NrfTransceiver::setReceptionAddress(uint8_t pipeIndex, const tAddress & rxAddress)
{
	if (pipeIndex <= 6)
	{
		NrfSpi::writeRegister(static_cast<NrfSpi::eNrfRegister>(NrfSpi::RX_ADDR_P0 + pipeIndex), rxAddress);

		_rxAddress[pipeIndex] = rxAddress;
	}
}

void NrfTransceiver::setAddressWidth(unsigned char widthInByte)
{
	assert(widthInByte == 3 || widthInByte == 4 || widthInByte == 5);

	NrfSpi::writeRegister(NrfSpi::SETUP_AW, widthInByte - 2);
}

bool NrfTransceiver::activateFeatures(bool enable)
{
	return NrfSpi::activateFeatures(enable);
}

/**
 * Enable/disable dynamic payload length (DPL).
 *
 * \enable Set to true to enable DPL
 * \pipeMask Bitmask to define for which RX pipe DPL should be enabled. The LSB corresponds to pipe0.
 *
 * Attention:
 *  - You need to enable first call activateFeatues() in order to use
 *    the DPL feature.
 *  - You also need to enable <em>Enhanced ShockBurst</em>.
 */
void NrfTransceiver::enableDPL(bool enable, unsigned char pipeMask /* = 0x00 */)
{
	NrfSpi::tRegister reg;
	assert(_bNrfFeaturesEnabled);	// Call activateFeatues first

	reg = NrfSpi::readRegister(FEATURE);

	if (enable)
	{
		assert(pipeMask);		// Set at least to 0x01 (DPL on pipe0)

		reg |= FEATURE_REG_EN_DPL;
	}
	else
	{
		reg &= ~FEATURE_REG_EN_DPL;
	}

	NrfSpi::writeRegister(FEATURE, reg);

	if (enable)
	{
		// Enable DPL on corresponding pipes
		NrfSpi::writeRegister(NrfSpi::DYNPD, pipeMask);
	}
}

void NrfTransceiver::traceOut(const char * const format, ...)
{
#if (NRFTRANSCEIVER_TRACE_VERBOSE != 0)
	static char str[256];

	va_list args;

	va_start(args, format);
	vsiprintf(str, format, args);
	va_end(args);

	assert(strlen(str) < 256);		// Uups! str is too small

	Trace::outln(str);
#else
	UNUSED(format);
#endif
}

IPhyTransceiver::Mode NrfTransceiver::mode() const
{
	return (_bEnabled) ? Active : Inactive;
}

bool NrfTransceiver::setMode(Mode mode)
{
	assert(NrfSpi::isInitialized());	// Call initialize() first!

	return enableTransceiver((mode == Active) ? true : false);
}

bool NrfTransceiver::enableTransceiver(bool bEnable)
{
	if (_bEnabled != bEnable)
	{
		unsigned char config;

		_bEnabled = bEnable;

		config = NrfSpi::readRegister(NrfSpi::CONFIG);

		if (bEnable)
		{
			// power-up, RX
			config |= NrfSpi::CONFIG_REG_PWR_UP |
					  NrfSpi::CONFIG_REG_PRIM_RX;
			NrfSpi::writeRegister(NrfSpi::CONFIG, config);

			// Enable transceiver
			NrfSpi::enableCE();
		}
		else
		{
			// Power down the chip
			config &= ~NrfSpi::CONFIG_REG_PWR_UP;
			NrfSpi::writeRegister(NrfSpi::CONFIG, config);

			// Disable transceiver
			NrfSpi::disableCE();
		}

		// read
		//config = NrfSpi::readRegister(NrfSpi::CONFIG);
	}
	return true;
}

void NrfTransceiver::enterTXMode()
{
	unsigned char config;

	// Enter TX mode
	config = NrfSpi::readRegister(NrfSpi::CONFIG);
	config &= ~NrfSpi::CONFIG_REG_PRIM_RX;		// Clear PRIM_RX_BIT to enter TX mode
	// Write new value to config register
    NrfSpi::writeRegister(NrfSpi::CONFIG, config);
}

void NrfTransceiver::enterRXMode()
{
	unsigned char config;

	// Enter RX mode
	config = NrfSpi::readRegister(NrfSpi::CONFIG);
	config |= NrfSpi::CONFIG_REG_PRIM_RX;		// Set PRIM_RX_BIT to enter RX mode
	// Write new value to config register
    NrfSpi::writeRegister(NrfSpi::CONFIG, config);
}

IPhyTransceiver::RadioChannel NrfTransceiver::radioChannel() const
{
	return _channel;
}

bool NrfTransceiver::setRadioChannel(RadioChannel channel)
{
	assert(NrfSpi::isInitialized());	// Call initialize() first!

	// Only values from 0 to 125 are allowed
	if (channel < 126)
	{
		if (_channel != channel)
		{
			_channel = channel;

			traceOut("Nrf: Changed radio channel: %d", _channel);

			NrfSpi::writeRegister(NrfSpi::RF_CH, channel);
		}
		return true;
	}
	return false;
}

bool NrfTransceiver::setObserver(const IPhyObserver & observer)
{
	if (!_pObserver)
	{
		_pObserver = const_cast<IPhyObserver*>(&observer);
		assert(_pObserver);
		return true;
	}
	return false;
}

bool NrfTransceiver::send(const Frame & frame)
{
	unsigned char status;

	UNUSED(status);

	assert(NrfSpi::isInitialized());	// Call initialize() first!

	if (!frame.length())
	{
		assert(false);	// Really wont to send frame with zero length?!
		return false;
	}

	// Set destination address
	setDestinationAddress(frame.destination());

	traceOut("Nrf: TX Frame (size %d): %s", frame.length(), frame.toString());

	NrfSpi::disableCE();

	// Send payload - send any data
	status = NrfSpi::sendFrame(frame);

    enterTXMode();

	// Enable CE -> Starts the transmission
    NrfSpi::enableCE();

	return true;
}

bool NrfTransceiver::transmit(const tAddress & destinationAddress, const uint8_t * const buffer, const std::size_t & length)
{
	unsigned char status;
	UNUSED(status);		// Make compiler happy

	assert(NrfSpi::isInitialized());	// Call initialize() first!

	if (!length)
	{
		assert(false);	// Really wont to send frame with zero length?!
		return false;
	}

	// Set destination address
	setDestinationAddress(destinationAddress);

	traceOut("Nrf: TX Frame (size %d): %s", length, Frame::toString(buffer, length));

	NrfSpi::disableCE();

	// Send payload - send any data
	status = NrfSpi::transmit(buffer, length);

    enterTXMode();		// Start transmission

	// Enable CE -> Starts the transmission
    NrfSpi::enableCE();

	return true;
}

#endif // EXCLUDE_BOARD_NRFTRANSCEIVER
