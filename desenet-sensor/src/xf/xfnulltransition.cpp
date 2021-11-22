#include "xfnulltransition.h"


/**
 * Constuctor
 *
 * @param id		ID of the null transition.
 * \param pBehavior Behavior in which the null transition will be executed.
 */
XFNullTransition::XFNullTransition( int id , IXFReactive * pBehavior)
 : IXFEvent(IXFEvent::NullTransition, id , pBehavior)
{
}

XFNullTransition::~XFNullTransition()
{

}
