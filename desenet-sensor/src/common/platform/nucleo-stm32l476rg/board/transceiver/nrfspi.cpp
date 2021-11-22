#include "desenet/frame.h"
#include "nrfspi.h"

using desenet::Frame;

NrfSpi::NrfSpi(Spi & spi)
 : _bInitialized(false),
   _spi(spi),
   _bNrfFeaturesEnabled(false)
{

}

NrfSpi::~NrfSpi()
{

}

bool NrfSpi::initialize()
{
	if (!isInitialized())
	{
		_bInitialized = true;

		spi().initialize();

		return true;
	}
	return false;
}

NrfSpi::tStatus NrfSpi::status()
{
	return sendCmd(NrfSpi::NOP);
}

NrfSpi::tStatus NrfSpi::sendCmd(eNrfSpiCmd cmd)
{
	tStatus status = 0x00;

	assert(cmd != R_RX_PAYLOAD);			// Use readFrame() instead
	assert(cmd != W_TX_PAYLOAD);			// Use sendFrame() instead
	assert(cmd != W_REGISTER);				// Use writeRegister() methods instead
	assert(cmd != R_REGISTER);				// Use readRegister() methods instead
	assert(cmd != W_ACK_PAYLOAD);
	assert(cmd != W_TX_PAYLOAD_NO_ACK);

	enableCS();

	switch (cmd)
	{
	case FLUSH_TX:
	case FLUSH_RX:
	case REUSE_TX_PL:
	case NOP:
		status = sendByte(cmd);
		break;
	case ACTIVATE:
		status = sendByte(cmd);
		sendByte(ACTIVATE_DATA);
		break;
	case R_RX_PL_WID:
		status = sendByte(cmd);
		status = sendByte(NOP);
		break;
	case R_RX_PAYLOAD:
	case W_TX_PAYLOAD:
	case W_REGISTER:
	case R_REGISTER:
	case W_ACK_PAYLOAD:
	case W_TX_PAYLOAD_NO_ACK:
		// To nothing
		break;
	}

	disableCS();
	return status;
}

/**
 * Sends a frame to the destination node. Use TX_ADDR register to
 * set the address for the destination node.
 */
NrfSpi::tStatus NrfSpi::sendFrame(const Frame & frame)
{
	tStatus status = 0x00;

	enableCS();

	status = sendByte(W_TX_PAYLOAD);

	for (size_t i = 0; i < frame.length(); i++)
	{
		sendByte(frame[i]);
	}

	disableCS();
	return status;
}

/**
 * Sends the buffer to the destination node. Use TX_ADDR register to
 * set the address for the destination node.
 */
NrfSpi::tStatus NrfSpi::transmit(const uint8_t * const buffer, const std::size_t & length)
{
	tStatus status = 0x00;

	enableCS();

	status = sendByte(W_TX_PAYLOAD);

	for (size_t i = 0; i < length; i++)
	{
		sendByte(buffer[i]);
	}

	disableCS();
	return status;
}

NrfSpi::tStatus NrfSpi::readFrame(Frame & frame)
{
	tStatus status = 0x00;

	assert(frame.length());		// Please specify how many bytes you want to read

	enableCS();

	status = sendByte(R_RX_PAYLOAD);

	for (size_t i = 0; i < frame.length() - Frame::HEADER_SIZE; i++)
	{
		frame[Frame::HEADER_SIZE + i] = sendByte(NOP);
	}

	disableCS();
	return status;
}

NrfSpi::tRegister NrfSpi::readRegister(eNrfRegister reg)
{
	tRegister value;

	// All but not these registers are allowed
	assert(!(reg == TX_ADDR || (reg >= RX_ADDR_P0 && reg <= RX_ADDR_P5)));

	enableCS();

	sendByte((R_REGISTER << 5) | reg);
	value = sendByte(NOP);

	disableCS();
	return value;
}

NrfSpi::tStatus NrfSpi::readRegister(eNrfRegister reg, tAddress & address)
{
	tStatus status = 0x00;

	// Only these registers are allowed
	assert(reg == TX_ADDR || (reg >= RX_ADDR_P0 && reg <= RX_ADDR_P5));

	enableCS();

	status = sendByte((R_REGISTER << 5) | reg);
	for (size_t i = 0; i < address.addressSize(); i++)
	{
		address[i] = sendByte(NOP);
	}

	disableCS();
	return status;
}

NrfSpi::tStatus NrfSpi::writeRegister(eNrfRegister reg, tRegister value)
{
	tStatus status = 0x00;

	// All but not these registers are allowed
	assert(!(reg == TX_ADDR || (reg >= RX_ADDR_P0 && reg <= RX_ADDR_P5)));

	enableCS();

	status = sendByte((W_REGISTER << 5) | reg);
	sendByte(value);

	disableCS();
	return status;
}

