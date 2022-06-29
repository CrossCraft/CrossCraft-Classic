#include "Client.hpp"
#include "InPackets.hpp"
#include "OutPackets.hpp"
#include "../World/World.hpp"
#include <thread>

namespace CrossCraft::MP
{

    Client::Client(World* wrld, std::string ip, u16 port)
    {
        this->wrld = wrld;
        SC_APP_INFO("Connecting to: [" + ip + "]@" + std::to_string(port));

        my_socket = static_cast<int>(socket(PF_INET, SOCK_STREAM, 0));
        struct sockaddr_in name
        {
        };
        name.sin_family = AF_INET;
        name.sin_port = htons(port);

        inet_pton(AF_INET, ip.c_str(), &name.sin_addr.s_addr);
        bool b =
            (::connect(my_socket, (struct sockaddr *)&name, sizeof(name)) >= 0);

        if (!b)
        {
            SC_APP_ERROR("Failed to open a connection! (Is the server open?)");
            throw std::runtime_error("Fail!");
        }
        is_ready = false;
        connected = true;
        packetsIn.clear();
        packetsOut.clear();

        auto ptr = create_refptr<Outgoing::PlayerIdentification>();
        ptr->PacketID = Outgoing::OutPacketTypes::ePlayerIdentification;
        ptr->ProtocolVersion = 0x07;
        memset(ptr->Username.contents, 0x20, STRING_LENGTH);
        strcpy((char*)ptr->Username.contents, wrld->cfg.username.c_str());
        memset(ptr->VerificationKey.contents, 0x20, STRING_LENGTH);
        ptr->Unused = 0x00;

        packetsOut.push_back(Outgoing::createOutgoingPacket(ptr.get()));
        send();
    }
    Client::~Client() {}

    void Client::process_packet(RefPtr<Network::ByteBuffer> packet) {
        auto data = Incoming::readIncomingPacket(packet);

        if (data == nullptr)
            return;

        switch (data->PacketID) {
        case Incoming::eServerIdentification: {
            auto data2 = reinterpret_cast<Incoming::ServerIdentification*>(data.get());

            SC_APP_INFO("Connecting To Server: {}", data2->ServerName.contents);
            SC_APP_INFO("MOTD: {}", data2->MOTD.contents);

            break;
        }

        case Incoming::eLevelInitialize: {
            SC_APP_INFO("World Initializing!");
            ringbuffer = create_scopeptr<Network::ByteBuffer>(256 * 64 * 256 + 4);
            break;
        }

        case Incoming::eLevelDataChunk: {
            if (ringbuffer.get() != nullptr) {
                auto data2 = reinterpret_cast<Incoming::LevelDataChunk*>(data.get());

                auto len = data2->ChunkLength;
                ringbuffer->WriteBuf(data2->ChunkData.contents, len);

                auto percent = data2->PercentComplete;

                SC_APP_INFO("Getting World: {}%", percent);
            }

            break;
        }
        case Incoming::eLevelFinalize: {
            SC_APP_INFO("Level Obtained! Decompressing!");

            break;
        }


        default:
            SC_APP_WARN("UNKNOWN PACKET! {}", data->PacketID);
            break;
        }
    }


    void Client::update(double dt)
    {
        receive();

        for (auto& p : packetsIn)
            process_packet(p);
        packetsIn.clear();


        send();
    }

    void Client::draw()
    {
    }

    void Client::send()
    {
        for (auto &p : packetsOut)
        {

            int res =
                ::send(my_socket, p->m_Buffer, static_cast<int>(p->GetUsedSpace()), 0);

            if (res < 0)
            {
                SC_APP_ERROR("Client: Failed to send packets. Disconnecting.");
                connected = false;
                break;
            }
        }

        packetsOut.clear();
    }

    auto get_len(Byte type) -> int
    {
        using namespace Incoming;
        switch (static_cast<InPacketTypes>(type))
        {

        case eServerIdentification:
        {
            return 131;
        }
        case ePing:
        {
            return 1;
        }
        case eLevelInitialize:
        {
            return 1;
        }
        case eLevelDataChunk:
        {
            return 1028;
        }
        case eLevelFinalize:
        {
            return 7;
        }
        case eSetBlock:
        {
            return 8;
        }
        case eSpawnPlayer:
        {
            return 74;
        }
        case ePlayerTeleport:
        {
            return 10;
        }
        case ePlayerUpdate:
        {
            return 7;
        }
        case ePositionUpdate:
        {
            return 5;
        }
        case eOrientationUpdate:
        {
            return 4;
        }
        case eDespawnPlayer:
        {
            return 2;
        }
        case eMessage:
        {
            return 66;
        }
        case eDisconnect:
        {
            return 65;
        }
        case eUpdateUserType:
        {
            return 2;
        }

        default: {
            return -1;
        }

        }
    }

    void Client::receive()
    {
        for (int i = 0; i < 50; i++) {
            Byte newByte;
            int res = ::recv(my_socket, reinterpret_cast<char*>(&newByte), 1, MSG_PEEK);

            if (res <= 0)
                return;

            auto len = get_len(newByte);
            if (len < 0)
                return;

            auto byte_buffer = create_refptr<Network::ByteBuffer>(len);

            for (int i = 0; i < len; i++)
            {
                Byte b;
                ::recv(my_socket, reinterpret_cast<char*>(&b), 1, 0);

                byte_buffer->WriteU8(b);
            }

            packetsIn.push_back(byte_buffer);
        }
    }

}