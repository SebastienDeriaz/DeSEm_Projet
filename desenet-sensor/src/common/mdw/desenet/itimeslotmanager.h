#ifndef ITIMESLOTMANAGER_H
#define ITIMESLOTMANAGER_H


#include "desenet/desenet.h"

/**
 * @brief Interface to be implemented by every TimeSlotManager
 *
 * This interface is needed by the desent::NetworkEnity.
 */
class ITimeSlotManager
{
public:
	enum SIG
	{
		CYCLE_START,		///< Indicates start of cycle (beacon received).
		CYCLE_FINISH,		///< End of cycle (last slot finished).
		OWN_SLOT_START,		///< Start of our slot.
		OWN_SLOT_FINISH		///< End of our slot.
	};

	/**
	 * @brief Interface to be implemented by the TimeSlotManager observer
	 */
	class Observer
	{
	public:
		/**
		 * @brief Method called on the observer to notify new signal.
		 *
		 * @param timeSlotManager TimeSlotManager who sends the signal.
		 * @param signal The signal raised.
		 */
		virtual void onTimeSlotSignal(const ITimeSlotManager & timeSlotManager, const SIG & signal) = 0;
	};

	// Methods to be implemented by the derived class.
	virtual void initialize(const desenet::SlotNumber & slotNumber) = 0;			///< Initializes the object.
	virtual void initializeRelations(const Observer & observer) = 0;				///< Initializes relations needed by the instance.
	virtual bool setObserver(const Observer & observer) = 0;						///< Sets the observer.
	virtual void start() = 0;														///< Starts the TimeSlotManager.

	virtual void onBeaconReceived(const desenet::SlotDuration & slotDuration) = 0;	///< Should be called whenever receiving a beacon frame from the gateway (master).

protected:
	// It is just an interface, so constructor and destructor should be not public.
	explicit ITimeSlotManager() {}
	virtual ~ITimeSlotManager() {}

};

#endif // ITIMESLOTMANAGER_H
