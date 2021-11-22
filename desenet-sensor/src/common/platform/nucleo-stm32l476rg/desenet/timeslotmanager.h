#ifndef STM32_TIMESLOTMANAGER_H
#define STM32_TIMESLOTMANAGER_H

#include <cassert>
#include "stm32l4xx_hal.h"
#include "desenet/desenet.h"
#include "desenet/itimeslotmanager.h"
#include "xf/xfreactive.h"

extern "C" void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef * htim);

namespace desenet {

/**
 * @brief TimeSlotManager provides to correct time for the time slots.
 *
 * Only one instance if this class is allowed (Singleton pattern).
 */
class TimeSlotManager : public ITimeSlotManager,
						protected XFReactive
{
	friend void ::HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef * htim);    // Can call onIrq();

public:
	TimeSlotManager();
	~TimeSlotManager();

	static TimeSlotManager & instance() { assert(_pInstance); return *_pInstance; }			///< General access to the instance.

	// ITimeSlotManager interface implementation
public:
	virtual void initialize(const desenet::SlotNumber & slotNumber);
	virtual void initializeRelations(const Observer & observer);
	virtual bool setObserver(const Observer & observer);
	virtual void start();

	virtual void onBeaconReceived(const desenet::SlotDuration & slotDuration);

protected:
	// Interface from XFReactive
	virtual EventStatus processEvent();		///< Implements the state machine.

protected:
	static void onIrq();					///< Called by the interrupt service routine (ISR).
	void notify(const SIG & signal);		///< Notifies observers about changes.

protected:
	void _startTimer();													///< Starts the hardware timer.
	void _setSlotDuration(const desenet::SlotDuration & slotDuration);	///< Configures the hardware timer with the right time.
	void _stopTimer();													///< Starts the hardware timer.
	void _processIrq();													///< Called every time an IRQ arrives. Used to handle time slots.

	void _enableInterrupt();											///< Configures hardware timer interrupt.

protected:
	static TimeSlotManager * _pInstance;		///< Points to single instance.
	Observer * _pObserver;						///< Observer receiving time slot signals.
	TIM_HandleTypeDef * _pHardwareTimer;        ///< Hardware timer to be used for timings.

	/**
	 * Timeout identifier(s) for this state machine
	 */
	typedef enum
	{
		Timeout_SIM_BEACON_RECEIVED_id = 1	///< Timeout id to simulate a beacon reception.
	} eTimeoutId;

	/**
	 * Enumeration used to have a unique identifier for every
	 * state in the state machine.
	 */
	typedef enum
	{
		STATE_UNKOWN = 0,			///< Unknown state.
		STATE_INITIAL = 1,			///< Initial state.
		STATE_ROOT = 2				///< State where to wait for further events.
	} eMainState;

	eMainState _currentState;					///< Attribute indicating currently active state.
	desenet::SlotNumber _slotCounter;			///< Set to zero when receiving a beacon. Incremented at every start of a new slot.
	desenet::SlotNumber _slotNumber;			///< Slot number of this node. Needs to be set at creation time.
	desenet::SlotDuration _slotDuration;		///< Last slot duration received from gateway node.
};
} // namespace desenet

#endif // STM32_TIMESLOTMANAGER_H
