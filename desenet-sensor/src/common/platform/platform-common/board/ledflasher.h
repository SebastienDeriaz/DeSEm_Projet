#ifndef BOARD_LEDFLASHER_H
#define BOARD_LEDFLASHER_H

#include "stdint.h"
#include "xf/xfreactive.h"

class LedController;

/**
 * @brief Implements state machine to let an LED blink for a given time.
 */
class LedFlasher : public XFReactive
{
	friend class LedController;

protected:
	LedFlasher(uint16_t ledIndex, LedController & ledController);
	virtual ~LedFlasher() {}

	void initialize(uint32_t flashTimeInMilliseconds = 100);
	void start();

	void flashLed();

	// XFReactive implementation
protected:
	virtual EventStatus processEvent();		///< Implements the state machine.

protected:	// State machine stuff
	/**
	 * Timeout identifier(s) for this state machine
	 */
	typedef enum
	{
		TM_LED_FLASH_id = 1		///< Flash time.
	} eTimeoutId;

	/**
	 * Event identifier(s) for this state machine
	 */
	typedef enum
	{
		EV_LED_FLASH_id = 1			///< LED flash event.
	} eEventId;

	/**
	 * Enumeration used to have a unique identifier for every
	 * state in the state machine.
	 */
	typedef enum
	{
		STATE_UNKOWN = 0,			///< Unknown state
		STATE_INITIAL = 1,			///< Initial state
		STATE_WAIT_FLASH = 2,		///< State wait for a flash
		STATE_LED_FLASH = 3			///< LED flash
	} eMainState;

	eMainState _newState;		///< Attribute indicating currently active state

protected:
	LedController & ledController() { return _ledController; }

protected:
	const uint16_t _ledIndex;
	LedController & _ledController;
	uint32_t _flashTimeInMilliseconds;
};

#endif // BOARD_LEDFLASHER_H
