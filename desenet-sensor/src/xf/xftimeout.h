#ifndef IXFTIMEOUTS_H
#define IXFTIMEOUTS_H

#include "ixfevent.h"

/**
 * @ingroup xf
 * Interface to be implemented by timeouts. Defined methods are
 * needed by the framework classes (\see XFTimeoutMananager).
 */
class XFTimeout : public IXFEvent
{
    friend class XFTimeoutManager;

public:
    XFTimeout(int id, int interval, IXFReactive * pBehavior);
    virtual ~XFTimeout();

    bool operator ==(const XFTimeout & timeout) const;

protected:
    int _interval;		///< Timeout interval
    int _relTicks;		///< Used by the TimeoutManager to calculate remaining time. Can get negative!
};

#endif // IXFTIMEOUTS_H
