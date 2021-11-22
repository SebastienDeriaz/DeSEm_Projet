#ifndef PHY_FRAME_H
#define PHY_FRAME_H

#include <stdint.h>
#include <cstddef>
#include <vector>
#include "iphytransceiver.h"

/**
 * @defgroup phy Phy
 * @brief The utililty classes needed for proper PHY operation.
 * @{
 *
 * @section phy_cd Class Diagram
 * The Phy package contains just the Frame class that holds PHY level frames and is based on the object pool template.
 * @image html Phy.png
 */

namespace phy {

/**
 * @brief Represents a frame used by the IPhyTransceiver interface.
 *
 * The frame does not has attributes that only concern the PHY Transmission layer, so basically a frame consists of an
 * address (which is source and destination in the same time.
 *
 * Note that the data of copied frames is shared, so you need to make a deepCopy() in order to have an independent frame
 * that does not change the content of the first one when modified.
 */
class Frame
{
public:
	typedef phy::Address<4> FrameAddress;	///< @brief The destination address on send messages, the source on received ones.

	/**
	 * @brief Creates a new empty frame with an invalid address.
	 */
	explicit Frame();

	/**
	 * @brief Creates a frame with the given address and the given size. Note that the data is NOT initialized.
	 *
	 * @param address 		Frame address.
	 * @param payloadSize 	The size of the payload data.
	 */
	explicit Frame( const FrameAddress & address , size_t payloadSize );

	/**
	 * @brief Creates a frame with the given address and copies the given data into the frame.
	 *
	 * @param address 		Frame address.
	 * @param payload 		Pointer to the memory location to copy the playload from.
	 * @param payloadSize 	The size of the payload data.
	 */
	explicit Frame( const FrameAddress & address , const void * const payload = NULL , size_t payloadSize = 0 );

	/**
	 * @brief Creates a frame with the given address and copies the given data into the frame.
	 *
	 * @param address 	Frame address.
	 * @param payload 	Reference to the vector where the data has to be copied from.
	 */
	explicit Frame( const FrameAddress & address , const std::vector<uint8_t> & payload );

	/*!
	 * @brief Destructor, rmoves the frame from the object pool.
	 */
	virtual ~Frame();

	/**
	 * @brief Copy constructor, makes a fast pointer copy and manages the reference counts to the internal data structures.
	 *
	 * @param other Reference to the frame to copy to this one.
	 */
	Frame( const Frame & other );

	/**
	 * @brief Assignment operator, makes a fast pointer copy and manages the reference counts to the internal data structures.
	 *
	 * @param other 	Reference to the frame to copy to this one.
	 * @return 			A reference to him self.
	 */
	const Frame & operator=( const Frame & other );

	/**
	 * @brief Creates a deep copy of the original frame that does not share the data buffer with the original one.
	 *
	 * @return A newly allocated frame with the content from this one.
	 */
	Frame deepCopy();

	/**
	 * @brief Returns the address (destination and/or source) of the frame.
	 *
	 * @return Address of the frame.
	 */
	FrameAddress address() const;

	/**
	 * @brief Sets the frame's address to the given one.
	 *
	 * @param address The address to set.
	 */
	void setAddress( const FrameAddress & address );

	/**
	 * @brief Returns the actual size of the payload.
	 *
	 * @return Payload size in bytes.
	 */
	size_t payloadSize() const;

	/**
	 * @brief Changes the payload size, but note that the payload will not be initialized at all when changing the size.
	 *
	 * @param size The payload size to use.
	 */
	void setPayloadSize( size_t size );

	/**
	 * @brief Returns maximum allowed payload size in byte
	 */
	static size_t maximalFramePayloadSize();

	/**
	 * @brief Returns a reference (R/W) to the byte of the payload at the given index. Note that this automatically
	 * 		  changes the size of the payload if the payload was smaller before.
	 *
	 * @param index 	Index of the byte inside the frame payload.
	 * @return 			Reference to the byte.
	 */
	uint8_t & operator[]( size_t index );

	/**
	 * @brief Returns a reference (R) to the byte of the payload at the given index. Note that this does NOT automatically
	 * 		  changes the size of the payload if the payload was smaller before, since the access is only meant to be read.
	 *
	 * @param index 	Index of the byte inside the frame payload.
	 * @return 			Read only reference to the byte.
	 */
	const uint8_t & operator[]( size_t index ) const;

	/**
	 * @brief Returns a native, modifiable pointer to the memory location that actually stores the frame's data.
	 *
	 * @return Pointer to the memory location that holds the playload data.
	 */
	uint8_t * payloadBytes();

	/**
	 * @brief Returns a native, read only pointer to the memory location that actually stores the frame's data.
	 *
	 * @return Pointer to the memory location that holds the playload data.
	 */
	const uint8_t * payloadBytes() const;

	/**
	 * @brief Used to trace content of frame.
	 *
	 * @return String representing frame content (hex values).
	 */
	const char * toString() const;

	/**
	 * @brief Initializes the frame pool. Note that this class method must be called before the first frame is created
	 * 		  or the creation of the frame will fail.
	 *
	 * @param numberOfFrames 	The number of frames that should be allocated.
	 * @param memory 			Pointer to the pointer that points to the location where the pool and management data
	 *               			structures should be created. If NULL, malloc() will be used to get dynamically memory
	 *               			to store the pool into.
	 */
	static bool initializeFramePool( size_t numberOfFrames , uint8_t * * memory = NULL );

private:
	// Private implementation part.
	class Private;
	Private * _pImpl;
};

} // namespace phy
#endif // PHY_FRAME_H
/** @} */
