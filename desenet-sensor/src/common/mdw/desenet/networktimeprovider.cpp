#include "clockwork/ttime.h"
#include "networktimeprovider.h"

namespace desenet {

//static
NetworkTime NetworkTimeProvider::currentNetworkTime()
{
	const TTime currentTime = TTime::currentTime();

	// Get the actual time and count milliseconds since midnight.
	return currentTime.getMds();
}

} // namespace desenet
