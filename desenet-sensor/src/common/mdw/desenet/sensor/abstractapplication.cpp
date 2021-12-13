#include "desenet/sensor/net.h"
#include "abstractapplication.h"

//#include <QDebug>

using desenet::sensor::AbstractApplication;

/*********************************************************************************************************
* TODO: Implement the protected methods of the AbstractApplication class here. From these you have to    *
* call the different methods on your DESENET Controller Entity implementation...                         *
*********************************************************************************************************/

// Default implementations.

void AbstractApplication::svSyncRequest()
{
    // TODO: Register application using the network entity
}

bool AbstractApplication::svPublishRequest(SvGroup group)
{
    return NetworkEntity::instance().subscribeToSvGroup(*this, group);
}

void AbstractApplication::evPublishRequest(const EvId& id, const SharedByteBuffer & evData)
{
    //qDebug() << "calling eventReceived with " << QByteArray((char*)evData.data(), evData.length());
    NetworkEntity::instance().eventReceived(id, evData);
}

/**
 * Default callback method for syncs
 */
void AbstractApplication::svSyncIndication(desenet::NetworkTime)
{
}

/**
 * Default callback method for SV publishes
 */
SharedByteBuffer::sizeType AbstractApplication::svPublishIndication(SvGroup, SharedByteBuffer &)
{
	return 0;
}
