#ifndef PLATFORM_CONFIG_H
#define PLATFORM_CONFIG_H

#include "desenet/desenet.h"

#ifdef __cplusplus
  using namespace desenet;
#endif

#define TARGET_PLATFORM_NUCLEO_STM32L476RG

// ISR priorities starting from the highest urgency (Cortex-Mx specific)
enum ISR_Priorities
{
	TSM_PRIO,				/* TIM used for time slots */
    SYSTICK_PRIO,			/* Cortex System Tick Interrupt (SysTick_IRQn) */
    NRF24_PRIO,
    UART1_PRIO,
    UART2_PRIO,
    EXTI9_5_PRIO,
};

static const CycleDuration BEACON_INTERVAL_IN_MS = 2000;	// Beacon interval. Needs to be greater then 'SLOT_DURATION_IN_MS * (desenet::MAX_SLOT_NUMBER + 2)'
static const SlotDuration SLOT_DURATION_IN_MS = 100;		// Time slot for a Sensor within it must send its frame(s) to the Gateway
static const SlotNumber DESENET_SLOT_NUMBER = 12;			// From 0 to 'desenet::MAX_SLOT_NUMBER'

const uint8_t SVGROUP_ACCELEROMETER = 2;		///< Sampled values group for accelerometer values.
const uint8_t EVID_JOYSTICK = 8;				///< Event Id for joystick event.

#define USE_TRACE				1		/* Enables/disables trace */
#define USE_USART2_TRACE		1		/* Use UART2 as output */
#define TRACE_UART_INDEX        1       /* Ex.: index 0 means use UART1 */
#define TRACE_FORMAT_LINUX		1		/* Set to one if the host is a Linux based OS */
#define TRACE_BUFFER_SIZE		256		/* Trace internal buffer size */

#define TIMESLOTMANAGER_SIMULATE_BEACON 0	/* Set to 1 to simulate beacon reception */

#define CLOCKWORK_VERBOSE				1
#define CLOCKWORK_SHOWCURRENTTIME		1

#define NRFTRANSCEIVER_TRACE_VERBOSE 	1

#define DESNET_NODE_NAME				"SENSOR"

#ifdef __cplusplus
  static const desenet::Address GATEWAY_ADDRESS({0xE2, 0xE2, 0xE2, 0xE2});	///< @brief Gateway's reception address.
  static const desenet::Address SENSOR_ADDRESS({0xC7, 0xC7, 0xC7, 0xC7});	///< @brief Sensor's reception address.
#endif // __cplusplus

#define JOYSTICK_TRACE_VERBOSE  1

#endif // PLATFORM_CONFIG_H
