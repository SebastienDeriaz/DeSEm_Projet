#ifndef XFTHREAD_H
#define XFTHREAD_H


#if defined(TC_MESHSIM) || defined(TC_QTCONSOLE) /********************************************************************/

#include <QtCore/QObject>

#endif /**************************************************************************************************************/


#include "ixfevent.h"
#include "xfeventqueue.hpp"


class XFTimeoutManager;


/**
 * @ingroup xf
 * XFThread representing the instance executing the behavior. In a OS less XF,
 * only one instance of XFThread gets created (see XF::_mainThread). In a XF
 * interfacing an OS, multiple instances can be made, each interfacing the real
 * OS thread.
 */
class XFThread


#if defined(TC_MESHSIM) || defined(TC_QTCONSOLE) /********************************************************************/

    : public QObject

#endif /**************************************************************************************************************/


{
public:
    XFThread();
    virtual ~XFThread();

    void start();										///< Starts the thread

    void pushEvent(IXFEvent * pEvent);					///< Adds event to the #_events queue
    void scheduleTimeout(int timeoutId, int interval, IXFReactive * pReactive);
    void unscheduleTimeout(int timeoutId, IXFReactive * pReactive);

protected:
    void execute();
    XFTimeoutManager * getTimeoutManager() const;		///< Returns pointer to timeout manager
    void dispatchEvent(IXFEvent * pEvent) const;

protected:
    bool _bExecuting;				///< True as long as the thread is executing the main loop.

    XFEventQueue _events;			///< Queue holding events waiting to get dispatched.


#if defined(TC_MESHSIM) || defined(TC_QTCONSOLE) /********************************************************************/

private:
    int _tid;
    void timerEvent( QTimerEvent * event );

#endif /**************************************************************************************************************/


};

#endif // XFTHREAD_H
