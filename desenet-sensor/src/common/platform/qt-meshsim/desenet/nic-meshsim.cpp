
#include "desenet/networkinterfacedriver.h"
#include "desenet/networktimeprovider.h"
#include <QTcpSocket>
#include <QtCore/QDataStream>
#include <string>
#include <array>
#include <unistd.h>
#if (defined(Q_OS_LINUX) || defined(Q_OS_MAC))
    #include <arpa/inet.h>
#endif
#if defined(Q_OS_WINDOWS)
    #include <winsock2.h>
#endif

using std::string;
using std::array;

namespace MeshSimulator
{
    typedef enum
    {
        InvalidCI               = 0 ,	// Invalid Channel Identifier.
        FrameDataCI             = 1 ,	// Frame Data Channel Identifier, used for raw frames (simulation PDUs).
        SimulationIndicationCI  = 2 ,	// Simulation Indication Channel Identifier, used for ACK/NACK.
        SimulationControlCI     = 3 ,	// Simulation Control Channel Identifier, used to control the node.
        TraceCI                 = 4 ,	// Trace Channel Identifier, sends messages to the MeshSimulator (Not impl.)
        DebugCommandCI          = 5 ,	// Debug Command Channel Identifier, used to send commands to the code.
        RpcCI                   = 6 ,	// Remote Procedure Call Channel Identifier, not implemented on MeshSimulator.
        NodePrivMessagesCI      = 7		// Private Node Messages Channel Identifier, not implemented.
    } ChannelIdentifier;

    typedef enum
    {
        InvalidSIT              = 0 ,	// Invalid Simulation Indication Type.
        AckSIT                  = 1 ,	// Acknowledge from a node that received the frame.
        NackSIT                 = 2		// Not Acknowledged.
    } SimulationIndicationType;

    typedef enum
    {
        InvalidSCC              = 0 ,	// Invalid Simulation Control Command.
        SetMacAddressSCC        = 1 ,	// Changes the MAC address of the node. MUST BE THE FIRST MESSAGE!
        EnableNodeSCC           = 2 ,	// Enables the node (visual feedback) on the MeshSimulator.
        DisableNodeSCC          = 3 ,	// Disables the node including visual feedback on the MeshSimulator.
        ChangeColorSCC          = 4 ,	// Changes the color of the node inside the MeshSimulator UI.
        ChangeNameSCC           = 5		// Changes the name of the node inside the MeshSimulator UI.
    } SimulationControlCommand;

    typedef enum
    {
        InvalidDC               = 0 ,	// Invalid Debug Command.
        RunContDC               = 1 ,	// Starts/Continues execution.
        PauseDC                 = 2 ,	// Stops execution.
        StepDC                  = 3 ,	// Single step execution.
        ResetDC                 = 4 ,	// Resets the code.
        QuitDC                  = 5 ,	// Quits the application connected to the MeshSimulator.
        IdentifyDC              = 6		// Identifies the instance controlling the node.
    } DebugCommand;

    enum
    {
        MessageHeaderSize		=  5 ,	// The size of a message header (1 byte for channel, 4 bytes size).
        MessageChannelIdOffset	=  0 ,	// The first byte is always the channel ID.
        MessageLenghtOffset		=  1 ,	// The second up to the fifth are the size in big endian.
        MessageLengthSize		=  4 ,	// The length of the message is coded with four bytes.
        MessageCommandOffset	=  5 ,	// The position of the command.
        MessageCommandSize		=  1	// The size of a standard command.
    };
}

class NrfMeshsimDriver: public desenet::NetworkInterfaceDriver {
public:
    static constexpr const char *const Identifier = "MeshSim virtual Network Interface";

    NrfMeshsimDriver(uint32_t address, const string &name = "DeseNET Testbench"): address_(address), name_(name) {}

    ~NrfMeshsimDriver() {
        _ready = false;
    }

    string identifier() const override {
        return Identifier;
    }

