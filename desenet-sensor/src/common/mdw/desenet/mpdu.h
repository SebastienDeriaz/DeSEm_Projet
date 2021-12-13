#pragma once
#include <string>

#include "frame.h"
#include "types.h"
#include <string.h>

namespace desenet {

/**
 * @brief Object to represent a MPDU Frame.
 *
 * The class can be used to construct a DESENET MPDU frame to send or to read
 * the fields of a received MPDU frame.
 */
class MPDU : public Frame {
    static const uint8_t SIZE = Frame::HEADER_SIZE + 32 + Frame::FOOTER_SIZE;
    static const int SENSOR_ID_INDEX = Frame::HEADER_SIZE + 0;
    static const int EPDU_COUNT_INDEX = Frame::HEADER_SIZE + 1;
    static const int EPDU_START = Frame::HEADER_SIZE + 2;

    union TypeID {
        uint8_t byte;
        struct {
            uint8_t ID : 7;
            uint8_t type : 1;
        } fields;
    };

   public:
    /**
     * @brief Constructs a new MPDU frame.
     *
     * Allocates a new frame
     */
    MPDU();

    /**
     * @brief ePDU type (Event of sampled value)
     *
     */
    enum ePDUType : uint8_t { SV = 0, EV = 1 };

    /**
     * @brief ePDU struct, contains the data and parameters of ePDU
     *
     */
    union ePDUHeader {
        uint8_t byte;
        struct {
            uint8_t length : 3;
            uint8_t SVGroup_eventID : 4;
            uint8_t type : 1;
        } fields;
    };

    /**
     * @brief Resets the frame and (initializes it according to the received one
     * (destination address))
     *
     */
    void reset(desenet::SlotNumber slotNumber /*const Frame & frame*/);

    /**
     * @brief Access to write on the mpdu buffer
     *
     */
    SharedByteBuffer pduBuffer;

    /**
     * @brief Number of PDUs
     *
     */
    int pduCount;

    /**
     * @brief Current position of buffer for writing PDUs (after count of PDUs)
     *
     */
    size_t pduBufferPosition;

    /**
     * @brief Returns the remaining bytes left in the MPDU
     * 
     */
    int remainingBytes();

    /**
     * @brief Copies the bytes from a buffer (evPDU) to memory
     * 
     * @param buf buffer (SharedByteBuffer)
     * @return number of bytes written
     */
    size_t writePDU(const SharedByteBuffer& buf);

    /**
     * @brief Commits an ePDU and sets the corresponding header
     *
     */
    void commitPDU(ePDUHeader& epdu);

    /**
     * @brief Finalizes the frame (sets the pdf count and the total length)
     *
     */
    void finalize();

   private:
    /**
     * @brief Sets the sensor ID
     * @param sensorID : uint8 (7 bits)
     */
    void setSensorID(const uint8_t ID);
};
}  // namespace desenet
