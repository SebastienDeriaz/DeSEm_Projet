#include <assert.h>
#include "platform-config.h"
#include "networktimeprovider.h"
#include "nrftransceiverdriver.h"

using desenet::NrfTransceiverDriver;

/*
 * @brief Provides a general method to instanciate a NIC.
 */
desenet::NetworkInterfaceDriver * instanciateNic(uint32_t address, const std::string & name)
{
	UNUSED(address); UNUSED(name);

	static NrfTransceiverDriver nrfTransceiverDriver;

	return &nrfTransceiverDriver;
}

NrfTransceiverDriver::NrfTransceiverDriver()
 : _initialized(false),
   _receiveCallbackHandler(nullptr)
{

}

NrfTransceiverDriver::~NrfTransceiverDriver()
{
	assert(false);	// Instance should never be deleted
}

bool NrfTransceiverDriver::initialize()
{
	if (!_initialized)
	{
		_transceiver.initialize();
		_transceiver.setObserver(*this);
		_initialized = true;
	}
	return true;
}

bool NrfTransceiverDriver::isInitialized() const
{
	return _initialized;
}

std::array<std::uint8_t, 4> NrfTransceiverDriver::localAddress() const
{
	std::array<std::uint8_t, 4> address;
#if (DESENET_GATEWAY_NODE != 0)
  GATEWAY_ADDRESS.copyTo(address.data());
#else
  SENSOR_ADDRESS.copyTo(address.data());
#endif
  return address;
}

bool NrfTransceiverDriver::transmit(const std::uint8_t * const buffer, const std::size_t length)
{
	// Cut away the frame header. It is automatically added by the nrf chip
	_transceiver.transmit(GATEWAY_ADDRESS, buffer + Frame::HEADER_SIZE, length - Frame::HEADER_SIZE);
	return true;
}

void NrfTransceiverDriver::setReceptionHandler(ReceptionCallbackHandler handler /* = nullptr */)
{
	_receiveCallbackHandler = handler;
}

std::string NrfTransceiverDriver::identifier() const
{
	return "nrf0";
}

std::string NrfTransceiverDriver::friendlyName() const
{
	return "nrf0";
}

desenet::NetworkInterfaceDriver * NrfTransceiverDriver::instanciate() const
{
	return const_cast<NrfTransceiverDriver *>(this);
}

void NrfTransceiverDriver::onReceive(const Frame & frame)
{
	assert(_receiveCallbackHandler);

	const uint32_t receptionTime = NetworkTimeProvider::currentNetworkTime();

	_receiveCallbackHandler(*this, receptionTime, frame.buffer(), frame.length());
}

void NrfTransceiverDriver::onSendStatus(IPhyObserver::SendStatus status)
{
	UNUSED(status);
}
