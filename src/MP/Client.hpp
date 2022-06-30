#pragma once
#include "ProtocolTypes.hpp"
#include "Rendering/Rendering.hpp"
#include "Utility.hpp"
#include <map>

#define DEFAULT_PORT 25565

namespace CrossCraft {
class World;
}

namespace CrossCraft::MP {

using namespace Stardust_Celeste;

struct PlayerInfo {
    short X;
    short Y;
    short Z;
    uint8_t Yaw;
    uint8_t Pitch;
};

class Client {
  public:
    Client(World *wrld, std::string ip, u16 port = DEFAULT_PORT);
    ~Client();

    void update(double dt);
    void draw();

    void send();
    void receive();

    std::vector<RefPtr<Network::ByteBuffer>> packetsOut;

    auto set_block(short x, short y, short z, uint8_t mode, uint8_t block)
        -> void;

    float update_timer;
    World *wrld;
    bool is_ready;

  private:
    ScopePtr<Network::ByteBuffer> ringbuffer;
    int my_socket;
    std::vector<RefPtr<Network::ByteBuffer>> packetsIn;
    bool connected;

    std::map<uint8_t, PlayerInfo> player_rep;

    void process_packet(RefPtr<Network::ByteBuffer> packet);
    Rendering::Mesh mesh;

    std::vector<Rendering::Vertex> mesh_data;
    std::vector<u16> mesh_indices;
};
} // namespace CrossCraft::MP