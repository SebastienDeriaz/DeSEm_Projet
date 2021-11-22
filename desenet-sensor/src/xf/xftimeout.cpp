#include "xftimeout.h"


/**
 * Constructor of the class.
 * \param id The id given by the behavioral class to uniquely identifing the timeout
 * \param interval The interval in milliseconds the timeout expires
 * \param pBehavior Pointer to behavioral class to which the timeout belongs.
 */
XFTimeout::XFTimeout(int id, int interval, IXFReactive * pBehavior)
 : IXFEvent(IXFEvent::Timeout, id, pBehavior),
   _interval(interval),
   _relTicks(interval)
{
}

XFTimeout::~XFTimeout()
{

}

/**
 * Operator checks if _pBehavior and _id are equal and returns
 * true if so. All other attributes get no attention.
 */
bool XFTimeout::operator ==(const XFTimeout & timeout) const
{
    // Check behavior and timeout id attributes, if there are equal
    return (_pBehavior == timeout._pBehavior && getId() == timeout.getId()) ? true : false;
}
