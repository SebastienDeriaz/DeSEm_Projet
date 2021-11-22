
#include "desenet/networkinterfacedriver.h"

extern desenet::NetworkInterfaceDriver * instanciateNrfMeshsimDriver(uint32_t address, const std::string & name);

/*
 * @brief Provides a general method to instanciate a NIC.
 */
desenet::NetworkInterfaceDriver * instanciateNic(uint32_t address, const std::string & name)
{
	return instanciateNrfMeshsimDriver(address, name);
}
