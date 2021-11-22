#pragma once

#include "desenet/frame.h"

using desenet::Frame;

/**
 * @ingroup interfaces
 * @brief This interface has to be implemented by all objects who want to register them self as observer at an object
 * implementing the IPhyTransceiver interface.
 *
 * For every received frame, the method onReceive() will be called and for each send frame, the onSendStatus() method will
 * be called with the actual state of the send operation. Note that depending on the used platform, the context in which
 * the two methods are executed can vary and it can be the same thread, another thread or in the worst case even an
 * interrupt service routine. So do not use the new() method or the malloc() function inside the methods and do not call
 * other methods that could use them. Execute only as much code as needed to process these events (set event flags,
 * push event to queue,...).
 */
class IPhyObserver
{
protected:
	// It is just an interface, so constructor and destructor should be not public.
	explicit IPhyObserver() {}
	virtual ~IPhyObserver() {}

public:
	/**
	 * @brief This method is called by the object implementing the IPhyTransceiver interface whenever a new frame was
	 * received from the PHY hardware.
	 *
	 * Attention the context the method is called in is not defined and can be either another thread and in the worst
	 * case an interrupt service routine.
	 *
	 * @param frame The frame received.
	 */
	virtual void onReceive(const Frame & frame) = 0;

	/**
	 * @brief Defines the different status possibilities of a sent frame which is reported back from the object
	 * implementing the IPhyObserver interface.
	 */
	enum SendStatus
	{
		InvalidStatus	= 0 ,	///< @brief Invalid status report.
		Delivered 		= 1 ,	///< @brief The frame was delivered (ack from peer).
		Sent			= 2 ,	///< @brief The frame was send, but no information about delivering (ack feature disabled).
		NotDelivered	= 3	,	///< @brief The frame could not be delivered (NACK or transceiver error).
	};

	/**
	 * @brief This method is called by the object implementing the IPhyTransceiver interface whenever a frame was delivered
	 * or when the transceiver was not able to deliver the frame in time or not at all.
	 *
	 * @param status The status of the last transmission (send).
	 */
	virtual void onSendStatus(SendStatus status) = 0;
};
