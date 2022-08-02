#include "Client.hpp"
#include "../World/World.hpp"
#include "InPackets.hpp"
#include "OutPackets.hpp"
#include <thread>
#include <zlib.h>

namespace CrossCraft::MP {

Client::Client(World *wrld, std::string ip, u16 port) {
    update_timer = 0.0f;
    this->wrld = wrld;
    SC_APP_INFO("Connecting to: [" + ip + "]@" + std::to_string(port));

    my_socket = static_cast<int>(socket(PF_INET, SOCK_STREAM, IPPROTO_TCP));

    struct sockaddr_in name {};
    name.sin_family = AF_INET;
    name.sin_port = htons(port);

    struct hostent* he = gethostbyname(ip.c_str());
    char* addr = inet_ntoa(*(struct in_addr*)he->h_addr_list[0]);

    inet_pton(AF_INET, addr, &name.sin_addr.s_addr);
    bool b =
        (::connect(my_socket, (struct sockaddr *)&name, sizeof(name)) >= 0);

#ifdef _WIN32
    unsigned long mode = (false) ? 0 : 1;
    ioctlsocket(my_socket, FIONBIO, &mode);
#endif

    if (!b) {
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
    strcpy((char *)ptr->Username.contents, wrld->cfg.username.c_str());
    memset(ptr->VerificationKey.contents, 0x20, STRING_LENGTH);
    ptr->Unused = 0x00;

    packetsOut.push_back(Outgoing::createOutgoingPacket(ptr.get()));
    send();

    // Model cube

    // Create color
    Rendering::Color c;
    c.color = 0xFFFFFFFF;

    // Push Back Verts

    // Front
    {
        mesh_data.push_back(Rendering::Vertex{
            0,
            0,
            c,
            0,
            0,
            1,
        });
        mesh_data.push_back(Rendering::Vertex{
            0,
            0,
            c,
            1,
            0,
            1,
        });

        mesh_data.push_back(Rendering::Vertex{
            0,
            0,
            c,
            1,
            1,
            1,
        });
        mesh_data.push_back(Rendering::Vertex{
            0,
            0,
            c,
            0,
            1,
            1,
        });
    }

    // Back Face
    {
        mesh_data.push_back(Rendering::Vertex{
            0,
            0,
            c,
            1,
            0,
            0,
        });
        mesh_data.push_back(Rendering::Vertex{
            0,
            0,
            c,
            0,
            0,
            0,
        });

        mesh_data.push_back(Rendering::Vertex{
            0,
            0,
            c,
            0,
            1,
            0,
        });

        mesh_data.push_back(Rendering::Vertex{
            0,
            0,
            c,
            1,
            1,
            0,
        });
    }

    // Left Face
    {
        mesh_data.push_back(Rendering::Vertex{
            0,
            0,
            c,
            0,
            0,
            0,
        });
        mesh_data.push_back(Rendering::Vertex{
            0,
            0,
            c,
            0,
            0,
            1,
        });

        mesh_data.push_back(Rendering::Vertex{
            0,
            0,
            c,
            0,
            1,
            1,
        });

        mesh_data.push_back(Rendering::Vertex{
            0,
            0,
            c,
            0,
            1,
            0,
        });
    }

    // Right Face
    {
        mesh_data.push_back(Rendering::Vertex{
            0,
            0,
            c,
            1,
            0,
            1,
        });
        mesh_data.push_back(Rendering::Vertex{
            0,
            0,
            c,
            1,
            0,
            0,
        });
        mesh_data.push_back(Rendering::Vertex{
            0,
            0,
            c,
            1,
            1,
            0,
        });
        mesh_data.push_back(Rendering::Vertex{
            0,
            0,
            c,
            1,
            1,
            1,
        });
    }

    // Top Face
    {
        mesh_data.push_back(Rendering::Vertex{
            0,
            0,
            c,
            0,
            1,
            1,
        });
        mesh_data.push_back(Rendering::Vertex{
            0,
            0,
            c,
            1,
            1,
            1,
        });

        mesh_data.push_back(Rendering::Vertex{
            0,
            0,
            c,
            1,
            1,
            0,
        });

        mesh_data.push_back(Rendering::Vertex{
            0,
            0,
            c,
            0,
            1,
            0,
        });
    }

    // Bottom Face
    {
        mesh_data.push_back(Rendering::Vertex{
            0,
            0,
            c,
            0,
            0,
            0,
        });
        mesh_data.push_back(Rendering::Vertex{
            0,
            0,
            c,
            1,
            0,
            0,
        });

        mesh_data.push_back(Rendering::Vertex{
            0,
            0,
            c,
            1,
            0,
            1,
        });

        mesh_data.push_back(Rendering::Vertex{
            0,
            0,
            c,
            0,
            0,
            1,
        });
    }

    // Push Back Indices
    for (int i = 0; i < 6; i++) {
        mesh_indices.push_back(0 + i * 4);
        mesh_indices.push_back(1 + i * 4);
        mesh_indices.push_back(2 + i * 4);
        mesh_indices.push_back(2 + i * 4);
        mesh_indices.push_back(3 + i * 4);
        mesh_indices.push_back(0 + i * 4);
    }

    mesh.add_data(mesh_data.data(), mesh_data.size(), mesh_indices.data(),
                  mesh_indices.size());
}
Client::~Client() {}

void Client::process_packet(RefPtr<Network::ByteBuffer> packet) {
    auto data = Incoming::readIncomingPacket(packet);

    if (data == nullptr)
        return;

    switch (data->PacketID) {
    case Incoming::eServerIdentification: {
        auto data2 =
            reinterpret_cast<Incoming::ServerIdentification *>(data.get());

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
            auto data2 =
                reinterpret_cast<Incoming::LevelDataChunk *>(data.get());

            auto len = data2->ChunkLength;
            ringbuffer->WriteBuf(data2->ChunkData.contents, len);

            auto percent = data2->PercentComplete;

            SC_APP_INFO("Getting World: {}%", percent);
        }

        break;
    }
    case Incoming::eLevelFinalize: {
        SC_APP_INFO("Level Obtained! Decompressing!");

        z_stream strm;
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;
        strm.avail_in = ringbuffer->GetUsedSpace();
        strm.next_in = (Bytef *)ringbuffer->m_Buffer;
        strm.avail_out = 0;
        strm.next_out = Z_NULL;

        int ret = inflateInit2(&strm, (MAX_WBITS + 16));
        if (ret != Z_OK) {
            throw std::runtime_error("INVALID START!");
        }

#ifdef PSP
        char *outBuffer = new char[256 * 64 * 256 + 4];
        strm.avail_out = 256 * 64 * 256 + 4;
#else
        char *outBuffer = new char[512 * 64 * 512 + 4];
        strm.avail_out = 512 * 64 * 512 + 4;
#endif
        strm.next_out = (Bytef *)outBuffer;

        ret = inflate(&strm, Z_FINISH);
        inflateEnd(&strm);
        int len = strm.total_out - 4;
        ringbuffer.release();

        int expected = ntohl(*((uint32_t *)(&outBuffer[0])));

        auto data2 = reinterpret_cast<Incoming::LevelFinalize *>(data.get());
        SC_APP_INFO("World Size {} {} {}", data2->XSize, data2->YSize,
                    data2->ZSize);

        wrld->world_size = {data2->XSize, data2->YSize, data2->ZSize};

        SC_APP_INFO("Decompressed {} bytes. Expected {}", len, expected);

        wrld->worldData = (block_t *)realloc(
            wrld->worldData,
            wrld->world_size.x * wrld->world_size.y * wrld->world_size.z);
        wrld->lightData = (uint16_t *)realloc(
            wrld->lightData, wrld->world_size.x *
                                 (wrld->world_size.y / 16 + 1) *
                                 wrld->world_size.z * sizeof(uint16_t));
        wrld->chunksMeta = (ChunkMeta *)realloc(
            wrld->chunksMeta, wrld->world_size.x / 16 *
                                  (wrld->world_size.y / 16 + 1) *
                                  wrld->world_size.z / 16 * sizeof(ChunkMeta));

        for (auto x = 0; x < wrld->world_size.x; x++)
            for (auto y = 0; y < wrld->world_size.y; y++)
                for (auto z = 0; z < wrld->world_size.z; z++) {
                    auto idx_source = (y * data2->XSize * data2->ZSize) +
                                      (z * data2->XSize) + x + 4;
                    auto idx_destiny = wrld->getIdx(x, y, z);

                    wrld->worldData[idx_destiny] = outBuffer[idx_source];
                    if (wrld->worldData[idx_destiny] == Block::Still_Water)
                        wrld->worldData[idx_destiny] = Block::Water;
                }

        delete[] outBuffer;

        // Update Lighting
        for (int x = 0; x < wrld->world_size.x; x++) {
            for (int z = 0; z < wrld->world_size.z; z++) {
                wrld->update_lighting(x, z);
            }
        }

        wrld->generate_meta();

        is_ready = true;

        break;
    }

    case Incoming::eMessage: {
        auto data2 = reinterpret_cast<Incoming::Message *>(data.get());
        wrld->player->chat->add_message(
            std::string((char *)data2->Message.contents));
        SC_APP_INFO("[Chat]: {}", data2->Message.contents);
        break;
    }

    case Incoming::eDespawnPlayer: {
        auto data2 = reinterpret_cast<Incoming::DespawnPlayer *>(data.get());
        auto id = data2->PlayerID;

        if (player_rep.find(id) != player_rep.end())
            player_rep.erase(id);

        break;
    }

    case Incoming::eSpawnPlayer: {
        auto data2 = reinterpret_cast<Incoming::SpawnPlayer *>(data.get());

        if (data2->PlayerID == -1) {
            wrld->player->pos = {(float)data2->X / 32.0f,
                                 (float)data2->Y / 32.0f,
                                 (float)data2->Z / 32.0f};
            wrld->player->rot = {(float)data2->Pitch / 256.0f * 360.0f,
                                 (float)data2->Yaw / 256.0f * 360.0f};
        } else {
            player_rep.emplace(data2->PlayerID,
                               PlayerInfo{data2->X, data2->Y, data2->Z,
                                          data2->Yaw, data2->Pitch});
        }

        break;
    }

    case Incoming::ePing: {
        break;
    }

    case Incoming::eSetBlock: {
        auto data2 = reinterpret_cast<Incoming::SetBlock *>(data.get());

        wrld->worldData[wrld->getIdx(data2->X, data2->Y, data2->Z)] =
            data2->BlockType;

        uint16_t x = data2->X / 16;
        uint16_t y = data2->Z / 16;
        uint32_t id = x << 16 | (y & 0x00FF);

        wrld->update_lighting(data2->X, data2->Z);

        if (wrld->chunks.find(id) != wrld->chunks.end())
            wrld->chunks[id]->generate(wrld);

        wrld->update_surroundings(data2->X, data2->Z);

        break;
    }

    case Incoming::ePlayerTeleport: {
        auto data2 = reinterpret_cast<Incoming::PlayerTeleport *>(data.get());

        if (data2->PlayerID == -1) {
            wrld->player->pos = {(float)data2->X / 32.0f,
                                 (float)data2->Y / 32.0f,
                                 (float)data2->Z / 32.0f};
            wrld->player->rot = {(float)data2->Pitch / 256.0f * 360.0f,
                                 (float)data2->Yaw / 256.0f * 360.0f};
        } else {
            if (player_rep.find(data2->PlayerID) != player_rep.end()) {
                player_rep[data2->PlayerID].X = data2->X;
                player_rep[data2->PlayerID].Y = data2->Y;
                player_rep[data2->PlayerID].Z = data2->Z;
                player_rep[data2->PlayerID].Yaw = data2->Yaw;
                player_rep[data2->PlayerID].Pitch = data2->Pitch;
            }
        }

        break;
    }

    default:
        SC_APP_WARN("UNKNOWN PACKET! {}", data->PacketID);
        break;
    }
}

void Client::update(double dt) {
    receive();

    for (auto &p : packetsIn)
        process_packet(p);
    packetsIn.clear();

    update_timer += dt;

    if (update_timer >= 0.05f) {
        update_timer = 0.0f;
        auto ptr = create_refptr<Outgoing::PositionAndOrientation>();
        ptr->PacketID = Outgoing::ePositionAndOrientation;
        ptr->PlayerID = -1;
        ptr->X = (short)(wrld->player->pos.x * 32.0f);
        ptr->Y = (short)((wrld->player->pos.y - 1.8f) * 32.0f) + 51;
        ptr->Z = (short)(wrld->player->pos.z * 32.0f);
        ptr->Pitch = (unsigned char)(wrld->player->rot.x / 360.0f * 255.0f);
        ptr->Yaw = (unsigned char)(wrld->player->rot.y / 360.0f * 255.0f);
        packetsOut.push_back(Outgoing::createOutgoingPacket(ptr.get()));
    }
    if (is_ready)
        send();
}

void Client::draw() {

    for (auto &[id, pinfo] : player_rep) {
        Rendering::RenderContext::get().matrix_clear();
        glm::vec3 entitypos = {(float)pinfo.X / 32.0f - 0.3f,
                               (float)pinfo.Y / 32.0f - 1.59675f,
                               (float)pinfo.Z / 32.0f - 0.3f};

        if ((entitypos - wrld->player->pos).length() >= 40.0f) {
            continue;
        }

        Rendering::RenderContext::get().matrix_translate(entitypos);

        Rendering::RenderContext::get().matrix_scale({0.6, 1.8, 0.6});

#if PSP
        sceGuDisable(GU_TEXTURE_2D);
#else
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
#endif

        mesh.draw();

#if PSP
        sceGuEnable(GU_TEXTURE_2D);
#else
        glEnable(GL_TEXTURE_2D);
#endif
    }
}

auto Client::set_block(short x, short y, short z, uint8_t mode, uint8_t block)
    -> void {
    auto ptr = create_refptr<Outgoing::SetBlock>();
    ptr->PacketID = Outgoing::eSetBlock;
    ptr->X = x;
    ptr->Y = y;
    ptr->Z = z;
    ptr->Mode = mode;
    ptr->BlockType = block;

    packetsOut.push_back(Outgoing::createOutgoingPacket(ptr.get()));
}

void Client::send() {
    for (auto &p : packetsOut) {

        int res = ::send(my_socket, p->m_Buffer,
                         static_cast<int>(p->GetUsedSpace()), 0);

        if (res < 0) {
            SC_APP_ERROR("Client: Failed to send packets. Disconnecting.");
            connected = false;
            break;
        }
    }

    packetsOut.clear();
}

auto get_len(Byte type) -> int {
    using namespace Incoming;
    switch (static_cast<InPacketTypes>(type)) {

    case eServerIdentification: {
        return 131;
    }
    case ePing: {
        return 1;
    }
    case eLevelInitialize: {
        return 1;
    }
    case eLevelDataChunk: {
        return 1028;
    }
    case eLevelFinalize: {
        return 7;
    }
    case eSetBlock: {
        return 8;
    }
    case eSpawnPlayer: {
        return 74;
    }
    case ePlayerTeleport: {
        return 10;
    }
    case ePlayerUpdate: {
        return 7;
    }
    case ePositionUpdate: {
        return 5;
    }
    case eOrientationUpdate: {
        return 4;
    }
    case eDespawnPlayer: {
        return 2;
    }
    case eMessage: {
        return 66;
    }
    case eDisconnect: {
        return 65;
    }
    case eUpdateUserType: {
        return 2;
    }

    default: {
        return -1;
    }
    }
}

void Client::receive() {
    Byte newByte;
    int res =
        ::recv(my_socket, reinterpret_cast<char *>(&newByte), 1, MSG_PEEK 
#if BUILD_PLAT != BUILD_WINDOWS
            | MSG_DONTWAIT
#endif
        );
    if (res <= 0)
        return;

    auto len = get_len(newByte);
    if (len < 0)
        return;

    auto byte_buffer = create_refptr<Network::ByteBuffer>(len);

    for (int i = 0; i < len; i++) {
        Byte b;
#if BUILD_PLAT == BUILD_WINDOWS
        int result = ::recv(my_socket, reinterpret_cast<char *>(&b), 1, 0);
        if (result != 1) {
            i--;
            continue;
        }
#else
        ::recv(my_socket, reinterpret_cast<char*>(&b), 1, 0);
#endif

        byte_buffer->WriteU8(b);
    }

#if PSP
    sceKernelDcacheWritebackInvalidateAll();
#endif

    packetsIn.push_back(byte_buffer);
}

} // namespace CrossCraft::MP