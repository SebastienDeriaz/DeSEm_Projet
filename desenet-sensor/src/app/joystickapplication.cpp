#include "joystickapplication.h"

#include "factory.h"
#include "platform-config.h"
#include "trace/trace.h"
#include "xf/xfevent.h"
#include "joystick.h"

using app::JoystickApplication;

JoystickApplication::JoystickApplication() : _currentState(STATE_UNKOWN) {
    joystickData = 0;
}

JoystickApplication::~JoystickApplication() {}

void JoystickApplication::initialize() {
    // Register application to get informed about arrival of beacons.
    svSyncRequest();
    // Register to publish sampled values for a specific group.
    // svPublishRequest(MY_GROUP);
}

void JoystickApplication::start() {
    startBehavior();  // Start state machine
}

/**
 * Called by the network after reception of a beacon frame from gateway node.
 */
void JoystickApplication::svSyncIndication(NetworkTime time) {
    (void)(time);
    //	GEN(XFEvent(EV_SV_SYNC_id));	// Tell state machine about sync
    // indication
    // Using the state machine to read accelerometer values does not work for
    // the moment. To let it work with it, NetworkEntity must also implement a
    // state machine and call svPublishIndication()'s asynchronously!

    // For the moment, read accelerator values directly here
    //_readAccelerometerValues();
}

void JoystickApplication::onPositionChange(IJoystick::Position position) {
    GEN(XFEvent(POSITION_CHANGED_EVENT))
}

EventStatus JoystickApplication::processEvent() {
    eMainState newState = _currentState;
    IJoystick::Position position;
    auto buffer = SharedByteBuffer(1);
    
    

    switch (_currentState) {
        case STATE_UNKOWN:
        case STATE_INITIAL:
            if (getCurrentEvent()->getEventType() == IXFEvent::Initial) {
                newState = STATE_ROOT;  // Move to state ROOT
            }
            break;
        case STATE_ROOT:
            if (getCurrentEvent()->getEventType() == IXFEvent::Event &&
                getCurrentEvent()->getId() == POSITION_CHANGED_EVENT) {
                newState = STATE_POSITION_CHANGED;  // Stay in state
            }
            break;
        case STATE_POSITION_CHANGED:
            newState = STATE_ROOT;
            break;
    }

    if (newState != _currentState) {
        switch (newState) {
            case STATE_ROOT:
            case STATE_UNKOWN:
            case STATE_INITIAL:
                break;
            case STATE_POSITION_CHANGED:
                position = joystick().position();
                joystickData = position.pressedButtons;
                buffer[0] = joystickData;
                evPublishRequest(EVID_JOYSTICK, buffer);
				GEN(XFNullTransition()); //Return to base state
                break;
        }
    }

    _currentState = newState;  // Save new state to actual

    return EventStatus::Consumed;  // We consume all given events/timeouts
}

board::Joystick& JoystickApplication::joystick() {
    return Factory::instance().joystick();
}
