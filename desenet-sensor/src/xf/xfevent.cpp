#include "xfevent.h"


/**
 * Constructor for the class
 *
 * \param id Identifier uniquely identifying the event in the context of the behavior
 * \param pBehavior The Behavior for which the event was constructed (will consume the event)
 **/
XFEvent::XFEvent(int id, IXFReactive * pBehavior)
    : IXFEvent(IXFEvent::Event, id, pBehavior)
{

}

XFEvent::~XFEvent()
{

}
