#ifndef XFNULLTRANSITION_H
#define XFNULLTRANSITION_H


#include "xf.h"
#include "ixfevent.h"


/**
 * @ingroup xf
 * Represents a transition in a state machine having no trigger.
 */
class XFNullTransition : public IXFEvent
{
public:
    XFNullTransition( int id = 0 , IXFReactive * pBehavior = NULL);
    virtual ~XFNullTransition();
};

#endif // XFNULLTRANSITION_H
