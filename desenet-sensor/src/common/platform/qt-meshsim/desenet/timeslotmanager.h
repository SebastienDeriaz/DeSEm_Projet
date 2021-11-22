#ifndef MESHSIM_TIMESLOTMANAGER_H
#define MESHSIM_TIMESLOTMANAGER_H

#include <assert.h>
#include <QtCore/QObject>
#include "desenet/itimeslotmanager.h"

/**
 * @brief TimeSlotManager provides to correct time for the time slots.
 *
 * Only one instance if this class is allowed (Singleton pattern).
 */
class TimeSlotManager : public ITimeSlotManager,
						protected QObject
{

public:
	TimeSlotManager();
	~TimeSlotManager();

	static ITimeSlotManager & instance() { assert(_pInstance); return *_pInstance; }

	// ITimeSlotManager interface implementation
public:
	virtual void initialize(const desenet::SlotNumber & slotNumber);
	virtual void initializeRelations(const Observer & observer);
	virtual bool setObserver(const Observer & observer);
	virtual void start();

	virtual void onBeaconReceived(const desenet::SlotDuration & slotDuration);

protected:
	void notify(const SIG & signal);		///< Notifies observers about changes.

protected:
	void _startTimer();													///< Starts the hardware timer.
	void _setSlotDuration(const desenet::SlotDuration & slotDuration);	///< Configures the hardware timer with the right time.
	void _stopTimer();													///< Starts the hardware timer.
	void _processIrq();													///< Called every time an IRQ arrives. Used to handle time slots.

	virtual void timerEvent(QTimerEvent * event);						///< Callback method for timer

protected:
	static TimeSlotManager * _pInstance;		///< Points to single instance.
	Observer * _pObserver;						///< Observer receiving time slot signals.
	desenet::SlotNumber _slotNumber;			///< Slot number of this node. Needs to be set at creation time.
	desenet::SlotDuration _slotDuration;		///< Last slot duration received from gateway node.
	int32_t _timerId;
};

#endif // MESHSIM_TIMESLOTMANAGER_H
