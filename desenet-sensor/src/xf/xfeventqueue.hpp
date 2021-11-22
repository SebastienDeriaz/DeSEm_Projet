#pragma once


#include <queue>
#include "ixfevent.h"

/**
 * @ingroup xf
 * Queue for pending events.
 */
class XFEventQueue : public std::queue<IXFEvent *>
{
public:
	XFEventQueue() {}
	virtual ~XFEventQueue() {}

	typedef std::queue<IXFEvent *> BaseClass;

	bool empty() __attribute__ ((noinline)) { return BaseClass::empty(); }	// Prevent gcc from optimizing this operation
};

