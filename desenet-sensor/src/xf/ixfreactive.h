#ifndef IXFREACTIVE_H
#define IXFREACTIVE_H

#include <stddef.h>
#include "xf.h"
#include "ixfevent.h"
#include "xfeventstatus.h"

class IXFThread;


/**
 * @ingroup xf
 * Interface that needs to be implemented by every class having a
 * behavior. Interface methods are needed by the other parts of the XF.
 */
class IXFReactive
{

public:
    /**
     * Constructor
     *
     * \param pThread Reference to thread executing the behavior of this class.
     */
    IXFReactive(IXFThread * pThread = NULL) { (void)pThread; }
    virtual ~IXFReactive() {}

    virtual void startBehavior() = 0;						///< Starts the behavior
    virtual void pushEvent(IXFEvent * pEvent) = 0;			///< Injects an event into the class
    virtual EventStatus process(IXFEvent * pEvent) = 0;		///< Called by the thread to process an event
};

#endif // IXFREACTIVE_H
