#include "xfstaticevent.h"


XFStaticEvent::XFStaticEvent(int id, IXFReactive * pBehavior)
: IXFEvent(IXFEvent::Event, id, pBehavior)
{
	// Clear the shouldDelete flag in the _eventStatus attribute.
	setShouldDelete(false);
}

XFStaticEvent::~XFStaticEvent()
{

}