NrfSpi::tStatus NrfSpi::writeRegister(eNrfRegister reg, const tAddress & address)
{
	tStatus status = 0x00;

	// Only these registers are allowed
	assert(reg == TX_ADDR || (reg >= RX_ADDR_P0 && reg <= RX_ADDR_P5));

	enableCS();

	status = sendByte((W_REGISTER << 5) | reg);
	for (size_t i = 0; i < address.addressSize(); i++)
	{
		sendByte(address[i]);
	}

	disableCS();
	return status;
}

bool NrfSpi::activateFeatures(bool enable)
{
	// Check if not already in this state
	if (_bNrfFeaturesEnabled != enable)
	{
		sendCmd(NrfSpi::ACTIVATE);
		_bNrfFeaturesEnabled = enable;
		return true;
	}
	return false;
}

unsigned char NrfSpi::getRXFramePipeNumber()
{
	unsigned char pipeNumber;
	tStatus status = NrfSpi::status();

	// Extract RX_P_NO field
	pipeNumber = ((status >> 1) & 0x07);

	if (!(pipeNumber == 0x07 || pipeNumber == 0x06))
	{
		return pipeNumber;
	}
	assert(false);	// Call this method only when you are sure there is a packet available
	return 0xFF;
}

size_t NrfSpi::getRXFramePaylpoadSize(uint8_t pipeNumber)
{
	assert(pipeNumber < 6);

	if (!featuresEnabled())
	{
		return readRegister((NrfSpi::eNrfRegister)(RX_PW_P0 + pipeNumber));
	}
	else
	{
		// DPL. Read value of next frame in RX FIFO by reading the R_RX_PL_WID register
		return sendCmd(R_RX_PL_WID);
	}
}

/**
 * Upon startup pipes 0 and 1 are activated by default.
 */
void NrfSpi::enableDataPipe(uint8_t pipeMask, bool enable)
{
	tRegister registerValue = readRegister(EN_RXADDR);

	assert((pipeMask & 0x3F) == pipeMask);		// Bits 6 and 7 shouldn't be touched

	if (enable)
	{
		registerValue |= pipeMask;
	}
	else
	{
		registerValue &= ~pipeMask;
	}

	writeRegister(EN_RXADDR, registerValue);
}

/**
 * Upon startup auto-acknowledgment for pipe 0 and pipe 1 are enabled by
 * default.
 */
void NrfSpi::enableAutoAcknowledgementForPipe(uint8_t pipeMask, bool enable)
{
	tRegister registerValue = readRegister(EN_AA);

	assert((pipeMask & 0x3F) == pipeMask);		// Bits 6 and 7 shouldn't be touched

	if (enable)
	{
		registerValue |= pipeMask;
	}
	else
	{
		registerValue &= ~pipeMask;
	}

	writeRegister(EN_AA, registerValue);
}

/**
 * Enables feature that gives ability to transmit frames without receiving an acknowledgment.
 * After setting this feature you can send frames over the W_TX_PAYLOAD_NOACK register. This
 * will automatically set the NO_ACK bit in the frame being send.
 */
void NrfSpi::enableDynamicTxAcknowledgment(bool enable)
{
	NrfSpi::tRegister reg;

	if (!featuresEnabled())
	{
		activateFeatures(true);
	}

	reg = readRegister(NrfSpi::FEATURE);

	if (enable)
	{
		reg |= FEATURE_REG_EN_DYN_ACK;
	}
	else
	{
		reg &= ~FEATURE_REG_EN_DYN_ACK;
	}

	writeRegister(NrfSpi::FEATURE, reg);
}

bool NrfSpi::rxFifoEmpty()
{
	const NrfSpi::tRegister reg = readRegister(NrfSpi::FIFO_STATUS);

	return ((reg & FIFO_STATUS_RX_EMPTY) == FIFO_STATUS_RX_EMPTY) ? true : false;
}

bool NrfSpi::txFifoEmpty()
{
	const NrfSpi::tRegister reg = readRegister(NrfSpi::FIFO_STATUS);

	return ((reg & FIFO_STATUS_TX_EMPTY) == FIFO_STATUS_TX_EMPTY) ? true : false;
}

bool NrfSpi::rxFifoFull()
{
	const NrfSpi::tRegister reg = readRegister(NrfSpi::FIFO_STATUS);

	return ((reg & FIFO_STATUS_RX_FULL) == FIFO_STATUS_RX_FULL) ? true : false;
}

NrfSpi::tRegister NrfSpi::readRPDRegister()
{
	return readRegister(NrfSpi::RPD);
}

NrfSpi::tStatus NrfSpi::sendByte(unsigned char data)
{
	return spi().sendByte(data);
}
