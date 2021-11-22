#include <assert.h>
#include <cstring>
#include "platform-config.h"
#include "frame.h"
#include "desenet/networkinterfacedriver.h"

using std::size_t;
using std::uint8_t;
using std::shared_ptr;
using std::make_shared;
using std::memcpy;
using desenet::NetworkInterfaceDriver;

desenet::Frame::Frame(size_t initialLength)
 : buffer_(initialLength),
   frameLength_(0)
{
    assert(buffer_.data());

    setLength(initialLength);
}

desenet::Frame::Frame(const SharedByteBuffer &buffer): buffer_(buffer)
{
    assert(buffer_.data());
    frameLength_ = buffer.length();
}

desenet::Frame desenet::Frame::useBuffer(std::uint8_t * buffer, std::size_t length)
{
    return Frame(SharedByteBuffer::proxy(buffer, length));
}

const desenet::Frame desenet::Frame::useBuffer(const uint8_t * const buffer, size_t length)
{
    return Frame(SharedByteBuffer::proxy(buffer, length));
}

desenet::Frame desenet::Frame::copyFromBuffer(const uint8_t * const buffer, size_t length)
{
    return Frame(SharedByteBuffer::copy(buffer, length));
}

bool desenet::Frame::isValid() const
{
    return !buffer_.empty() &&
    		frameLength_ >= Frame::HEADER_SIZE + 1 &&	// + 1 for byte containing frame type
    		type() != FrameType::Invalid;
}

void desenet::Frame::setLength(size_t length)
{
    if (length <= buffer_.length())
    {
        frameLength_ = length;

        // Set also the payload field accordingly
        setPayloadSize(length - (Frame::HEADER_SIZE + Frame::FOOTER_SIZE));
    }
    else
    {
    	assert(false);	// Frame length too big for buffer!
    }
}

desenet::Address desenet::Frame::destination() const
{
    Address destination;

    if (frameLength_ >= ADDRESS_SIZE)
    {
        destination.copyFrom(buffer_.data());
    }
    return destination;
}

uint8_t desenet::Frame::payloadSize() const
{
	uint8_t payloadSize = 0;

	payloadSize = *(buffer_.data() + desenet::Address::addressSize());

	return payloadSize;
}

void desenet::Frame::setDestination(const desenet::Address & destination)
{
	// Destination address is first field in frame
	memcpy(buffer_.data(), destination.data(), destination.addressSize());
}

void desenet::Frame::setPayloadSize(uint8_t payloadSize)
{
	memcpy(buffer_.data() + desenet::Address::addressSize(), &payloadSize, sizeof(payloadSize));
}

desenet::FrameType desenet::Frame::type() const {
    if (frameLength_ >= 1 && ((buffer_[Frame::HEADER_SIZE + 0] & 0x80) == 0x00))
        return FrameType::Beacon;
    else if (frameLength_ >= 1 && ((buffer_[Frame::HEADER_SIZE + 0] & 0x80) == 0x80))
        return FrameType::MPDU;
    else
        return FrameType::Invalid;
}

void desenet::Frame::setType(desenet::FrameType type)
{
	switch (type)
	{
	case FrameType::Invalid:
		break;
	case FrameType::Beacon:
		buffer_[Frame::HEADER_SIZE + 0] &= ~0x80;	// Set bit 7 to zero
		break;
	case FrameType::MPDU:
		buffer_[Frame::HEADER_SIZE + 0] |= 0x80;	// Set bit 7 to one
		break;
	}
}

const char * desenet::Frame::toString() const
{
	return toString(buffer_.data(), length());		// Call the static version of toString()
}

//static
const char * desenet::Frame::toString(const uint8_t * const buffer, const std::size_t & length)
{
	static char str[3 * Frame::Mtu];

	for (size_t i = 0; i < length; i++)
	{
		sprintf(str + (i * 3), "%02x ", buffer[i]);
	}

	str[length * 3] = '\0';		// Finish the string

	return str;
}

NetworkInterfaceDriver & desenet::operator <<(NetworkInterfaceDriver & driver, const desenet::Frame & frame)
{
    driver.transmit(frame.buffer(), frame.length());
    return driver;
}
