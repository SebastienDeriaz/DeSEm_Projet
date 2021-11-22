#pragma once

#include "xf/xfreactive.h"
#include "ttime.h"

/** @brief Returns time reference
 *
 * Returns actual time (hours:minutes:seconds) since startup of the device or
 * when using setCurrentTime() the actual time.
 *
 * This class implements the singleton pattern.
 *
 */
class Clockwork : public XFReactive
{
public:
	Clockwork();
	~Clockwork();

	void initialize() {}
	void start();

	const TTime & currentTime();							///< Returns actual time as time object.
	static Clockwork & instance();							///< Reference to single instance.
	void setCurrentTime(unsigned char hours,
						unsigned char minutes,
						unsigned char seconds,
						unsigned short milliseconds = 0);	///< Sets/adjusts the clockwork.
	void setCurrentTime(const TTime & currentTime);			///< Sets/adjusts the clockwork.
	void time(unsigned char & hours,
			  unsigned char & minutes,
			  unsigned char & seconds,
			  unsigned short & milliseconds);				///< Returns actual time as parameters.
	inline const TTime & time() { return currentTime(); }	///< Returns actual time as time object.

	// XFReactive implementation
protected:
	virtual EventStatus processEvent();		///< Implements the state machine.

protected:
	virtual void doInit();
	virtual void doSecondUpdate();
	virtual void doShowCurrentTime();

protected:	// State machine types and attributes
	/**
	 * Timeout identifier(s) for this state machine
	 */
	typedef enum
	{
		SECOND_INTERVAL_id = 1	///< Second interval.
	} eTimeoutId;

	/**
	 * Event identifier(s) for this state machine
	 */
	typedef enum
	{
		EV_MINUTE_ELAPSED_id = 1	///< Minute elapsed event.
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

	eMainState _currentState;				///< Attribute indicating currently active state

protected:
	uint64_t _oldTick;									///< Holds the from the execution framework previously read tick value.
	TTime _time;										///< Holds the calculated time.
	bool _bCurrentTimeReceived;							///< Gets true after call of setCurrentTime()
	static Clockwork * _pInstance;						///< Pointer to single instance.
};

