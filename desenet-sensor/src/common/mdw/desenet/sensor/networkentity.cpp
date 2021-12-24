#include "networkentity.h"

#include <assert.h>


#include <array>
#include <list>
#include <map>
#include <utility>
#include <vector>

#include "abstractapplication.h"
#include "board/ledcontroller.h"
#include "desenet/beacon.h"
#include "desenet/frame.h"
#include "desenet/mpdu.h"
#include "desenet/timeslotmanager.h"
#include "platform-config.h"

using std::array;
using std::bind;
using std::list;
using std::make_pair;
using std::map;
using std::pair;
using std::vector;

using desenet::sensor::NetworkEntity;

NetworkEntity* NetworkEntity::_pInstance(
    nullptr);  // Instantiation of static attribute

NetworkEntity::NetworkEntity()
    : _pTimeSlotManager(nullptr), _pTransceiver(nullptr) {
    assert(!_pInstance);  // Only one instance allowed
    _pInstance = this;
    applications.clear();
}

NetworkEntity::~NetworkEntity() {}

void NetworkEntity::initialize(const desenet::SlotNumber& slotNumber) {
    // Save the slot number to reset the mpdu
    this->slotNumber = slotNumber;
}

void NetworkEntity::initializeRelations(ITimeSlotManager& timeSlotManager,
                                        NetworkInterfaceDriver& transceiver) {
    _pTimeSlotManager = &timeSlotManager;
    _pTransceiver = &transceiver;

    //->initialize(slot_number) is called in net.cpp
    _pTimeSlotManager->initializeRelations(*this);

    // Set the receive callback between transceiver and network. Bind this
    // pointer to member function
    transceiver.setReceptionHandler(std::bind(
        &NetworkEntity::onReceive, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

/**
 * This method does not automatically create an instance if there is none
 * created so far. It is up the the developer to create an instance of this
 * class prior to access the instance() method.
 */
// static
NetworkEntity& NetworkEntity::instance() {
    assert(_pInstance);
    return *_pInstance;
}

/**
 * Called by the NetworkInterfaceDriver (layer below) after reception of a new
 * frame
 */
void NetworkEntity::onReceive(NetworkInterfaceDriver& driver,
                              const uint32_t receptionTime,
                              const uint8_t* const buffer, size_t length) {
    (void)(driver); //Unused variables
    (void)(receptionTime);

    Frame frame = Frame::useBuffer(buffer, length);
    if (frame.type() == desenet::FrameType::Beacon) {
        // Cast the current frame to a beacon and MPDU
        Beacon beacon(frame);
        // Blink on the LED
        LedController::instance().flashLed(0);
        _pTimeSlotManager->onBeaconReceived(beacon.slotDuration());

        // Notify the different apps of the beaconr reception
        std::list<AppBind>::iterator i;
        for (i = applications.begin(); i != applications.end(); i++) {
            i->app.svSyncIndication(beacon.networkTime());
        }
        // Reset the MPDU (and put the current slotNumber)
        mpdu.reset(slotNumber);

        desenet::MPDU::ePDUHeader svPDU;
        // Store sample values in the mpdu
        for (i = applications.begin(); i != applications.end(); i++) {
            if (beacon.svGroupMask().test(i->group)) {
                // get the data from the sensor
                svPDU.fields.SVGroup_eventID = i->group;
                svPDU.fields.type = desenet::MPDU::ePDUType::SV;
                svPDU.fields.length =
                    i->app.svPublishIndication(i->group, mpdu.pduBuffer);
                if (svPDU.fields.length > 0)
                    mpdu.commitPDU(svPDU);
                else
                    break;
            }
        }

        // Add events to the MPDU (as much as possible)
        desenet::MPDU::ePDUHeader evPDU;
        while (eventsQueue.size() > 0 &&
               eventsQueue.begin()->data.length() <= (size_t)mpdu.remainingBytes()) {
            evPDU.fields.SVGroup_eventID = eventsQueue.begin()->id;
            evPDU.fields.type = desenet::MPDU::ePDUType::EV;
            // Writes data to the MPDU buffer, svApplication writes itself to
            // the buffer. Here we need to do it ourselves
            evPDU.fields.length = mpdu.writePDU(eventsQueue.begin()->data);

            if (evPDU.fields.length > 0) {
                // "Commit" the data to MPDU (set the appropriate header for the data
                // and prepare for the next one)
                mpdu.commitPDU(evPDU);
            }
            // Delete the event once it's used up
            eventsQueue.pop_front();
        }

        // Prepare the mpdu for sending
        mpdu.finalize();

        // Clearing the list. Too bad for the events that haven't been sent
        eventsQueue.clear();
    } else if (frame.type() == desenet::FrameType::MPDU) {
        // do nothing
    } else {
        // Invalid frame
    }
}

void NetworkEntity::onTimeSlotSignal(const ITimeSlotManager& timeSlotManager,
                                     const ITimeSlotManager::SIG& signal) {
    (void)timeSlotManager;
    // Flash the led when the timeslot is reached
    if (signal == ITimeSlotManager::SIG::OWN_SLOT_START) {
        LedController::instance().flashLed(0);
        // Send the MPDU frame
        transceiver() << mpdu;
    }
}

bool NetworkEntity::subscribeToSvGroup(AbstractApplication& app,
                                       SvGroup group) {
    // Add binding for this application to the list
    applications.push_back(AppBind{app, group});
    return true;
}

void NetworkEntity::unsubscribe(AbstractApplication& app) {
    // Remove the requested application from the list
    applications.remove_if(
        [&app](NetworkEntity::AppBind ab) { return &ab.app == &app; });
}

void NetworkEntity::eventReceived(const EvId& id,
                                  const SharedByteBuffer& evData) {
    // Create Event based on data
    EventElement newEvent(id, evData.copy());
    // Add it to the list
    eventsQueue.push_back(newEvent);
}
