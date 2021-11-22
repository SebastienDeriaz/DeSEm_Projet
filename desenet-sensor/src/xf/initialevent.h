#ifndef INITIALEVENT_H
#define INITIALEVENT_H


#include "ixfevent.h"


/**
 * @ingroup xf
 * Represents the first transition executed in a state machine.
 */
class InitialEvent : public IXFEvent
{
public:
    InitialEvent();
    virtual ~InitialEvent();
};

#endif // INITIALEVENT_H
