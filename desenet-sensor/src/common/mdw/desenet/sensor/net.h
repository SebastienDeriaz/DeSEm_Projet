#ifndef NET_H
#define NET_H

#include "desenet/desenet.h"
#include "desenet/sensor/networkentity.h"
#include "desenet/networkinterfacedriver.h"
#include "desenet/timeslotmanager.h"

using desenet::sensor::NetworkEntity;

namespace desenet {
namespace sensor {

/**
 * @brief Represents the nodes network.
 *
 * Platform dependent implementation of the network.
 */
class Net
{
public:
	Net();
	virtual ~Net();

	void initialize(const desenet::SlotNumber & slotNumber);	///< Initialises the object.
	void start();												///< Starts the network and its components.

	static Net & instance();									///< Reference to single net instance.
	inline NetworkEntity & entity() { return _networkEntity; }	///< Reference to network entity.

public:
	NetworkEntity _networkEntity;								///< Used by the application layer to communicate to the gateway.
	desenet::NetworkInterfaceDriver * _pNetworkInterfaceDriver;	///< Pointer to network interface driver.
	TimeSlotManager _timeSlotManager;							///< Time slot manager providing time slot events.

protected:
	static Net * _pInstance;									///< Pointer to single instance.
};

} // sensor
} // desenet
#endif // NET_H
