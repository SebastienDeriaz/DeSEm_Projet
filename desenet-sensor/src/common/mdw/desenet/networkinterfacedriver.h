#pragma once
#include <functional>
#include <string>
#include <array>
#include <vector>

namespace desenet
{
    /**
     * @brief Multiplatform C++ Network driver interface.
     */
    class NetworkInterfaceDriver
    {
    public:
        /**
         * @brief Descriptor for an existing network interface driver.s
         */
        class Descriptor
        {
        public:
            /**
             * @brief Constructor.
             */
            Descriptor();

            /**
             * @brief Destructor.
             */
            virtual ~Descriptor() {}

            /**
             * @brief Returns a human readable description of the represented network interface.
             *
             * @return  Human readable identifier.
             */
            virtual std::string friendlyName() const = 0;

            /**
             * @brief Returns an unique network interface identifier.
             *
             * @return  Unique network interface identifier.
             */
            virtual std::string identifier() const = 0;

            /**
             * @brief Creates an instance of the represented network interface driver.
             *
             * @return  Pointer to an instance of the network interface driver.
             */
            virtual NetworkInterfaceDriver *instanciate() const = 0;
        };

        friend class Descriptor;

        /**
         * @brief Callback function definition to receive network frames from a network interface driver instance.
         *
         * It is important to be aware that the buffer given in this method belongs to the caller. This means that you should **never** keep a pointer to
         * the buffer outside the actual context of the called object. If you need to save the frame or a part of the frame for later use, make a copy.
         *
         * Take care to not add infinite loops in this method neither do eternal computations.
         *
         * @param driver        Reference to the driver that is actually calling the function object.
         * @param receptionTime Time when the frame was received (milliseconds since start of the day).
         * @param buffer        Pointer to the frame data buffer.
         * @param length        The actual size of the buffer in bytes.
         */
        typedef std::function<void (NetworkInterfaceDriver & driver, const uint32_t receptionTime, const uint8_t * const buffer, size_t length)> ReceptionCallbackHandler;

        /**
         * @ List of network interface descriptors.
         */
        using DescriptorList = std::vector<Descriptor *>;

        /**
         * @brief Destructor.
         */
        virtual ~NetworkInterfaceDriver();

        /**
         * @brief Returns an unique network interface identifier.
         *
         * @return  Unique network interface identifier.
         */
        virtual std::string identifier() const = 0;

        /**
         * @brief Tries to initialize the network interface card. Returns true if the initialisation was successful, false otherwise.
         *
         * Normally initialisation includes the start of a detached thread which will be used in order to receive the network frames and call the
         * callback function defined using the receive method.
         *
         * @return  True on success, false otherwise.
         */
        virtual bool initialize() = 0;

        /**
         * @brief Returns true if the network interface is ready to use, false otherwise.
         *
         * @return  True if ready, false otherwise.
         */
        virtual bool isInitialized() const = 0;

        /**
         * @brief Returns the network interface's local MAC address.
         *
         * @return  Local network MAC address.
         */
        virtual std::array<std::uint8_t,4> localAddress() const = 0;

        /**
         * @brief Sends a frame using the network interface with the given data pointed by **buffer** and with the size **length**.
         *
         * @param buffer    Points to the begin of the data to include in the frame in memory.
         * @param length    The number of bytes to send.
         * @return          True on success, false otherwise.
         */
        virtual bool transmit(const std::uint8_t *const buffer, const std::size_t length) = 0;

        /**
         * @brief Configures the reception of network frames. Note that the method returns immediately and does not receive a frame at all. You will use this
         *        method in order to configure which callback function or functor will be used in order to receive new frames from the network interface.
         *
         * Note that the frame handler function, method or functor will be called from a high priority thread run by the ethernet driver. So you can allocate
         * memory and use all OS facilities without the need to care about problems related to running code on the interrupt service routine.
         *
         * In order to stop the frame reception just call this method with **nullptr** as argument.
         *
         * @param handler Member function that should be called for each received network frame.
         */
        virtual void setReceptionHandler(ReceptionCallbackHandler handler = nullptr) = 0;

        /**
         * @brief Returns the list of interface descriptors of the available network interfaces (drivers).
         *
         * @return  List of network interface driver descriptors.
         */
        static DescriptorList interfaceDescriptors();

    protected:
        NetworkInterfaceDriver() {}

    private:
        static DescriptorList & _descriptors();
    };
} // namespace desenet

/*
 * @brief Provides a general method to instanciate a NIC.
 *
 * Needs to be provided by every NIC driver implementation.
 */
desenet::NetworkInterfaceDriver * instanciateNic(uint32_t address, const std::string & name);
