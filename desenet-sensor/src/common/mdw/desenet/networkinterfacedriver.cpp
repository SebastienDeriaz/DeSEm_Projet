#include "networkinterfacedriver.h"

using std::vector;
using std::string;

desenet::NetworkInterfaceDriver::Descriptor::Descriptor()
{
    _descriptors().push_back(this);
}

desenet::NetworkInterfaceDriver::~NetworkInterfaceDriver()
{
}

vector<desenet::NetworkInterfaceDriver::Descriptor *> &desenet::NetworkInterfaceDriver::_descriptors()
{
    static vector<Descriptor *> __descriptors;
    return __descriptors;
}

vector<desenet::NetworkInterfaceDriver::Descriptor *> desenet::NetworkInterfaceDriver::interfaceDescriptors()
{
    return _descriptors();
}
