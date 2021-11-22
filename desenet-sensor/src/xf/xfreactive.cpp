#include <assert.h>
#include "xf.h"
#include "xfreactive.h"
#include "xftimeout.h"
#include "initialevent.h"


/**
 * Constructor
 *
 * \param pThread Reference to thread who will process the behavioral part of this reactive class.
 */
XFReactive::XFReactive(XFThread * pThread)
 : _pThread((pThread) ? pThread : XF::getMainThread()),
   _pCurrentEvent(nullptr)
{
}

XFReactive::~XFReactive()
{

}

void XFReactive::startBehavior()
{
    GEN(InitialEvent());
}


void XFReactive::pushEvent(IXFEvent * pEvent)
{
    assert(pEvent);

    // Set owner
    pEvent->setBehavior(this);
    // Push to threads event queue
    getThread()->pushEvent(pEvent);
}

/**
 * \brief Processes the given event.
 *
 * This may change the state of the class
 * according to its implemented behavior.
 *
 * Must only be called by the associated thread (see #_pThread).
 *
 * \param pEvent The event to process.
 */
EventStatus XFReactive::process(IXFEvent * pEvent)
{
    EventStatus eventStatus;

    assert(pEvent);

    setCurrentEvent(pEvent);

    eventStatus = processEvent();

    setCurrentEvent(NULL);

    return eventStatus;
}

/**
 * Executes the current event in its implemented behavior.
 * This method needs to be overridden to implement the behavior needed.
 */
EventStatus XFReactive::processEvent()
{
    // You should really implement this function in
    // the derived class!

    return EventStatus::Unknown;
}


XFThread * XFReactive::getThread()
{
    return _pThread;
}

void XFReactive::setCurrentEvent(IXFEvent * pEvent)
{
    _pCurrentEvent = pEvent;
}

IXFEvent * XFReactive::getCurrentEvent() const
{
    assert(_pCurrentEvent);
    return _pCurrentEvent;
}

/** \brief Returns a reference to the actually processed timeout.
 *
 * Will work only if the current event is of type IXFEvent::Timeout.
 */
XFTimeout * XFReactive::getCurrentTimeout()
{
    assert(getCurrentEvent()->getEventType() == IXFEvent::Timeout);

    return (XFTimeout *)getCurrentEvent();
}
