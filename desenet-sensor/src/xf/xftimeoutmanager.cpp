#include <assert.h>

#ifdef TC_STM32CUBEIDE

#include "critical/critical.h"

#elif TC_STM32 /*****************************************************************************************************/

#include "misc.h"
#include "critical.h"

#elif defined( TC_MESHSIM ) || defined(TC_QTCONSOLE) /****************************************************************/

// We do not have threads on desktop XF, so we o not need the enterCritical() and exitCritical() functions.
#define enterCritical()
#define exitCritical()

// Support to stop XF execution with mesh simulator.
volatile bool __XF_PAUSED = false;

#endif /**************************************************************************************************************/


#include "xftimeoutmanager.h"
#include "ixfreactive.h"
#include "xftimeout.h"


// Initialization of static class attributes
XFTimeoutManager * XFTimeoutManager::_pInstance = NULL;


XFTimeoutManager::XFTimeoutManager()
 : _tickInterval(0),
   _sysTicksInMs(0)
{
}

XFTimeoutManager::~XFTimeoutManager()
{
}

XFTimeoutManager * XFTimeoutManager::getInstance()
{
    if (!_pInstance)
    {
        _pInstance = new XFTimeoutManager;
    }

    return _pInstance;
}

/**
 * Sets the time interval in milliseconds in which the timeout manager
 * should handle the timeouts.
 */
void XFTimeoutManager::setTickInterval(int tickInterval)
{
    _tickInterval = tickInterval;
}

/**
 * Starts the timeout manager.
 */
void XFTimeoutManager::start()
{
    // Check tickInterval. Set default value if not set
    if (_tickInterval == 0)
    {
        _tickInterval = 10;
    }

    startHardwareTimer(_tickInterval);
}

/**
 * Adds a timeout to the timeout manager.
 *
 * \param timeoutId The timeout id known by the reactive parameter. Is needed by the reactive part to uniquely identify the timeout.
 * \param interval The time in milliseconds to wait until the timeout expires.
 * \param pReactive The reactive instance where to inject the timeout when it timeouts.
 */
void XFTimeoutManager::scheduleTimeout(int timeoutId, int interval, IXFReactive * pReactive)
{
    XFTimeout * pTimeout = new XFTimeout(timeoutId, interval, pReactive);

    if (pTimeout)
    {
        addTimeout(pTimeout);
    }
}

/**
 * Removes all timeouts corresponding the given parameters.
 */
void XFTimeoutManager::unscheduleTimeout(int timeoutId, IXFReactive * pReactive)
{
    removeTimeouts(timeoutId, pReactive);
}

/**
 * On each call to this method, XFTimeoutManager::_tickInterval will be removed from the
 * timeouts. When a timeout expires it is injected back to its reactive instance.
 */
void XFTimeoutManager::tick()
{
    _sysTicksInMs += tickInterval();

    if (!_timeouts.empty())
    {
        XFTimeout * pFirstTimeout = _timeouts.front();

        // Subtract time elapsed
        pFirstTimeout->_relTicks -= tickInterval();

        // Check timeout timed out
        if (pFirstTimeout->_relTicks <= 0)
        {
            // Check remaining ticks can be given further
            if (_timeouts.size() > 1)
            {
                TimeoutList::iterator i = _timeouts.begin();

                // Add ticks overrun to next timeout
                i++;
                (*i)->_relTicks -= pFirstTimeout->_relTicks;
            }

            // Inject the timeout back to the behavioral class
            returnTimeout(pFirstTimeout);

            // Remove timeout
            _timeouts.pop_front();

            // Check if timeouts with same timeout value are present
            for (TimeoutList::iterator it = _timeouts.begin(); it != _timeouts.end(); /*Do not increment here!*/)
            {
                if ((*it)->_relTicks == 0)
                {
                    returnTimeout(*it);
                    it = _timeouts.erase(it);
                }
                else
                {
                    break;
                }
            }
        }
    }
}

