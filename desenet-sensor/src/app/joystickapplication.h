#ifndef JOYSTICKAPPLICATION_H
#define JOYSTICKAPPLICATION_H

#include "platform-config.h"
#include "xf/xfreactive.h"
#include "desenet/sensor/abstractapplication.h"
#include "../common/platform/platform-common/board/interfaces/ijoystickobserver.h"

namespace board {
	class Joystick;
}

/*
*  DeSEm - Sébastien Deriaz - 06.12.2021
*/

namespace app
{

/**
 * @brief Sends joystick values as sampled values to the gateway node.
 */
class JoystickApplication : public XFReactive,
								 public desenet::sensor::AbstractApplication, public IJoystickObserver
{
public:
	JoystickApplication();
	virtual ~JoystickApplication();

	void initialize();
	void start();

	// desenet::AbstractApplication callback implementation
protected:
	virtual void svSyncIndication(NetworkTime time);


	// XFReactive implementation
protected:
	virtual EventStatus processEvent();		///< Implements the state machine.

protected:	// State machine stuff
	/**
	 * Event identifier(s) for this state machine
	 */
	typedef enum
	{
		POSITION_CHANGED_EVENT = 1	///< Sampled values sync event.
	} eEventId;

	/**
	 * Enumeration used to have a unique identifier for every
	 * state in the state machine.
	 */
	typedef enum
	{
		STATE_UNKOWN = 0,			///< Unknown state
		STATE_INITIAL = 1,			///< Initial state
		STATE_ROOT = 2,				///< State where to wait for further events
		STATE_POSITION_CHANGED = 3
	} eMainState;

	eMainState _currentState;		///< Attribute indicating currently active state

protected:
	board::Joystick & joystick();

	void _readJoystickValue();				///< Reads actual values from accelerometer

	void onPositionChange( IJoystick::Position position );

protected:
	uint8_t joystickData;		///< Acceleration values read at sync indication.
};

} // namespace app

#endif // JOYSTICKAPPLICATION_H
