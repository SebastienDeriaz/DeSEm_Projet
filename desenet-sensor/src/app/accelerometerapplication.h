#ifndef ACCELEROMETERAPPLICATION_H
#define ACCELEROMETERAPPLICATION_H

#include "platform-config.h"
#include "xf/xfreactive.h"
#include "desenet/sensor/abstractapplication.h"

namespace board {
	class Accelerometer;
}

namespace app
{

/**
 * @brief Sends accelerometer values as sampled values to the gateway node.
 */
class AccelerometerApplication : public XFReactive,
								 public desenet::sensor::AbstractApplication
{
public:
	AccelerometerApplication();
	virtual ~AccelerometerApplication();

	void initialize();
	void start();

	// desenet::AbstractApplication callback implementation
protected:
	virtual void svSyncIndication(NetworkTime time);
	virtual desenet::SharedByteBuffer::sizeType svPublishIndication(desenet::SvGroup group, desenet::SharedByteBuffer & svData);


	// XFReactive implementation
protected:
	virtual EventStatus processEvent();		///< Implements the state machine.

protected:	// State machine stuff
	/**
	 * Event identifier(s) for this state machine
	 */
	typedef enum
	{
		EV_SV_SYNC_id = 1	///< Sampled values sync event.
	} eEventId;

	/**
	 * Enumeration used to have a unique identifier for every
	 * state in the state machine.
	 */
	typedef enum
	{
		STATE_UNKOWN = 0,			///< Unknown state
		STATE_INITIAL = 1,			///< Initial state
		STATE_ROOT = 2				///< State where to wait for further events
	} eMainState;

	eMainState _currentState;		///< Attribute indicating currently active state

protected:
	board::Accelerometer & accelerometer();

	void _readAccelerometerValues();				///< Reads actual values from accelerometer

protected:
	const desenet::SvGroup MY_GROUP = SVGROUP_ACCELEROMETER;
	int16_t _accelValuesSyncArray[3];		///< Acceleration values read at sync indication.
};

} // namespace app

#endif // ACCELEROMETERAPPLICATION_H
