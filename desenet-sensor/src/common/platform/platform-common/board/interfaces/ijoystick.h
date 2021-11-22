#pragma once

#include <cstdint>

class IJoystickObserver;

/**
 * @ingroup interfaces
 * @brief This interface represents a joystick that can be moved into four directions and and at the center it can be
 * 		  pressed down.
 *
 * This leads to six possible states of the joystick. The state of the joystick can be polled or
 * an JoystickObserver can be registered to get joystick events. Note that just one observer is supported and
 * if the setObserver() method is called a second time, the first instance of the observer will be dropped.
 */
class IJoystick
{
protected:
    // It is just an interface, so constructor and destructor should be not public.
    explicit IJoystick() {}
    virtual ~IJoystick() {}

public:
    /**
     * @brief Initializes all subsystems and the object itself needed for proper joystick operation.
     *
     * @return True if the joystick could be initialized and is ready to be used, false otherwise.
     */
    virtual bool initialize() = 0;

    /**
     * @brief Sets the observer that will receive events upon joystick position changes.
     *
     * Note that the object pointed by observer has to implement the IJoystickObserver interface.
     * If already an observer was registered before, the old one is discarded, since only one observer can
     * get joystick events at the same time.
     *
     * @param 	observer Pointer to the observer object.
     * @return 	True if the observer was accepted, false if it was disabled (observer=NULL) or on error.
     */
    virtual bool setObserver( IJoystickObserver * observer ) = 0;

    virtual void start() = 0;	///< Starts the state machine of the joystick

    /**
     * @brief Defines the actual position of the joystick. Note that depending on the hardware, the joystick
     * can be at multiple position at the same time. That is why the positions are coded as a bit mask.
     */
    enum JoystickButton
    {
        Invalid	= 0x00 ,	///< @brief The joystick value is invalid.
        Center 	= 0x01 ,	///< @brief The joystick is centred (idle).
        Left 	= 0x02 ,	///< @brief The joystick has been moved to or is at the left.
        Right	= 0x04 ,	///< @brief The joystick has been moved to or is at the right.
        Up		= 0x08 ,	///< @brief The joystick has been moved upwards or is in the up position.
        Down	= 0x10 ,	///< @brief The joystick has been moved downwards or is in down position.
        Pressed = 0x20		///< @brief The joystick has been pressed at the center.
    };

    typedef uint32_t PressedButtons;

    union Position
    {
        JoystickButton position;
        PressedButtons pressedButtons;
    };

    /**
     * @brief Returns the actual position of the joystick.
     *
     * @return The actual position of the joystick.
     */
    virtual Position position() const = 0;

    /**
     * @brief Convenience method, that converts the Joystick object into its actual value.
     *
     * @return The actual position of the joystick.
     */
    inline operator Position() const
    {
        return position();
    }
};
