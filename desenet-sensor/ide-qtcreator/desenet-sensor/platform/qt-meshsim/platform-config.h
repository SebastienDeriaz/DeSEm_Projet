#ifndef PLATFORM_CONFIG_H
#define PLATFORM_CONFIG_H

#include "desenet/desenet.h"

#ifdef __cplusplus
  using namespace desenet;
#endif

#define TARGET_PLATFORM_QT_MESHSIM

#ifndef UNUSED
        #define UNUSED(x) (void)(x)
#endif

static const CycleDuration BEACON_INTERVAL_IN_MS = 2000;        // Beacon interval. Needs to be greater then 'SLOT_DURATION_IN_MS * (desenet::MAX_SLOT_NUMBER + 2)'
static const SlotDuration SLOT_DURATION_IN_MS = 100;            // Time slot for a Sensor within it must send its frame(s) to the Gateway
static const SlotNumber DESENET_SLOT_NUMBER = 14;               // From 0 to 'desenet::MAX_SLOT_NUMBER'

const uint8_t SVGROUP_ACCELEROMETER = 2;                        ///< Sampled values group for accelerometer values.
const uint8_t EVID_JOYSTICK = 8;                                ///< Event Id for joystick event.

#define USE_TRACE                       1                       /* Enables/disables trace */
#define USE_USART2_TRACE                1                       /* Use UART2 as output */
#define TRACE_FORMAT_LINUX              1                       /* Set to one if the host is a Linux based OS */
#define TRACE_BUFFER_SIZE               256                     /* Trace internal buffer size */

#define TIMESLOTMANAGER_SIMULATE_BEACON 0                       /* Set to 1 to simulate beacon reception */

#define NRFTRANSCEIVER_TRACE_VERBOSE    1

#define DESNET_NODE_NAME                "SENSOR"

#ifdef __cplusplus
  static const desenet::Address GATEWAY_ADDRESS({0xE2, 0xE2, 0xE2, 0xE2});      ///< @brief Gateway's reception address.
  static const desenet::Address SENSOR_ADDRESS({0xC7, 0xC7, 0xC7, 0xC7});       ///< @brief Sensor's reception address.
#endif // __cplusplus

#endif // PLATFORM_CONFIG_H
