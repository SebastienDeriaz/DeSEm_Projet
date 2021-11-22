#ifndef BOARD_LEDCONTROLLER_H
#define BOARD_LEDCONTROLLER_H

#include "platform-common/board/ledflasher.h"

/*
 * @brief General access to board LEDs.
 *
 * For the Olimex STM-103STK board only on LED is present. It
 * can be access using LED index zero.
 */
class LedController
{
public:
	virtual ~LedController();
	
	static LedController & instance();			///< Reference to the joystick.

	bool initialize();							///< Initializes the LED controller.
	void start();								///< Start state machine(s).

	void setLed(int index, bool bEnable);		///< Enables/disables LED using given index.
	void flashLed(uint16_t index = 0);			///< Flashs LED for a short time.
	
protected:
	LedController();
	LedController(const LedController &) = default;				    ///< Forbids to copy an object.
	const LedController & operator =(const LedController & j);	    ///< Forbids to copy a joystick.

protected:
	bool _bInitialized;			///< True after the call to the #initialize method.

	LedFlasher _ledFlasher;		///< Used to flash LED for a short time.
};

#endif // BOARD_LEDCONTROLLER_H
