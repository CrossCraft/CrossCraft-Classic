#pragma once
#include "ProtocolTypes.hpp"
#include "Utility.hpp"

namespace CrossCraft::MP::Outgoing
{
    using namespace Stardust_Celeste;

    enum OutPacketTypes
    {
        ePlayerIdentification = 0x00,
        eSetBlock = 0x05,
        ePositionAndOrientation = 0x08,
        eMessage = 0x0d
    };

    struct PlayerIdentification : public BasePacket
    {
        Byte ProtocolVersion;
        String Username;
        String VerificationKey;
        Byte Unused;
    };

    struct SetBlock : public BasePacket
    {
        Short X;
        Short Y;
        Short Z;

        Byte Mode;
        Byte BlockType;
    };

    struct PositionAndOrientation : public BasePacket
    {
        Byte PlayerID;

        Short X;
        Short Y;
        Short Z;

        Byte Yaw;
        Byte Pitch;
    };

    struct Message : public BasePacket
    {
        Byte Unused;
        String Message;
    };

    auto createOutgoingPacket(BasePacket *base_packet) -> RefPtr<Network::ByteBuffer>;
}