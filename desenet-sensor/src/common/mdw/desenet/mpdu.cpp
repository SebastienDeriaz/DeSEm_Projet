#include "mpdu.h"

#include <algorithm>
#include <cstring>

#include "platform-config.h"

using std::bitset;
using std::memcpy;
using std::memset;
using std::min;
using std::size_t;
using std::string;
using std::uint32_t;
using std::uint8_t;

desenet::MPDU::MPDU() : Frame(MPDU::SIZE) {
    memset(buffer() + Frame::HEADER_SIZE, 0,
           1);  // Set first field in payload to zero
    setType(FrameType::MPDU);
}

void desenet::MPDU::reset(desenet::SlotNumber slotNumber/*const Frame & frame*/) {
    // Copy the destination address from the received frame (can't actually... needs to be checked)
    //std::initializer_list<uint8_t> base_address = {0xE2, 0xE2, 0xE2, 0xE2};
    //setDestination(base_address);

    // Reset the buffer
    if(buffer()) {
        memset(buffer(), 0, length());
    }

    //Gets the slot number and puts it in the frame
    setType(FrameType::MPDU);
    setSensorID(slotNumber);

    setDestination(desenet::Address::fromHexString("E2E2E2E2"));

    // Lengths to check
    pduBuffer = SharedByteBuffer::proxy(buffer() + EPDU_START, MPDU::SIZE - EPDU_START - MPDU::FOOTER_SIZE);

    // Reset the number of PDU stored in the frame and the buffer position
    pduCount = 0;
    pduBufferPosition = 0;
}

void desenet::MPDU::setSensorID(const uint8_t ID) {
    // Reset the register (except MSB)
    uint8_t* buf = buffer();
    TypeID current;
    current.byte = buf[SENSOR_ID_INDEX];
    current.fields.ID = ID;
    buf[SENSOR_ID_INDEX] = current.byte;
}
void desenet::MPDU::commitPDU(ePDUHeader & epdu) {
    //Write the PDU length
    buffer()[EPDU_START + pduBufferPosition] = (uint8_t)epdu.byte;

    pduBufferPosition += epdu.fields.length + 1;
    pduCount++;

    //Advance the buffer for the next write
    pduBuffer = SharedByteBuffer::proxy(buffer() + EPDU_START + pduBufferPosition + 1, MPDU::SIZE - EPDU_START - MPDU::FOOTER_SIZE - pduBufferPosition);
}

void desenet::MPDU::finalize() {
    buffer()[EPDU_COUNT_INDEX] = (uint8_t)pduCount;
    //Header + Type/ID + ePDU count + PDUs
    setLength(Frame::HEADER_SIZE + 2 + pduBufferPosition);
}
