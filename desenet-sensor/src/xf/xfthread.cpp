#include "xfthread.h"
#include "ixfreactive.h"
#include "xftimeoutmanager.h"
#ifdef TC_STM32
  #include "mcu/mcu.h"
#endif


XFThread::XFThread()
 : _bExecuting(true)


#if defined(TC_MESHSIM) || defined(TC_QTCONSOLE) /********************************************************************/

    , _tid( -1 )

#endif /**************************************************************************************************************/


{
}

XFThread::~XFThread()
{
}

void XFThread::start()
{
    execute();
}

XFTimeoutManager * XFThread::getTimeoutManager() const
{
    return XFTimeoutManager::getInstance();
}

/**
 * Dispatchs the event to the corresponding behavioral part. For example
 * the state machine which sould process the event.
 * \param pEvent The event to dispatch
 */
void XFThread::dispatchEvent(IXFEvent * pEvent) const
{
    EventStatus eventStatus;

    eventStatus = pEvent->getBehavior()->process(pEvent);

    // Check if behavior should be deleted
    if (eventStatus == EventStatus::Terminate)
    {
        delete pEvent->getBehavior();
    }
}

void XFThread::pushEvent(IXFEvent * pEvent)
{
    _events.push(pEvent);


#if defined(TC_MESHSIM) || defined(TC_QTCONSOLE) /********************************************************************/

    if ( _tid == -1 )
    {
        _tid = startTimer( 0 );
    }

#endif /**************************************************************************************************************/


}

/**
 * Adds a new timeout to be handled. The XFThread will forward the timeout
 * information to the timeout manager which is responsible to handle all timeouts.
 */
void XFThread::scheduleTimeout(int timeoutId, int interval, IXFReactive * pReactive)
{
    // Forward timeout to the timeout manager
    getTimeoutManager()->scheduleTimeout(timeoutId, interval, pReactive);
}

/**
 * Removes all timeouts corresponding the given parameters.
 */
void XFThread::unscheduleTimeout(int timeoutId, IXFReactive * pReactive)
{
    // Forward timeout to the timeout manager
    getTimeoutManager()->unscheduleTimeout(timeoutId, pReactive);
}

/**
 * Main loop of the thread. Implements event loop processing.
 */
void XFThread::execute()
{
    while(_bExecuting)
    {
        IXFEvent * pEvent;

        while (_events.empty())
        {
            // Wait until something to do
#ifdef TC_STM32
            Mcu::sleep();	// Let the MCU sleep until an event occurs.
#endif
#if defined(TC_MESHSIM) || defined(TC_QTCONSOLE) /********************************************************************/

            killTimer( _tid );
            _tid = -1;
            return;

#endif /**************************************************************************************************************/
            __asm__("NOP");    // Prevent compiler from optimising this loop away
        }

        // Deque next event from queue
        pEvent = _events.front(); _events.pop();

        if (pEvent)
        {
            // Forward the event to the behavioral class
            dispatchEvent(pEvent);
        }

        if (pEvent->getEventType() == IXFEvent::Terminate)
        {
            // Exit the event loop
            _bExecuting = false;
        }

        if (pEvent->shouldDelete())
        {
            // Delete consumed event
            delete pEvent;
        }
    }
}


#if defined(TC_MESHSIM) || defined(TC_QTCONSOLE) /********************************************************************/

void XFThread::timerEvent( QTimerEvent * event )
{
    (void)event;    // Parameter not used
    execute();
}

#endif /**************************************************************************************************************/
