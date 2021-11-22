#pragma once

#include "platform-config.h"			// Defines JOYSTICK_TRACE_VERBOSE
#include "xf/xfreactive.h"
#include "interfaces/ijoystick.h"

class MeshSimBoard;

/**
 * @brief Joystick representing the 4 way joystick with push button on the Olimex STM-103STK board.
 *
 * The class who wants to get informed about joystick changes needs to implement the
 * IJoystickObserver interface.
 *
 * This class implements the singleton pattern.
 */
class Joystick : public IJoystick
{
public:
	bool initialize();									///< Initializes the joystick instance.
	bool setObserver(IJoystickObserver * observer);		///< Sets the joystick observer.
	void start();										///< Starts the state machine of the joystick.
	static Joystick & instance();						///< Reference to the joystick.
	Position position() const;							///< Actual position of the joystick.

protected:
	Joystick();
	Joystick(const Joystick &);							///< Forbids to copy an object.
	const Joystick & operator =(const Joystick & j) { UNUSED(j); return j; }	///< Forbids to copy an application.

	MeshSimBoard & meshSimBoard() const;				///< Internal calls are delegated to MeshSimBoard.

protected:
	bool _bInitialized;
	IJoystickObserver * _pObserver;

};
