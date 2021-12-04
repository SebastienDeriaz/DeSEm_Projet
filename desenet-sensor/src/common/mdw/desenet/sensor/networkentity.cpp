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
    (void)(driver);
    (void)(receptionTime);
    Frame frame = Frame::useBuffer(buffer, length);
    if (frame.type() == desenet::FrameType::Beacon) {
        // Cast the current frame to a beacon and MPDU
        Beacon beacon(frame);
        // Blink on the LED
        LedController::instance().flashLed(0);
        _pTimeSlotManager->onBeaconReceived(beacon.slotDuration());

        std::list<AppBind>::iterator i;
        for (i = applications.begin(); i != applications.end(); i++) {
            i->app.svSyncIndication(beacon.networkTime());
        }
        mpdu.reset(slotNumber);

        desenet::MPDU::ePDUHeader ePDU;
        for (i = applications.begin(); i != applications.end(); i++) {
            if (beacon.svGroupMask().test(i->group)) {
                // get the data from the sensor
                ePDU.fields.SVGroup_eventID = i->group;
                ePDU.fields.type = desenet::MPDU::ePDUType::SV;
                ePDU.fields.length =
                    i->app.svPublishIndication(i->group, mpdu.pduBuffer);
                if (ePDU.fields.length > 0)
                    mpdu.commitPDU(ePDU);
                else
                    break;
            }
        }

        mpdu.finalize();

        // Insérer toutes les données dans le MPDU

        // Lorsqu'on demande un buffer, on utilise
        // SharedByteBuffer.proxy(length, maxLength) Une fois que les données
        // sont écrites (grace à svPublishIndication), on effectue un "commit"
        // qui va entrer la bonne longueur au début du ePDU

        // Créer un champ de bits pour les valeurs dans le MPDU header

        // On supprime l'événement une fois qu'on l'as ajouté au buffer

        // Si on a pas réussi à envoyer tous les éléments, on vide la liste,
        // tant pis pour ceux qui ont pas été envoyés
    } else if (frame.type() == desenet::FrameType::MPDU) {
        // do nothing ?
    } else {
        // Invalid frame
    }
}

void NetworkEntity::onTimeSlotSignal(const ITimeSlotManager& timeSlotManager,
                                     const ITimeSlotManager::SIG& signal) {
    // Flash the led when the timeslot is reached
    if (signal == ITimeSlotManager::SIG::OWN_SLOT_START) {
        LedController::instance().flashLed(0);
        // Send the MPDU frame
        transceiver() << mpdu;
    }
}

bool NetworkEntity::subscribeToSvGroup(AbstractApplication& app,
                                       SvGroup group) {
    applications.push_back(AppBind{app, group});
    return true;
}

void NetworkEntity::unsubscribe(AbstractApplication& app) {
    applications.remove_if(
        [&app](NetworkEntity::AppBind ab) { return &ab.app == &app; });
}
