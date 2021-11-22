#include "platform-config.h"
#if (JOYSTICK_TRACE_VERBOSE != 0)
	#include <trace/trace.h>
	#include <stdarg.h>
	#include <stdio.h>
#endif
#include "main.h"
#include "critical/critical.h"
#include "xf/xfstaticevent.h"
#include "joystick.h"

Joystick::Joystick()
{
	_bInitialized = false;
	_pObserver = NULL;
	_position.position = Invalid;
	_currentState = STATE_INITIAL;
	_timeoutActive = false;
}

Joystick::Joystick(const Joystick &)
 : XFReactive()
{
	_bInitialized = false;
	_pObserver = NULL;
	_position.position = Invalid;
	_currentState = STATE_INITIAL;
}

bool Joystick::initialize()
{
	if (!_bInitialized)
	{
		_bInitialized = true;

		return true;
	}
	return false;
}

bool Joystick::setObserver(IJoystickObserver * observer)
{
	_pObserver = observer;

	return true;
}

void Joystick::start()
{
	startBehavior();
}

//static
Joystick & Joystick::instance()
{
	static Joystick joystick;

	return joystick;
}

IJoystick::Position Joystick::position() const
{
	return _position;
}

Joystick::PressedButtons Joystick::_readActualPosition() const
{
    PressedButtons pressed = Invalid;

    // Check UP button pressed
    pressed |= (HAL_GPIO_ReadPin(NAVS_UP_GPIO_Port, NAVS_UP_Pin) == GPIO_PIN_RESET) ? JoystickButton::Up : JoystickButton::Invalid;

    // Check DOWN button pressed
    pressed |= (HAL_GPIO_ReadPin(NAVS_DOWN_GPIO_Port, NAVS_DOWN_Pin) == GPIO_PIN_RESET) ? JoystickButton::Down : JoystickButton::Invalid;

    // And also for the remaining buttons
    pressed |= (HAL_GPIO_ReadPin(NAVS_LEFT_GPIO_Port, NAVS_LEFT_Pin) == GPIO_PIN_RESET) ? JoystickButton::Left : JoystickButton::Invalid;
    pressed |= (HAL_GPIO_ReadPin(NAVS_RIGHT_GPIO_Port, NAVS_RIGHT_Pin) == GPIO_PIN_RESET) ? JoystickButton::Right : JoystickButton::Invalid;
    pressed |= (HAL_GPIO_ReadPin(NAVS_CENTER_GPIO_Port, NAVS_CENTER_Pin) == GPIO_PIN_RESET) ? JoystickButton::Pressed : JoystickButton::Invalid;

    return pressed;
}

EventStatus Joystick::processEvent()
{
	eEventStatus eventStatus = EventStatus::Unknown;

	switch (_currentState)
	{
	case STATE_INITIAL:
		{
			if (getCurrentEvent()->getEventType() == IXFEvent::Initial)
			{
				GEN(XFNullTransition());

				_currentState = STATE_CHECK_JOYSTICK;

				eventStatus = EventStatus::Consumed;
			}
		}
		break;
	case STATE_CHECK_JOYSTICK:
		{
		    // Check if we got an interrupt from the buttons (GPIOs)
		    if (getCurrentEvent()->getEventType() == IXFEvent::Event &&
		        getCurrentEvent()->getId() == Event_IRQ)
		    {
		          // Launch timeout if not already one is running
		          if (!_timeoutActive)
		          {
		              getThread()->scheduleTimeout(Timeout_CHECK_JOYSTICK_id, 50, this);
		          }

		          _timeoutActive = true;
		    }

			if (getCurrentEvent()->getEventType() == IXFEvent::NullTransition ||
				(getCurrentEvent()->getEventType() == IXFEvent::Timeout &&
				 getCurrentTimeout()->getId() == Timeout_CHECK_JOYSTICK_id))
			{
				{
					doCheckJoystickState();
				}

				_timeoutActive = false;
				_currentState = STATE_CHECK_JOYSTICK;
				eventStatus = EventStatus::Consumed;
			}
		}
		break;
	default:
		break;
	}

	return eventStatus;
}

void Joystick::doCheckJoystickState()
{
	// Store previously calculated value
	const Position oldPosition = _position;

	_position.pressedButtons =  _readActualPosition();

	//Trace::outln("Button state: 0x%0X", _position.pressedButtons);

	// Did the position of the joystick change?
	if (oldPosition.pressedButtons != _position.pressedButtons)
	{
#if (JOYSTICK_TRACE_VERBOSE != 0)

	    if ((oldPosition.pressedButtons & JoystickButton::Up) != (_position.pressedButtons & JoystickButton::Up))
	    {
            traceOut("Joystick button up %s", (_position.pressedButtons == JoystickButton::Up) ? "pressed" : "released");
	    }

        if ((oldPosition.pressedButtons & JoystickButton::Down) != (_position.pressedButtons & JoystickButton::Down))
        {
            traceOut("Joystick button down %s", (_position.pressedButtons == JoystickButton::Down) ? "pressed" : "released");
        }

        if ((oldPosition.pressedButtons & JoystickButton::Left) != (_position.pressedButtons & JoystickButton::Left))
        {
            traceOut("Joystick button left %s", (_position.pressedButtons == JoystickButton::Left) ? "pressed" : "released");
        }

        if ((oldPosition.pressedButtons & JoystickButton::Right) != (_position.pressedButtons & JoystickButton::Right))
        {
            traceOut("Joystick button right %s", (_position.pressedButtons == JoystickButton::Right) ? "pressed" : "released");
        }

        if ((oldPosition.pressedButtons & JoystickButton::Pressed) != (_position.pressedButtons & JoystickButton::Pressed))
        {
            traceOut("Joystick button center %s", (_position.pressedButtons == JoystickButton::Pressed) ? "pressed" : "released");
        }

#endif // (JOYSTICK_TRACE_VERBOSE != 0)

		// Notify observer
		if (_pObserver)
		{
			_pObserver->onPositionChange(position());
		}
	}
}

void Joystick::traceOut(const char * const format, ...)
{
#if (JOYSTICK_TRACE_VERBOSE != 0)
	static const unsigned int STR_SIZE = 64;
	static char str[STR_SIZE];

	va_list args;

	va_start(args, format);
	vsiprintf(str, format, args);
	va_end(args);

	assert(strlen(str) < STR_SIZE);		// Uups! str is too small

	Trace::outln(str);
#else
	(void)(format);
#endif
}

void Joystick::onIrq(uint16_t GPIO_Pin)
{
    static XFStaticEvent irqEvent(Event_IRQ);

    enterISR();
    pushEvent( &irqEvent );
    exitISR();
}
