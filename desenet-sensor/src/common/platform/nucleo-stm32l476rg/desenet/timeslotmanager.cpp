#include <cassert>
#include "mcu/mcu.h"
#include "platform-config.h"
#include "timeslotmanager.h"

#ifndef USE_GPIOS_TO_SIGNAL_TIME_SLOTS
  #define USE_GPIOS_TO_SIGNAL_TIME_SLOTS 0
#endif

#ifndef TIME_SLOT_MANAGER_HARDWARE_TIMER_HANDLE
    // Default: Take hardware timer TIM2
    extern "C" TIM_HandleTypeDef                        htim2;      /* Defined in main.c   */
    #define TIME_SLOT_MANAGER_HARDWARE_TIMER_HANDLE     htim2       /* Hardware timer TIM2 */
#endif // TIME_SLOT_MANAGER_HARDWARE_TIMER_HANDLE

const static uint32_t SIM_BEACON_INTERVAL_IN_MS = 2000;

namespace desenet {

TimeSlotManager * TimeSlotManager::_pInstance(nullptr);

TimeSlotManager::TimeSlotManager()
 : _pObserver(nullptr),
   _pHardwareTimer(&TIME_SLOT_MANAGER_HARDWARE_TIMER_HANDLE),
   _currentState(STATE_UNKOWN),
   _slotCounter(0),
   _slotNumber(desenet::MAX_SLOT_NUMBER + 1),	// Set to something not valid
   _slotDuration(0)
{
	assert(!_pInstance);	// Only one instance allowed
	_pInstance = this;

	assert(_pHardwareTimer);    // Must not be nullptr
}

TimeSlotManager::~TimeSlotManager()
{

}

void TimeSlotManager::initialize(const desenet::SlotNumber & slotNumber)
{
	assert(slotNumber <= desenet::MAX_SLOT_NUMBER);		// Slot 0 to 31

	HAL_TIM_Base_DeInit(_pHardwareTimer);

	_slotNumber = slotNumber;

#if (USE_GPIOS_TO_SIGNAL_TIME_SLOTS != 0)
	/*
	 * Use PB12 and PB13 to indicate time slot info
	 * PB12 -> UEXT-10
	 * PB13 -> UEST-09
	 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef gpioStructure;
	gpioStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
	gpioStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	gpioStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpioStructure);

	// Set pins to zero.
	GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
	GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET);

#endif // USE_GPIOS_TO_SIGNAL_TIME_SLOTS
}

void TimeSlotManager::initializeRelations(const Observer & observer)
{
	setObserver(observer);
}

bool TimeSlotManager::setObserver(const TimeSlotManager::Observer & observer)
{
	if (_pObserver == nullptr)
	{
		_pObserver = const_cast<Observer*>(&observer);
		return true;
	}
	return false;
}

void TimeSlotManager::start()
{
	startBehavior();
}

/*
 * Beacons & slots:         B    S0    S1    S2    S3    S4    S5
 *               1                   +-----+     +-----+     +-----+
 *                                   |     |     |     |     |     |
 * GPIO_Pin_12:  0    ---+-----+-----+     +-----+     +-----+     +...
 * _slotCounter:         0     0     1     2     3     4     5     6
 */

void TimeSlotManager::onBeaconReceived(const desenet::SlotDuration & slotDuration)
{
	assert(_slotNumber <= desenet::MAX_SLOT_NUMBER);		// Slot 0 to 31

	notify(SIG::CYCLE_START);

	_stopTimer();

	_slotCounter = 0;	// Restart counting of slots
#if (USE_GPIOS_TO_SIGNAL_TIME_SLOTS != 0)
	GPIO_WriteBit(GPIOB, GPIO_Pin_12, (BitAction)(_slotCounter % 2));	// Indicate start of beacon
#endif

	if (_slotDuration != slotDuration)
	{	// Slot duration has changed
		_setSlotDuration(slotDuration);
	}

	_startTimer();
}

//static
void TimeSlotManager::onIrq()
{	// Called within IRQ!
	instance()._processIrq();
}

void TimeSlotManager::notify(const SIG & signal)
{
	assert(_pObserver);
	if (_pObserver)
	{
		_pObserver->onTimeSlotSignal(*this, signal);
	}
}

void TimeSlotManager::_startTimer()
{
    assert(_slotCounter == 0);                              // Slot counter must be 0

    __HAL_TIM_SET_COUNTER(_pHardwareTimer, 0);              // Reset hardware counter to 0
    __HAL_TIM_CLEAR_IT(_pHardwareTimer, TIM_IT_CC1);        // Clear output compare interrupt

    HAL_TIM_OC_Start_IT(_pHardwareTimer, TIM_CHANNEL_1);    // Start timer and enable interrupts
}

void TimeSlotManager::_setSlotDuration(const desenet::SlotDuration & slotDuration)
{
	uint32_t timClk;
	uint32_t period = 0;
	uint32_t prescaler = 0;
	HAL_StatusTypeDef status;

	assert(slotDuration > 0);

	_slotDuration = slotDuration;

	HAL_TIM_Base_DeInit(_pHardwareTimer);
	timClk = HAL_RCC_GetPCLK1Freq();

	// Formula:
	// slotDuration = period * (prescaler + 1) / timClk
	//
	// The prescaler slows down the clock frequency
	// The period defines how many ticks the timer should count
	// to reach the desired interval (slotDuration).

	// Calculate the prescaler
	period = 0xFFFF;	// Set temporary the period to the highest value
	prescaler = (timClk / 1000 * slotDuration / period) - 1;

	if (prescaler <= 0xFFFF)
	{
		period = timClk / 1000 * slotDuration / (prescaler + 1);

		if (period > 0xFFFF)	// Check if calculated period is too high
		{
			prescaler++;	// Push prescaler to the next value.
			period = timClk / 1000 * slotDuration / (prescaler + 1);
			// Now the period must to be in the right range
		}
	}
	else
	{
		assert(false); // Slot duration is too high.
	}

	assert(period <= 0xFFFFUL);	// Error: Not able to provide the time slot duration needed

    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

	// Configure hardware timer
    _pHardwareTimer->Instance = TIM2;
    _pHardwareTimer->Init.Prescaler = prescaler;
    _pHardwareTimer->Init.CounterMode = TIM_COUNTERMODE_UP;
    _pHardwareTimer->Init.Period = period;
    _pHardwareTimer->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    _pHardwareTimer->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    status = HAL_TIM_Base_Init(_pHardwareTimer);
    assert(status == HAL_OK);

    status = HAL_TIM_OC_Init(_pHardwareTimer);
    assert(status == HAL_OK);

    // Enable 'output compare 1' interrupt event
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC1REF;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    status = HAL_TIMEx_MasterConfigSynchronization(_pHardwareTimer, &sMasterConfig);
    assert(status == HAL_OK);

    sConfigOC.OCMode = TIM_OCMODE_TIMING;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    status = HAL_TIM_OC_ConfigChannel(_pHardwareTimer, &sConfigOC, TIM_CHANNEL_1);
    assert(status == HAL_OK);
}

void TimeSlotManager::_stopTimer()
{
    HAL_TIM_OC_Stop_IT(_pHardwareTimer, TIM_CHANNEL_1);
}

void TimeSlotManager::_processIrq()
{
#if (USE_GPIOS_TO_SIGNAL_TIME_SLOTS != 0)
	GPIO_WriteBit(GPIOB, GPIO_Pin_12, (BitAction)(_slotCounter % 2));	// Toggle pin each time
#endif

	if (_slotCounter == _slotNumber)				// Start of your slot
	{
#if (USE_GPIOS_TO_SIGNAL_TIME_SLOTS != 0)
	GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_SET);
#endif
		notify(SIG::OWN_SLOT_START);
	}
	else if (_slotCounter == (_slotNumber + 1))		// End of our slot
	{
#if (USE_GPIOS_TO_SIGNAL_TIME_SLOTS != 0)
	GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET);
#endif
		notify(SIG::OWN_SLOT_FINISH);
	}