void XFTimeoutManager::addTimeout(XFTimeout * pNewTimeout)
{
    enterCritical();

    if (!_timeouts.empty())
    {
        // Insert timeout before timeout(s) triggering later
        TimeoutList::iterator i = _timeouts.begin();

        if ((*i)->_relTicks >= pNewTimeout->_relTicks)
        {
            // A new timeout at the beginning
            _timeouts.push_front(pNewTimeout);

            // Remove time from following timeout
            (*i)->_relTicks -= pNewTimeout->_relTicks;
        }
        else
        {
            unsigned int index = 0;

            // Remove time from new timeout
            pNewTimeout->_relTicks -= (*i)->_relTicks;
            i++; index++;

            while (i != _timeouts.end() &&
                   (*i)->_relTicks < pNewTimeout->_relTicks)
            {
                pNewTimeout->_relTicks -= (*i)->_relTicks;
                i++; index++;
            }
            // Insert new timeout before
            i = _timeouts.insert(i, pNewTimeout);

            if (_timeouts.size() > index + 1)
            {
                // Remove time from following timeout
                i++;
                assert(i != _timeouts.end());
                (*i)->_relTicks -= pNewTimeout->_relTicks;
            }
        }
    }
    else
    {
        _timeouts.push_front(pNewTimeout);
    }

    exitCritical();
}

/**
 * Removes all timeouts corresponding the given parameters.
 */
void XFTimeoutManager::removeTimeouts(int timeoutId, IXFReactive * pReactive)
{
    const XFTimeout timeout(timeoutId, 0, pReactive);
    XFTimeout * pTimeout;

    enterCritical();

    for (TimeoutList::iterator i = _timeouts.begin();
         i != _timeouts.end();)
    {
        pTimeout = *i;

        // Check if behavior and timeout id are equal
        if (*pTimeout == timeout)
        {
            TimeoutList::iterator next = i;

            // Check if remaining ticks can be given further
            if (++next != _timeouts.end())
            {
                // Add (remaining) ticks to next timeout in list
                (*next)->_relTicks += pTimeout->_relTicks;
            }

            i = _timeouts.erase(i);

            delete pTimeout;
        }
        else
        {
            i++;
        }
    }

    exitCritical();
}

/**
 * Returns the timeout back to the queue of the thread executing
 * the behavioral instance.
 */
void XFTimeoutManager::returnTimeout(XFTimeout * pTimeout)
{
    pTimeout->getBehavior()->pushEvent(pTimeout);
}


/**
 * Initializes and starts a hardware timer, which will call XFTimeoutManager::tick()
 * in a regular interval given by parameter tickTime
 *
 * \param tickTime Time in milliseconds the hardware timer should call XFTimeoutManager::tick()
 */
void XFTimeoutManager::startHardwareTimer(int tickTime)
{
    //
    // Configure SysTick to interrupt at requested interval
    //


#ifdef TC_STM32 /*****************************************************************************************************/

    // Update SystemCoreClock according to Clock Register Values
    SystemCoreClockUpdate();

    // Set tick interval to tickTime. Timer interrupt (SysTickHandler())
    // should be called every tickTime milliseconds.
    SysTick_Config(SystemCoreClock * tickTime / 1000);

    NVIC_SetPriority(SysTick_IRQn, 0x00);

#elif defined( TC_MESHSIM ) || defined(TC_QTCONSOLE) /****************************************************************/

    startTimer( tickTime );

#endif /**************************************************************************************************************/


}


#ifdef TC_STM32 /*****************************************************************************************************/

/**
 * SysTick Interrupt Service Routine
 */
extern "C" void SysTick_Handler()
{
    enterISR();
    XFTimeoutManager::getInstance()->tick();	// Call Framework hook tick function
    exitISR();
}

#elif defined( TC_MESHSIM ) || defined(TC_QTCONSOLE) /****************************************************************/

void XFTimeoutManager::timerEvent( QTimerEvent * event )
{
    (void)event;    // Parameter not used
    if ( !__XF_PAUSED ) tick();
}

#endif /**************************************************************************************************************/