    bool initialize() override {
        // Try to connect to the local mesh simulator.
        socket.connectToHost("localhost", 8844);
        if (!socket.waitForConnected(5000)) {
            return false;
        }

        // Prepare and send name.
        QByteArray nameCommand;
        QDataStream nameCommandBuilder(&nameCommand, QIODevice::WriteOnly);
        nameCommandBuilder.setByteOrder(QDataStream::BigEndian);
        nameCommandBuilder << static_cast<quint8>(MeshSimulator::SimulationControlCI)
                           << static_cast<quint32>(name_.length() + 1)
                           << static_cast<quint8>(MeshSimulator::ChangeNameSCC);
        nameCommandBuilder.writeRawData(name_.data(), name_.length());
        if (!socket.write(nameCommand)) {
            socket.close();
            return false;
        }
        socket.flush();
        usleep(10000);

        // Setup reception address to 0xE2E2E2E2.
        QByteArray addressCommand;
        QDataStream addressCommandBuilder(&addressCommand, QIODevice::WriteOnly);
        addressCommandBuilder.setByteOrder(QDataStream::BigEndian);
        addressCommandBuilder << static_cast<quint8>(MeshSimulator::SimulationControlCI)
                              << static_cast<quint32>(5)
                              << static_cast<quint8>(MeshSimulator::SetMacAddressSCC)
                              << address_;

        if (!socket.write(addressCommand)) {
            socket.close();
            return false;
        }
        socket.flush();

        // Handle data reception.
        QObject::connect(&socket, &QTcpSocket::readyRead, [this]() {
            while (socket.bytesAvailable() > MeshSimulator::MessageHeaderSize) {
                quint8 channelIdentifier;
                socket.read(reinterpret_cast<char *>(&channelIdentifier), 1);

                quint32 frameSize;
                socket.read(reinterpret_cast<char *>(&frameSize), 4);
                frameSize = htonl(frameSize);

                if (socket.bytesAvailable() >= frameSize) {
                    switch (channelIdentifier) {
                        case MeshSimulator::FrameDataCI: {
                                QByteArray data = socket.read(frameSize);
                                if (_receptionHandler) {

                                    const uint32_t receptionTime = desenet::NetworkTimeProvider::currentNetworkTime();
                                    _receptionHandler(*this, receptionTime, reinterpret_cast<const uint8_t *>(data.data() + 1), data.length() - 3);
                                }
                            }
                            break;

                        default:
                            socket.read(frameSize);
                            break;
                    }
                }
            }
        });

        _ready = true;
        return true;
    }

    bool isInitialized() const override {
        return _ready;
    }

    array<uint8_t,4> localAddress() const override {
        return {{0xE2, 0xE2, 0xE2, 0xE2}};
    }

    bool transmit(const uint8_t *const buffer, size_t length) override {
        QByteArray transmissionCommand;
        QDataStream transmissionCommandBuilder(&transmissionCommand, QIODevice::WriteOnly);
        transmissionCommandBuilder.setVersion(QDataStream::Qt_4_4);
        transmissionCommandBuilder.setByteOrder(QDataStream::BigEndian);
        transmissionCommandBuilder << static_cast<quint8>(MeshSimulator::FrameDataCI)
                                   << static_cast<quint32>(length + 3)
                                   << static_cast<quint8>(buffer[0] & 0x80 ? 0xAA : 0x55);
        transmissionCommandBuilder.writeRawData(reinterpret_cast<const char *>(buffer), length);
        transmissionCommandBuilder << qChecksum(reinterpret_cast<const char *>(buffer), length);

        int status = socket.write(transmissionCommand);
        socket.flush();
        return status;
    }

    void setReceptionHandler(ReceptionCallbackHandler handler) override {
        _receptionHandler = handler;
    }

private:
    bool _ready = false;
    uint32_t address_;
    string name_;
    ReceptionCallbackHandler _receptionHandler = nullptr;
    QTcpSocket socket;
};

class NrfMeshsimProber {
    class InterfaceDescriptor: public desenet::NetworkInterfaceDriver::Descriptor {
    public:
        explicit InterfaceDescriptor(const string &identifier): identifier_(identifier) {}

    private:
        // Descriptor interface implementation.
        std::string friendlyName() const override {
            return identifier_;
        }

        std::string identifier() const override {
            return identifier_;
        }

        desenet::NetworkInterfaceDriver *instanciate() const override {
            return new NrfMeshsimDriver(0xE2E2E2E2);
        }

        string identifier_;
    };

public:
    NrfMeshsimProber() {
        new InterfaceDescriptor(NrfMeshsimDriver::Identifier);
    }
};

NrfMeshsimProber __nrfMeshSimProber;

desenet::NetworkInterfaceDriver *instanciateNrfMeshsimDriver(uint32_t address, const string &name) {
    return new NrfMeshsimDriver(address, name);
}
