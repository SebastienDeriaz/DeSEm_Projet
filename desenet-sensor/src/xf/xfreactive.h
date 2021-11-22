#ifndef XFREACTIVE_H
#define XFREACTIVE_H


#include "ixfreactive.h"
#include "xfthread.h"
#include "xfeventstatus.h"
#include "xftimeout.h"
#include "xfnulltransition.h"


/**
 * @ingroup xf
 * Reactive class implementing a behavior. This class can be used to implement a
 * state machine behavior or an activity behavior.
 */
class XFReactive : public IXFReactive
{
public:
    #define GEN(event) pushEvent(new event);

    XFReactive(XFThread * pThread = NULL);
    virtual ~XFReactive();

    virtual void startBehavior();					///< Starts the behavior, resp. the state machine.

    virtual void pushEvent(IXFEvent * pEvent);

protected:
    virtual EventStatus process(IXFEvent * pEvent);	///< Called by XFThread.

protected:
    virtual EventStatus processEvent();				///> Overload to implement your state machine.
    virtual XFThread * getThread();					///< Returns reference to #_pThread.

    void setCurrentEvent(IXFEvent * pEvent);		///< Sets the current event to be processed in processEvent().
    IXFEvent * getCurrentEvent() const;				///< Returns the current event to be processed in processEvent().

    XFTimeout * getCurrentTimeout();

protected:
    XFThread * _pThread;							///< Reference to Thread executing this behavior.
    IXFEvent * _pCurrentEvent;						///< Reference to actually processed event.
};

#endif // XFREACTIVE_H
