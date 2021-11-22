#ifndef DESENET_NETWORKTIMEPROVIDER_H
#define DESENET_NETWORKTIMEPROVIDER_H

#include "types.h"

namespace desenet {

/**
 * @brief Provides actual network time as specified by the desenet protocol.
 */
class NetworkTimeProvider
{
public:
	static NetworkTime currentNetworkTime();		///< Returns the actual network time (milliseconds since midnight).
};

} // namespace desenet
#endif // DESENET_NETWORKTIMEPROVIDER_H
