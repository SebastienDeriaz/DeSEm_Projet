#ifndef DESNET_SENSOR_ABSTRACTAPPLICATION_H
#define DESNET_SENSOR_ABSTRACTAPPLICATION_H

#include "platform-config.h"
#include "desenet/types.h"

namespace desenet {
namespace sensor {

    class NetworkEntity;

    /**
     * @brief DESENET application base class.
     *
     * All DESENET applications have to derive from this base class in order to be able
     * to call the DESENET service primitives and in order to get notified about service
     * indications or confirmations.
     */
    class AbstractApplication
    {
        friend class NetworkEntity;
    public:
        AbstractApplication() {}				///< @brief Default constructor.
        virtual ~AbstractApplication() {}		///< @brief Destructor.

    protected:
        /**
         * @brief Registers the application at the DESENET stack in order to get synchronized.
         *
         * @see svSyncIndication()
         */
        void svSyncRequest();

        /**
         * @brief Using this method an application can ask to publish sampled values for the given group.
         *
         * As only one application can publish data for a given sampled values group, the method can fail if there is already another
         * application publishing under this group.
         *
         * @param group The sampled values group to publish to.
         * @return      True on success, false if there is already another application publishing for that group.
         * @see         svPublishIndication()
         */
        bool svPublishRequest(SvGroup group);

        /**
         * @brief Publishes the given event.
         *
         * Note that the shared buffer given to the stack will be copied.
         *
         * @param id        ID of the event.
         * @param evData    The actual data of the event.
         */
        static void evPublishRequest(EvId id, const SharedByteBuffer & evData);

    private:
        /**
         * @brief This method will be called every time a Beacon frame has been received from the Master Node.
         *
         * This method might be called in the context of an ISR!
         *
         * @param time  The actual network time in milliseconds since UNIX epoch.
         * @see         svSyncRequest()
         */
        virtual void svSyncIndication(NetworkTime time);

        /**
         * @brief This method will be called by the DESENET stack in order to signal that the application can now write the sampled values into
         *        the ePDU buffer.
         *
         * This method might be called in the context of an ISR!
         *
         * @param group     The sampled values group to publish the data for.
         * @param svData    Shared buffer where the actual sampled values data can be written to.
         * @return          The number of bytes written to the buffer.
         * @see             svPublishRequest()
         */
        virtual SharedByteBuffer::sizeType svPublishIndication(SvGroup group, SharedByteBuffer &svData);
    };
} // namespace sensor
} // namespace desenet
#endif // DESNET_SENSOR_ABSTRACTAPPLICATION_H