	// Stop hardware timer after counting all time slots
	if (_slotCounter >= (desenet::MAX_SLOT_NUMBER + 1))
	{
		notify(SIG::CYCLE_FINISH);

		_stopTimer();
	}

	_slotCounter++;	// Increment slotCounter for the next slot
}

EventStatus TimeSlotManager::processEvent()
{
	eMainState newState = _currentState;

	switch (_currentState)
	{
	case STATE_UNKOWN:
	case STATE_INITIAL:
		if (getCurrentEvent()->getEventType() == IXFEvent::Initial)
		{
			newState = STATE_ROOT;	// Move to state ROOT

			// Start timeout
			getThread()->scheduleTimeout(Timeout_SIM_BEACON_RECEIVED_id, SIM_BEACON_INTERVAL_IN_MS, this);
		}
		break;
	case STATE_ROOT:
		if (getCurrentEvent()->getEventType() == IXFEvent::Timeout &&
			getCurrentTimeout()->getId() == Timeout_SIM_BEACON_RECEIVED_id)
		{
			newState = STATE_ROOT;	// Stay in state

#if (TIMESLOTMANAGER_SIMULATE_BEACON != 0)
			// Restart timeout
			getThread()->scheduleTimeout(Timeout_SIM_BEACON_RECEIVED_id, SIM_BEACON_INTERVAL_IN_MS, this);

			// Simulate we got a beacon
			onBeaconReceived(2000 /*ms*/);
#endif // TIMESLOTMANAGER_SIMULATE_BEACON
		}
		break;
	}

	_currentState = newState;		// Save new state to actual

	return EventStatus::Consumed;	// We consume all given events/timeouts
}

} // namespace desenet
