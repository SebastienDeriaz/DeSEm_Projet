#ifndef COMMON_DESENET_H
#define COMMON_DESENET_H

#include <stdint.h>

#ifdef __cplusplus
  #include "types.h"	// Should not be seen by C code

  namespace desenet {
#endif // __cplusplus

typedef uint32_t CycleDuration;				///< Used to type time in milliseconds between two beacons.
typedef uint8_t SlotNumber;					///< Type used to define a slot number.
typedef uint16_t SlotDuration;				///< Type to define the duration for a network communication slot.

const SlotNumber MAX_SLOT_NUMBER = 31;		///< Number of the last slot. Slots count from 0 to this number.

#ifdef __cplusplus
  }  // namespace desnet
#endif // __cplusplus

#endif // COMMON_DESENET_H
