#ifndef BOARD_NRFTRANSCEIVER_H
#define BOARD_NRFTRANSCEIVER_H

#include "platform-config.h"				// Defines NRFTRANSCEIVER_TRACE_VERBOSE
#include "iphytransceiver.h"
#include "board/transceiver/nrfspi.h"
#include "xf/xfreactive.h"

#if (EXCLUDE_BOARD_NRFTRANSCEIVER == 0)

extern "C" void EXTI9_5_IRQHandler();
extern "C" void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

namespace board {

/**
 * @brief Class abstracting the nRF24L01 chip.
 *
 * It is used to transmit and receive frames over the air.
 * It has simple send and receive operations. To receive frames
 * and status notifications register using the #setObserver
 * method.
 * Prior to enable the transceiver do not forget to initialize
 * it using the #initialize method.
 */
class NrfTransceiver : public IPhyTransceiver,
					   protected NrfSpi,
					   public XFReactive
{
	friend class NrfTransceiverTester;		// For test purpose only
	friend void ::EXTI9_5_IRQHandler();		// To call onIrq()
	friend void ::HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);    // To call onIrq()

public:
	typedef NrfSpi::tAddress tAddress;		///< Type used for source and destination address

	NrfTransceiver();
	virtual ~NrfTransceiver();

	// IPhyTransceiver interface implementation.
	bool initialize();

	bool setObserver(const IPhyObserver & observer);		///< Observer to forward received frames.

	Mode mode() const;
	bool setMode(Mode mode);
	RadioChannel radioChannel() const;

	/**
	 * Sets the RF channel for the transceiver. Hardware default is channel 2.
	 *
	 * \param channel Valid values are from 0 to 125 (total 126 RF channels)
	 */
	bool setRadioChannel(RadioChannel channel);

	bool send(const Frame & frame);
	bool transmit(const tAddress & destinationAddress, const uint8_t * const buffer, const std::size_t & length);

protected:
	void _initNrfTransceiverIrq() const;			///< @brief Initializes interrupt used by the NRF chip.
	virtual void _initializePipes();				///< @brief Initializes nrf pipes.
	void onIrq();
	EventStatus processEvent();

	/**
	 * Enables/disable transceiver. When enabling the transceiver
	 * it goes into RX mode.
	 *
	 * \param bEnable Set to true to enable transceiver
	 */
	bool enableTransceiver(bool bEnable);
	void enterTXMode();							///< Puts transceiver into TX mode
	void enterRXMode();							///< Puts transceiver into RX mode

	void setDestinationAddress(tAddress address);
	void setReceptionAddress(uint8_t pipeIndex, const tAddress & rxAddress);
	void setAddressWidth(unsigned char widthInByte);
	bool activateFeatures(bool enable);
	void enableDPL(bool enable, unsigned char pipeMask = 0x00);
	inline void flushTxFifo() { NrfSpi::sendCmd(NrfSpi::FLUSH_TX); }
	inline void flushRxFifo() { NrfSpi::sendCmd(NrfSpi::FLUSH_RX); }

	inline IPhyObserver & observer() { return *_pObserver; }

	static void traceOut(const char * const format, ...);

	inline static NrfTransceiver & instance() { assert(_pInstance); return *_pInstance; }

protected:
	static NrfTransceiver * _pInstance;

private:
	bool _bInitialized;							///< Goes to true after calling the #initialize method.
	RadioChannel _channel;						///< Frequency channel transceiver is operating on (default: 2).

	IPhyObserver * _pObserver;					///< Observer who wants to receive packets and status notifications.

	bool _bEnabled;								///< True when transceiver is enabled.
	bool _autoAck;								///< True when data pipe 0 should auto ACK received frames.

	Address _rxAddress[6];						///< The 6 possible RX addresses the chip can listen on.
};

} // namespace board
#endif // EXCLUDE_BOARD_NRFTRANSCEIVER
#endif // BOARD_NRFTRANSCEIVER_H
