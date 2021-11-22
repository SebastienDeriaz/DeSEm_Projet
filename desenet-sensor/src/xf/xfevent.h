#ifndef XFEVENT_H
#define XFEVENT_H


#include "xf.h"
#include "ixfevent.h"


/**
 * @ingroup xf
 * Base class for standard events. To signal an event, derive
 * a new class from this class.
 */
class XFEvent : public IXFEvent
{
public:
    XFEvent(int id, IXFReactive * pBehavior = NULL);
    virtual ~XFEvent();
};

#endif // XFEVENT_H
