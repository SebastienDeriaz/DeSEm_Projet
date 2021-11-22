#include <assert.h>
#include "platform-config.h"		// For CLOCKWORK_SHOWCURRENTTIME etc.
#include "trace/trace.h"
#include "xf/xftimeoutmanager.h"
#include "xf/xfevent.h"
#include "clockwork.h"

Clockwork * Clockwork::_pInstance;

Clockwork::Clockwork()
: _currentState(STATE_UNKOWN),
  _oldTick(0),
  _bCurrentTimeReceived(false)
{
	assert(!_pInstance);	// Only one instance allowed.
	_pInstance = this;
}

Clockwork::~Clockwork()
{
}

void Clockwork::start()
{
	startBehavior();	// Start state machine
}

const TTime & Clockwork::currentTime()
{
	doSecondUpdate();

	return _time;
}

Clockwork & Clockwork::instance()
{
	assert(_pInstance);
	return *_pInstance;
}

void Clockwork::setCurrentTime(unsigned char hours,
							   unsigned char minutes,
							   unsigned char seconds,
							   unsigned short milliseconds)
{
	if (_bCurrentTimeReceived)
	{
		long diffInMilliseconds;
		TTime currentTime;

		// Calculate drift in ms
		currentTime.setHMS(hours, minutes, seconds, milliseconds);
		currentTime.getDiffinMSec(_time, diffInMilliseconds);
	}

	// Adjust clock to current time received
	_time.setHMS(hours, minutes, seconds, milliseconds);

	// Update old tick value, otherwise the clock drifts slowly away
	_oldTick = XFTimeoutManager::getInstance()->sysTicksInMs();

	_bCurrentTimeReceived = true;
}

void Clockwork::setCurrentTime(const TTime & currentTime)
{
	unsigned char hours, minutes, seconds;
	unsigned short milliseconds;

	currentTime.getHMS(hours, minutes, seconds, milliseconds);
	setCurrentTime(hours, minutes, seconds, milliseconds);
}

void Clockwork::time(unsigned char & hours, unsigned char & minutes, unsigned char & seconds, unsigned short & milliseconds)
{
	// First update attributes
	doSecondUpdate();

	hours = _time.hour();
	minutes = _time.minute();
	seconds = _time.second();
	milliseconds = _time.msec();
}

EventStatus Clockwork::processEvent()
{
	eMainState newState = _currentState;

	switch (_currentState)
	{
	case STATE_UNKOWN:
	case STATE_INITIAL:
		if (getCurrentEvent()->getEventType() == IXFEvent::Initial)
		{
			newState = STATE_ROOT;	// Move to state ROOT

			doInit();

			// Start timeout
			getThread()->scheduleTimeout(SECOND_INTERVAL_id, 1000, this);
		}
		break;
	case STATE_ROOT:
		if (getCurrentEvent()->getEventType() == IXFEvent::Timeout &&
			getCurrentTimeout()->getId() == SECOND_INTERVAL_id)
		{
			newState = STATE_ROOT;	// Stay in state

			doSecondUpdate();

			// Start timeout
			getThread()->scheduleTimeout(SECOND_INTERVAL_id, 1000, this);
		}
		else if(getCurrentEvent()->getEventType() == IXFEvent::Event &&
				getCurrentEvent()->getId() == EV_MINUTE_ELAPSED_id)
		{
			doShowCurrentTime();
		}
		break;
	}

	_currentState = newState;		// Save new state to actual

	return EventStatus::Consumed;	// We consume all given events/timeouts
}

void Clockwork::doInit()
{
#if (CLOCKWORK_VERBOSE != 0)
	Trace::outln("Starting Clockwork (CW)");
	Trace::outln("CW: Framework tick interval: %d ms", XFTimeoutManager::getInstance()->tickInterval());
#endif

	// Update old tick value
	_oldTick = XFTimeoutManager::getInstance()->sysTicksInMs();
}

void Clockwork::doShowCurrentTime()
{
#if (CLOCKWORK_SHOWCURRENTTIME != 0)
	Trace::outln("Time: %02d:%02d:%02d.%03d",
			 	 _time.hour(),
			 	 _time.minute(),
			 	 _time.second(),
			 	 _time.msec());
#endif
}

void Clockwork::doSecondUpdate()
{
	uint32_t ticksElapsed = XFTimeoutManager::getInstance()->sysTicksInMs() - _oldTick;
	uint32_t millisecondsElapsed, msLong;
	uint8_t hours = _time.hour();
	uint8_t minutes = _time.minute();
	uint8_t seconds = _time.second();

	_oldTick += ticksElapsed;

	millisecondsElapsed = ticksElapsed;

	// Sum elapsed milliseconds
	msLong = millisecondsElapsed + _time.msec();

	if (msLong >= 3600000)
	{
		// Add first hours
		hours += msLong / 3600000;
		msLong %= 3600000;
	}

	if (msLong >= 60000)
	{
		// then minutes
		minutes += msLong / 60000;
		msLong %= 60000;
	}

	// Add seconds elapsed
	seconds += (msLong / 1000);
	msLong %= 1000;

	if (seconds >= 60)
	{
		// Increment minutes
		minutes += (seconds / 60);
		seconds %= 60;

		// Tell state machine one more minute elapsed
		pushEvent(new XFEvent(EV_MINUTE_ELAPSED_id));
	}

	if (minutes >= 60)
	{
		hours += (minutes / 60);
		minutes %= 60;
	}

	if (hours >= 24)
	{
		hours %= 24;
	}

	// Set calculated time
	_time.setHMS(hours,
			minutes,
			seconds,
			(uint16_t)msLong /* remaining milliseconds*/);
}
