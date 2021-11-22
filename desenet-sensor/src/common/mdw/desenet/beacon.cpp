#include <algorithm>
#include <cstring>
#include "platform-config.h"
#include "beacon.h"

using std::string;
using std::uint32_t;
using std::uint8_t;
using std::size_t;
using std::min;
using std::memcpy;
using std::memset;
using std::bitset;

desenet::Beacon::Beacon(uint32_t cycleInterval)
 : Frame(Beacon::SIZE)
{
	memset(buffer() + Frame::HEADER_SIZE, 0, 1);	// Set first field in payload to zero
    setDestination(SENSOR_ADDRESS);
    setType(FrameType::Beacon);
    setNetworkTime(0);
    setCycleInterval(cycleInterval);
}

desenet::Beacon::Beacon(const Frame & frame)
 : Frame(frame)
{
}

void desenet::Beacon::setDestinationAddress(Address destinationAddress)
{
	Frame::setDestination(destinationAddress);
}

uint64_t desenet::Beacon::networkTime() const
{
    uint64_t time;
    memcpy(&time, buffer() + Frame::HEADER_SIZE + 1, sizeof(time));
    return time;
}

void desenet::Beacon::setNetworkTime(uint32_t time)
{
	// Copy network time (Will erase frametype field)
    memcpy(buffer() + Frame::HEADER_SIZE + 1, &time, sizeof(time));
}

uint32_t desenet::Beacon::cycleInterval() const
{
    uint32_t interval;
    memcpy(&interval, buffer() + Frame::HEADER_SIZE + 5, sizeof(interval));
    return interval;
}

void desenet::Beacon::setCycleInterval(uint32_t interval)
{
    memcpy(buffer() + Frame::HEADER_SIZE + 5, &interval, 3 /* bytes */);
}

desenet::SlotDuration desenet::Beacon::slotDuration()
{
	desenet::SlotDuration slotDuration;

    memcpy(&slotDuration, buffer() + Frame::HEADER_SIZE + 8, sizeof(slotDuration));

	return slotDuration;
}

void desenet::Beacon::setSlotDuration(desenet::SlotDuration slotDuration)
{
	memcpy(buffer() + Frame::HEADER_SIZE + 8, &slotDuration, sizeof(slotDuration));
}

desenet::SvGroupMask desenet::Beacon::svGroupMask() const
{
    uint32_t bits;
    memcpy(&bits, buffer() + Frame::HEADER_SIZE + 10, sizeof(bits));
    return SvGroupMask(bits);
}

void desenet::Beacon::setSvGroupMask(const desenet::SvGroupMask & mask)
{
    uint32_t bits = mask.to_ulong();
    memcpy(buffer() + Frame::HEADER_SIZE + 10, &bits, sizeof(bits));
}
