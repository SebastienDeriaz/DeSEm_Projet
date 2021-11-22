#pragma once
#include <string>
#include "frame.h"
#include "types.h"

namespace desenet {

    /**
     * @brief Object to represent a Beacon Frame.
     *
     * The class can be used to construct a DESENET Beacon frame to send or to read the fields of a received Beacon frame.
     */
    class Beacon: public Frame
    {
    public:
    	static const uint8_t SIZE = Frame::HEADER_SIZE + 12 + Frame::FOOTER_SIZE;			///< Beacon size header + payload: (from 'Frametype' to 'SV Group Mask')
        /**
         * @brief Constructs a new beacon frame.
         *
         * Allocates a new frame and initializes its fields to the given parameter values and default values for all other fields.
         *
         * @param cycleInterval The cycle interval in microseconds.
         */
        Beacon(uint32_t cycleInterval = 0);

        /**
         * @brief Constructs a Beacon from the frame data.
         *
         * Note that the constructor does not fail if the actual data in the frame is not a Beacon frame. Use the type() method and ensure that it returns
         * Beacon in order to check if the beacon data is valid.
         *
         * @param frame The frame to use to get the beacon data.
         */
        Beacon(const Frame & frame);

        /**
         * @brief Sets the destination address
         */
        void setDestinationAddress(Address destinationAddress);

        /**
         * @brief Returns the network time field as milliseconds since UNIX epoch.
         *
         * @return  Network time.
         */
        uint64_t networkTime() const;

        /**
         * @brief Sets the network time field using time as milliseconds since UNIX epoch.
         *
         * @param time  Network time.
         */
        void setNetworkTime(uint32_t time);

        /**
         * @brief Returns the DESENET cycle interval as microseconds.
         *
         * @return  Cycle interval in microseconds.
         */
        uint32_t cycleInterval() const;

        /**
         * @brief Sets the DESENET cycle interval to interval in microseconds.
         *
         * @param interval  Cycle interval in microseconds.
         */
        void setCycleInterval(uint32_t interval);

        desenet::SlotDuration slotDuration();
        void setSlotDuration(desenet::SlotDuration slotDuration);

        /**
         * @brief Returns the sampled values group mask.
         *
         * @return Group mask.
         */
        SvGroupMask svGroupMask() const;

        /**
         * @brief Sets the sampled values group mask.
         *
         * @param mask  Group mask.
         */
        void setSvGroupMask(const SvGroupMask & mask);
    };
} // namespace desenet
