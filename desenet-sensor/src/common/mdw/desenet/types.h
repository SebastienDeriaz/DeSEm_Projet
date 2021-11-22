#pragma once
#include <bitset>
#include <cstdint>
#include "containers/SharedByteBuffer"
#include "address.hpp"

namespace desenet {

const uint8_t ADDRESS_SIZE = 4;

/**
 * @brief Shared buffer in order to pass data around the stack without the need to copy it and still have memory returned
 *        in the moment the last user releases the object.
 */
typedef ::hei::SharedByteBuffer SharedByteBuffer;

/**
 * @brief Identifies a single sampled values group. Valid values are from 0 to 15.
 */
typedef std::uint8_t SvGroup;
    
/**
 * @brief Sampled values group mask: For each group, a single boolean indicates if the group is enabled (true) or not (false).
 *
 * These boolean values can be accessed in a array like manner typedef the suffix operators.
 */
typedef std::bitset<16> SvGroupMask;

/**
 * @brief Identifies an event.
 */
typedef std::uint16_t EvId;

/**
 * MAC address.
 */
typedef phy::Address<4> Address;

/**
 * @brief The network time as a 32 bit unsigned integer in milliseconds midnight.
 */
typedef std::uint32_t NetworkTime;

} // namespace desenet
