#pragma once
#include <string>
#include "frame.h"
#include "types.h"

namespace desenet {

    /**
     * @brief Object to represent a MPDU Frame.
     *
     * The class can be used to construct a DESENET MPDU frame to send or to read the fields of a received MPDU frame.
     */
    class MPDU: public Frame
    {
        static const int SENSOR_ID_INDEX = 0;
    public:
        /**
         * @brief Constructs a new MPDU frame.
         *
         * Allocates a new frame and initializes its fields to the given parameter values and default values for all other fields.
         *
         * @param cycleInterval The cycle interval in microseconds.
         */
        MPDU(uint32_t cycleInterval = 0);

        /**
         * @brief Constructs a MPDU from the frame data.
         *
         * Note that the constructor does not fail if the actual data in the frame is not a MPDU frame. Use the type() method and ensure that it returns
         * MPDU in order to check if the MPDU data is valid.
         *
         * @param frame The frame to use to get the MPDU data.
         */
        MPDU(const Frame & frame);

        /**
         * @brief Sets the destination address
         */
        void setDestinationAddress(Address destinationAddress);


        /**
        * @brief Sets the sensor ID 
        * @param sensorID : uint8 (7 bits)
        */
       void setSensorID(const uint8_t ID) {
           // Reset the register (except MSB)
           
       }
    };
} // namespace desenet
