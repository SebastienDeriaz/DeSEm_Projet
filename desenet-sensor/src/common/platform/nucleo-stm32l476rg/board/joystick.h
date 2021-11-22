#ifndef BOARD_JOYSTICK_H
#define BOARD_JOYSTICK_H

#include "platform-config.h"			// Defines JOYSTICK_TRACE_VERBOSE
#include "xf/xfreactive.h"
#include "board/interfaces/ijoystickobserver.h"

extern "C" void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

/**
 * @brief Joystick representing the 4 way joystick with push button on the Olimex STM-103STK board.
 *
 * The class who wants to get informed about joystick changes needs to implement the
 * IJoystickObserver interface.
 *
 * This class implements the singleton pattern.
 */
class Joystick : public IJoystick, public XFReactive
{
    friend void ::HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
public:
	bool initialize();									///< Initializes the joystick instance.
	bool setObserver(IJoystickObserver * observer);		///< Sets the joystick observer.
	void start();										///< Starts the state machine of the joystick.
	static Joystick & instance();						///< Reference to the joystick.
	Position position() const;							///< Actual position of the joystick.

protected:
	Joystick();
	Joystick(const Joystick &);							///< Forbids to copy an object.
	const Joystick & operator =(const Joystick & j) { (void)(j); return j; }	///< Forbids to copy a joystick.

	PressedButtons _readActualPosition() const;         ///< Reads the button state on the corresponding GPIOs.

	// From XFReactive
	virtual EventStatus processEvent();

	// Method(s) called in state machine
	void doCheckJoystickState();

	static void traceOut(const char * const format, ...);

	void onIrq(uint16_t GPIO_Pin);

protected:
	bool _bInitialized;
	IJoystickObserver * _pObserver;
	Position _position;

	/**
	 * Values to compare measured values by the ADC
	 */
	typedef enum
	{
		UP_VALUE        = 960,
		DOWN_VALUE      = 190,
		LEFT_VALUE      = 1990,
		RIGHT_VALUE     = 470,
		DIVERSION       =  30
	}	eValue;

	/**
	 * Custom event identifier(s)
	 */
	typedef enum
	{
	    Event_IRQ = 1
	} eEventId;

	/**
	 * Timeout identifier(s) for this state machine
	 */
	typedef enum
	{
		Timeout_CHECK_JOYSTICK_id = 1	///< Timeout id for CHECK_JOYSTICK state
	} eTimeoutId;

	/**
	 * Enumeration used to have a unique identifier for every
	 * state in the state machine.
	 */
	typedef enum
	{
		STATE_UNKOWN = 0,			///< Unknown state
		STATE_INITIAL = 1,			///< Initial state
		STATE_CHECK_JOYSTICK = 2	///< LED_ON state
	} eMainState;

	eMainState _currentState;		///< Attribute indicating currently active state
	bool _timeoutActive;
};

#endif // BOARD_JOYSTICK_H

