#include "xf.h"
#include "critical/critical.h"
#include "xftimeoutmanager.h"


bool XF::_bInitialized = false;
bool XF::_isRunning = false;
XFThread XF::_mainThread;


/**
 * Initializes the eXecution Framework (XF). Call
 * this method prior to initialize the other objects of the system.
 */
void XF::init(int timeInterval)
{
    // Create and initialize XFTimeoutManager
    XFTimeoutManager::getInstance()->setTickInterval(timeInterval);
    // Start it
    XFTimeoutManager::getInstance()->start();
}

/**
 * Starts the execution of the framework. This
 * results in processing the events in the main loop.
 */


#if defined(TC_STM32) || defined(TC_STM32CUBEIDE) /*******************************************************************/

void XF::start()
{
    _isRunning = true;     // From here the XF is running

    _mainThread.start();
}

#endif /**************************************************************************************************************/


XFThread * XF::getMainThread()
{
    return &_mainThread;
}

bool XF::isRunning()
{
    return _isRunning;
}

void XF_init(int timeInterval)
{
    XF::init(timeInterval);
}

void XF_start()
{
    XF::start();
}

bool XF_isRunning()
{
    return XF::isRunning();
}

void XF_tick()
{
    enterISR();                                     // Tell critical section we are in an ISR
    if (XF::isRunning())                            // Call tick only if XF is running
    {
        XFTimeoutManager::getInstance()->tick();    // Call framework hook tick function
    }
    exitISR();
}

int32_t XF_tickIntervalInMilliseconds()
{
    return XFTimeoutManager::getInstance()->tickInterval();
}
