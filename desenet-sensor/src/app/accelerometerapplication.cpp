#include "platform-config.h"
#include "trace/trace.h"
#include "factory.h"
#include "xf/xfevent.h"
#include "accelerometerapplication.h"

using app::AccelerometerApplication;

AccelerometerApplication::AccelerometerApplication()
: _currentState(STATE_UNKOWN)
{
	memset(_accelValuesSyncArray, 0, sizeof(_accelValuesSyncArray));
}

AccelerometerApplication::~AccelerometerApplication()
{
}

void AccelerometerApplication::initialize()
{
	// Register application to get informed about arrival of beacons.
	svSyncRequest();
	// Register to publish sampled values for a specific group.
	svPublishRequest(MY_GROUP);
}

void AccelerometerApplication::start()
{
	startBehavior();	// Start state machine
}

/**
 * Called by the network after reception of a beacon frame from gateway node.
 */
void AccelerometerApplication::svSyncIndication(NetworkTime time)
{
    (void)(time);
//	GEN(XFEvent(EV_SV_SYNC_id));	// Tell state machine about sync indication
// Using the state machine to read accelerometer values does not work for the moment. To
// let it work with it, NetworkEntity must also implement a state machine and call
// svPublishIndication()'s asynchronously!

	// For the moment, read accelerator values directly here
	_readAccelerometerValues();
}

desenet::SharedByteBuffer::sizeType AccelerometerApplication::svPublishIndication(desenet::SvGroup group, desenet::SharedByteBuffer & svData)
{
	if (group == MY_GROUP)
	{
		memcpy(&svData[0], &_accelValuesSyncArray[0], 2);
		memcpy(&svData[2], &_accelValuesSyncArray[1], 2);
		memcpy(&svData[4], &_accelValuesSyncArray[2], 2);

		return sizeof(_accelValuesSyncArray);
	}
	return 0;
}

EventStatus AccelerometerApplication::processEvent()
{
	eMainState newState = _currentState;

	switch (_currentState)
	{
	case STATE_UNKOWN:
	case STATE_INITIAL:
		if (getCurrentEvent()->getEventType() == IXFEvent::Initial)
		{
			newState = STATE_ROOT;	// Move to state ROOT
		}
		break;
	case STATE_ROOT:
		if (getCurrentEvent()->getEventType() == IXFEvent::Event &&
			getCurrentEvent()->getId() == EV_SV_SYNC_id)
		{
			newState = STATE_ROOT;	// Stay in state

			// Read actual values from accelerometer
//			_readAccelerometerValues();
// Reading accelerometer values in the state machine takes the values too late.
// This part of the state machine is NOT executed between beacon reception and svPublishIndication(). It
// is called AFTER the svPublishIndication() and hence too late. Values are transfered on network interval
// later.
// Fast solution is to get accelerometer values directly on beacon reception.
// Right solution would be to add to the NetworkEntity class a state machine and call svPublishIndication()
// in the state machine.

#if (ACCELEROMETER_APP_VERBOSE != 0)
			// Show actual accelerometer values
			Trace::outln("App: acc (%5d, %5d, %5d)", _accelValuesSyncArray[0], _accelValuesSyncArray[1], _accelValuesSyncArray[2]);
#endif
		}
		break;
	}

	_currentState = newState;		// Save new state to actual

	return EventStatus::Consumed;	// We consume all given events/timeouts
}

board::Accelerometer & AccelerometerApplication::accelerometer()
{
	return Factory::instance().accelerometer();
}

void AccelerometerApplication::_readAccelerometerValues()
{
	accelerometer().getAccelerationValues(_accelValuesSyncArray[0],	/* x */
										  _accelValuesSyncArray[1],	/* x */
										  _accelValuesSyncArray[2]	/* z */);
}
