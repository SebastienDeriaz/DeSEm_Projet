#ifndef NRFTRANSCEIVERDRIVER_H
#define NRFTRANSCEIVERDRIVER_H

#include "desenet/networkinterfacedriver.h"
#include "board/transceiver/nrftransceiver.h"
#include "board/transceiver/iphyobserver.h"

namespace desenet {

/**
 * @brief Nrf24L01 specific NetworkInterfaceDriver implementation.
 */
class NrfTransceiverDriver : public NetworkInterfaceDriver,
							 public NetworkInterfaceDriver::Descriptor,
							 protected IPhyObserver
{
public:
	NrfTransceiverDriver();
	virtual ~NrfTransceiverDriver();

	// NetworkInterfaceDriver interface implementation
public:
	virtual std::string identifier() const;
	virtual bool initialize();
	virtual bool isInitialized() const;
	virtual std::array<std::uint8_t, 4> localAddress() const;
	virtual bool transmit(const std::uint8_t *const buffer, const std::size_t length);
	virtual void setReceptionHandler(ReceptionCallbackHandler handler = nullptr);

	// NetworkInterfaceDriver::Descriptor interface implementation
public:
	virtual std::string friendlyName() const;
	virtual desenet::NetworkInterfaceDriver * instanciate() const;

	// IPhyObserver interface implementation
protected:
	virtual void onReceive(const Frame & frame);
	virtual void onSendStatus(SendStatus status);

protected:
	bool _initialized;
	board::NrfTransceiver _transceiver;

	ReceptionCallbackHandler _receiveCallbackHandler;
};
} // namespace desenet

#endif
