#ifndef XFEVENTSTATUS_H
#define XFEVENTSTATUS_H


/**
 * @ingroup xf
 * The EventStatus is used after consumption of an event. It gives an
 * information on how the event was consumed and how the framework should
 * react.
 */
class EventStatus
{
public:
    /**
     * Status values about the event processed.
     *
     * \see XFReactive::process
     */
    typedef enum
    {
        Unknown			= 0,
        Consumed		= 0x0001,
        NotConsumed		= 0x0002,
        Terminate		= 0x0004
    } eEventStatus;

    /**
     * Constructor
     *
     * \param eventStatus Initial value of the event status.
     */
    EventStatus(eEventStatus eventStatus = Unknown) : _status(eventStatus) {}

    /**
     * Check if both variables contain the same event status
     */
    bool operator == (const EventStatus::eEventStatus & eventStatus) const
    {
        return (this->_status == eventStatus);
    }

public:
    eEventStatus _status;		///< Attribute containing the status of the event
};

typedef EventStatus::eEventStatus eEventStatus;

#endif // XFEVENTSTATUS_H
