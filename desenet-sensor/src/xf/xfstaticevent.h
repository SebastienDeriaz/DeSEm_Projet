#pragma once


#include "xf.h"
#include "ixfevent.h"


/**
 * @ingroup xf
 * Base class for static events. Derive from this class if you
 * want the event to live further after processing.
 */
class XFStaticEvent : public IXFEvent
{
public:
	XFStaticEvent(int id, IXFReactive * pBehavior = NULL);
	virtual ~XFStaticEvent();
};
