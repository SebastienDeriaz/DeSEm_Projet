#pragma once
#include "ijoystick.h"

/**
 * @ingroup interfaces
 * @brief This interface has to be implemented by all objects which want to register them self as observer at an object
 * 		  implementing the IJoystick interface.
 *
 * Any time the position of the joystick changes, the onPositionChange() method will be called. Attention, the context
 * in which the observers method onPositionChange() will be called is not defined and can be another thread or even a
 * interrupt service routine. Take care of that.
 */
class IJoystickObserver
{
protected:
	// It is just an interface, so constructor and destructor should be not public.
	explicit IJoystickObserver() {}
	virtual ~IJoystickObserver() {}

public:
	/**
	 * @brief This method is called by the object implementing the IJoystck interface whenever the position
	 * of the hardware joystick changed.
	 *
	 * Note that the context in which this method will be called is not defined
	 * and can be another thread or an interrupt service routine. To avoid problems, do not use the new()
	 * method or the malloc() method inside this method and do not call methods that may use these. Write
	 * just as much code as needed inside the method.
	 *
	 * @param position The actual position of the joystick.
	 */
	virtual void onPositionChange( IJoystick::Position position ) = 0;
};
