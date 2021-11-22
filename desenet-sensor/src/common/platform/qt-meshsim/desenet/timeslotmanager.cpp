#include <QtCore/QTimerEvent>
#include <assert.h>
#include "platform-config.h"
#include "timeslotmanager.h"

const static uint32_t SIM_BEACON_INTERVAL_IN_MS = 2000;

TimeSlotManager * TimeSlotManager::_pInstance(nullptr);

TimeSlotManager::TimeSlotManager()
 : _pObserver(nullptr),
   _slotNumber(desenet::MAX_SLOT_NUMBER + 1),	// Set to something not valid
   _slotDuration(0),
   _timerId(0)
{
	assert(!_pInstance);	// Only one instance allowed
	_pInstance = this;
}

TimeSlotManager::~TimeSlotManager()
{

}

void TimeSlotManager::initialize(const desenet::SlotNumber & slotNumber)
{
	assert(slotNumber <= desenet::MAX_SLOT_NUMBER);		// Slot 0 to 31

	_slotNumber = slotNumber;
}

void TimeSlotManager::initializeRelations(const Observer & observer)
{
	setObserver(observer);
}

bool TimeSlotManager::setObserver(const TimeSlotManager::Observer & observer)
{
	if (_pObserver == nullptr)
	{
		_pObserver = const_cast<Observer*>(&observer);
		return true;
	}
	return false;
}

void TimeSlotManager::start()
{
	// Nothing to do on this platform
}

void TimeSlotManager::onBeaconReceived(const desenet::SlotDuration & slotDuration)
{
	assert(_slotNumber <= desenet::MAX_SLOT_NUMBER);		// Slot 0 to 31

	notify(SIG::CYCLE_START);

	_stopTimer();

	if (_slotDuration != slotDuration)
	{	// Slot duration has changed
		_setSlotDuration(slotDuration);
	}

	_startTimer();	// indicating start of our slot
}

void TimeSlotManager::notify(const SIG & signal)
{
	assert(_pObserver);
	if (_pObserver)
	{
		_pObserver->onTimeSlotSignal(*this, signal);
	}
}

void TimeSlotManager::_startTimer()
{
	_timerId = startTimer(_slotDuration * (_slotNumber + 1), Qt::PreciseTimer);
}

void TimeSlotManager::_setSlotDuration(const desenet::SlotDuration & slotDuration)
{
	_slotDuration = slotDuration;
}

void TimeSlotManager::_stopTimer()
{
	if (_timerId)
	{
		killTimer(_timerId);
		_timerId = 0;
	}
}

void TimeSlotManager::timerEvent(QTimerEvent * event)
{
	if (event->timerId() == _timerId)
	{
		killTimer(_timerId);	// Kill timer. We want only one shot.
		_timerId = 0;

		// Inform observers
		notify(OWN_SLOT_START);
	}
}
