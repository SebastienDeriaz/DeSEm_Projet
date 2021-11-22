#include "xf/xfevent.h"
#include "board/ledcontroller.h"
#include "ledflasher.h"

LedFlasher::LedFlasher(uint16_t ledIndex, LedController & ledController)
 : _newState(STATE_UNKOWN),
   _ledIndex(ledIndex),
   _ledController(ledController),
   _flashTimeInMilliseconds(100)
{
}

void LedFlasher::initialize(uint32_t flashTimeInMilliseconds /* = 100 */)
{
	_flashTimeInMilliseconds = flashTimeInMilliseconds;
}

void LedFlasher::start()
{
	XFReactive::startBehavior();
}

void LedFlasher::flashLed()
{
	GEN(XFEvent(EV_LED_FLASH_id));
}

EventStatus LedFlasher::processEvent()
{
	const eMainState lastState = _newState;

	// Action on transition switch
	switch (lastState)
	{
	case STATE_UNKOWN:
	case STATE_INITIAL:
		if (getCurrentEvent()->getEventType() == IXFEvent::Initial)
		{
			_newState = STATE_WAIT_FLASH;
		}
		break;
	case STATE_WAIT_FLASH:
		if (getCurrentEvent()->getEventType() == IXFEvent::Event &&
			getCurrentEvent()->getId() == EV_LED_FLASH_id)
		{
			_newState = STATE_LED_FLASH;
		}
		break;
	case STATE_LED_FLASH:
		if (getCurrentEvent()->getEventType() == IXFEvent::Timeout &&
			getCurrentTimeout()->getId() == TM_LED_FLASH_id)
		{
			_newState = STATE_WAIT_FLASH;
		}
		break;
	}

	if (lastState != _newState)
	{
		// Action on entry switch
		switch (_newState)
		{
		case STATE_WAIT_FLASH:
			{
				ledController().setLed(_ledIndex, false);
			}
			break;
		case STATE_LED_FLASH:
			{
				getThread()->scheduleTimeout(TM_LED_FLASH_id, _flashTimeInMilliseconds, this);

				ledController().setLed(_ledIndex, true);
			}
			break;
		default:
			break;
		}
	}

	return EventStatus::Consumed;	// We consume all given events/timeouts
}
