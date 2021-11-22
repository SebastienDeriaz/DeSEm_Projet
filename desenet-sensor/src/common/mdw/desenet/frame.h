#ifndef DESENET_FRAME_H
#define DESENET_FRAME_H

#include <memory>
#include "platform-config.h"
#include "types.h"

namespace board {
	class NrfTransceiver;
}
#if (DESENET_GATEWAY_NODE != 0) || defined(DESENET_TESTBENCH)
namespace serialcon {
	class Msg;
}
#endif

namespace desenet {

	class NetworkInterfaceDriver;

    /**
     * @brief The frame type of the data in the frame buffer.
     */
    enum class FrameType
    {
        /**
          * @brief The frame data does not seem to be valid DESENET data.
          */
        Invalid,

        /**
          * @brief The frame data is a DESENET beacon.
          */
        Beacon,

        /**
          * @brief The frame data is a DESENET Multi-PDU.
          */
        MPDU
    };

   /**
    * @brief Frame send or received by the lower layer.
    *
    * This class may be use to construct more specific frame types used by the higher layers.
    */
    class Frame
    {
        friend NetworkInterfaceDriver & operator <<(NetworkInterfaceDriver & driver, const Frame & frame);
        friend class NrfTransceiverDriver;
        friend class board::NrfTransceiver;
#if (DESENET_GATEWAY_NODE != 0) || defined(DESENET_TESTBENCH)
        friend class serialcon::Msg;
#endif

    public:
        /**
         * @brief Size of the frame header: Destination address (4 bytes) + payload length (1 byte)
         *
         * Used for simulation only. The header is stripped away before it is given
         * to the NrfTransceiver class.
         */
        static constexpr size_t HEADER_SIZE = 5;

        /**
         * @brief Size of the frame footer.
         *
         * The Nrf24 chip automatically adds the CRC to the frame send. Therefore, no memory space needs to be reserved.
         */
        static constexpr size_t FOOTER_SIZE = 0;

        /**
         * @brief The MTU (maximum transfer unit) for DESENET frames.
         *
         * The maximum size (bytes) of a frame.
         */
        static constexpr SharedByteBuffer::sizeType Mtu = HEADER_SIZE + 32 + FOOTER_SIZE;		// 32 byte limit comes from Nrf24 transceiver


        /**
         * @brief Constructs a Frame which uses the given buffer for the frame data storage.
         *
         * Note that the frame takes ownership of the buffer and you have to ensure that the given buffer will exclusively used for this frame during his
         * lifecycle.
         *
         * @param buffer    Pointer to the buffer.
         * @param length    The actual length of the buffer.
         * @return          A Frame using the given buffer as its data storage.
         */
        static Frame useBuffer(std::uint8_t *buffer, std::size_t length);

        /**
         * @brief Constructs a readonly Frame which uses the given buffer for the frame data storage.
         *
         * Note that the frame takes ownership of the buffer and you have to ensure that the given buffer will exclusively used for this frame during his
         * lifecycle.
         *
         * @param buffer    Pointer to the readonly buffer.
         * @param length    The actual length of the buffer.
         * @return          A readonly Frame using the given buffer as its data storage.
         */
        static const Frame useBuffer(const std::uint8_t *const buffer, std::size_t length);

        /**
         * @brief Constructs a readonly Frame by allocating a new buffer and copies the given buffer to its own buffer.
         *
         * Note that the buffer is allocated using new and delete[] automatically and the buffer will get freed once the last copy of frame gets destructed.
         *
         * @param buffer    Pointer to the buffer to copy the data from.
         * @param length    The actual length of the buffer to copy the data from.
         * @return          A Frame with the data copied from the source buffer.
         */
        static Frame copyFromBuffer(const std::uint8_t *const buffer, std::size_t length);

        /**
         * @brief Returns true if the frame seems to be a valid DESENET frame.
         *
         * @return  True if valid, false otherwise.
         */
        bool isValid() const;

        /**
         * @brief Returns the destination MAC address.
         *
         * @return Destination MAC address.
         */
        Address destination() const;

        /**
         * @brief Returns the content of the payload field.
         */
        uint8_t payloadSize() const;

        /**
         * @brief Returns the actual type of the DESENET frame.
         *
         * @return  DESENET frame type or Invalid if the frame data is no valid DESENET frame.
         */
        FrameType type() const;

        /**
         * @brief Returns the frame length.
         *
         * This length is not the actual length of the buffer in memory, it is the length of the frame represented by an object of this class. The length of
         * the buffer backing the frame is constant and can be retrieved by the method reservedLength().
         *
         * @return  Length of the frame.
         */
        std::size_t length() const {
            return frameLength_;
        }

        const uint8_t & operator[](size_t index) const { return buffer_[index]; }	///< Byte access (read-only) to buffer
        uint8_t & operator[]( size_t index ) { return buffer_[index]; }				///< Byte access (write) to buffer
       	const char * toString() const;												///< Returns content of the frame as string representation (HEX values).
       	static const char * toString(const uint8_t * const buffer, const std::size_t & length);	///< Static version of #toString.

    protected:
        /**
         * @brief Creates a new frame by allocating a new buffer on the heap.
         *
         * Note that frames can not be resized, so you have always to allocate a buffer with the maximal needed size.
         *
         * @param initialLength The initial length of the buffer, defaults to the MTU.
         */
        Frame(std::size_t initialLength = Mtu);

        /**
         * @brief Sets the destination MAC address (located in frame header).
         *
         * @param destination   Destination MAC address.
         */
        void setDestination(const Address & destination);

        /**
         * @brief Sets the frame length. Includes header, payload and footer of the frame.
         *
         * Because the buffer backing the frame object can not be changed in size, the length of
         * the frame logically can not exceed. The maximal possible frame size
         * can be received using the method reservedLength().
         *
         * @param length The new length of the frame.
         */
        void setLength(std::size_t length);

        /**
         * @brief Sets the frame type.
         *
         * @param type  DESENET frame type.
         */
        void setType(FrameType type);

        /**
         * @brief Returns a pointer to the buffer.
         *
         * Respect the maximal buffer length by calling the reservedLength() method and never write over out the buffer bounds, please.
         *
         * @return  Pointer to the buffer.
         */
        std::uint8_t * buffer()
        {
            return buffer_.data();
        }

        /**
         * @brief Returns a readonly pointer to the buffer.
         *
         * @return  Readonly pointer to the buffer.
         */
        const std::uint8_t *buffer() const
        {
            return buffer_.data();
        }

        /**
         * @brief Returns the length of the buffer that serves as data storage for the frame object.
         *
         * @return  Length of the data buffer backing the frame.
         */
        std::size_t reservedLength() const
        {
            return buffer_.length();
        }

    private:
        /**
         * @brief Sets the size in the payload field (located in frame header).
         *
         * Using the method setLength() the payload field gets set too.
         */
        void setPayloadSize(uint8_t payloadSize);

        Frame(const SharedByteBuffer &buffer);

        SharedByteBuffer buffer_;
        std::size_t frameLength_;
    };

    /**
     * @brief Streaming output operator in order to easily send a frame using an network interface driver.
     *
     * @param driver    The driver to use that should send the frame.
     * @param frame     The frame.
     * @return          A reference to the network interface.
     */
    NetworkInterfaceDriver & operator <<(NetworkInterfaceDriver & driver, const Frame & frame);

}	// namespace desenet
#endif // DESENET_FRAME_H
