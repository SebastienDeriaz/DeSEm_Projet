#include "platform-config.h"
#include "net.h"

// External function in order to instantiate a NIC.
extern desenet::NetworkInterfaceDriver *instanciateNic(uint32_t address, const std::string &name);

namespace desenet {
namespace sensor {

Net * Net::_pInstance(nullptr);		// Instantiation of static attribute

Net::Net()
 : _pNetworkInterfaceDriver(nullptr)
{
    assert(!_pInstance);		// Only one instance allowed
    _pInstance = this;
}

Net::~Net()
{
}

void Net::initialize(const desenet::SlotNumber & slotNumber)
{
    char sensorName[16];

    _networkEntity.initialize();
    _timeSlotManager.initialize(slotNumber);

    snprintf(sensorName, 16, "Sensor %d", slotNumber);

    _pNetworkInterfaceDriver = instanciateNic(*(uint32_t *)SENSOR_ADDRESS.data(), sensorName);
    _pNetworkInterfaceDriver->initialize();

    _networkEntity.initializeRelations(_timeSlotManager, *_pNetworkInterfaceDriver);

    // TODO: Add additional initialization code here
}

void Net::start()
{
    _timeSlotManager.start();
}

/**
 * This method does not automatically create an instance if there is none created so far.
 * It is up the the developer to create an instance of this class prior to access the instance() method.
 */
//static
Net & Net::instance()
{
    assert(_pInstance);
    return *_pInstance;
}

} // sensor
} // desenet
