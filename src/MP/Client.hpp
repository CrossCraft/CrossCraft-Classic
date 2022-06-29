#pragma once
#include "Utility.hpp"
#include "ProtocolTypes.hpp"

#define DEFAULT_PORT 25565

namespace CrossCraft {
    class World;
}

namespace CrossCraft::MP
{

    using namespace Stardust_Celeste;

    class Client
    {
    public:
        Client(World* wrld, std::string ip, u16 port = DEFAULT_PORT);
        ~Client();

        void update(double dt);
        void draw();

        void send();
        void receive();

        std::vector<RefPtr<Network::ByteBuffer>> packetsOut;

        auto set_block(short x, short y, short z, uint8_t mode, uint8_t block) -> void;

        float update_timer;
        World* wrld;
        bool is_ready;
    private:
        ScopePtr<Network::ByteBuffer> ringbuffer;
        int my_socket;
        std::vector<RefPtr<Network::ByteBuffer>> packetsIn;
        bool connected;

        void process_packet(RefPtr<Network::ByteBuffer> packet);
    };
}