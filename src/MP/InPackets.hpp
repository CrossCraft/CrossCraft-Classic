#pragma once
#include "ProtocolTypes.hpp"
#include "Utility.hpp"

namespace CrossCraft::MP::Incoming
{
    using namespace Stardust_Celeste;

    enum InPacketTypes
    {
        eServerIdentification = 0x00,
        ePing = 0x01,
        eLevelInitialize = 0x02,
        eLevelDataChunk = 0x03,
        eLevelFinalize = 0x04,
        eSetBlock = 0x06,
        eSpawnPlayer = 0x07,
        ePlayerTeleport = 0x08,
        ePlayerUpdate = 0x09,
        ePositionUpdate = 0x0a,
        eOrientationUpdate = 0x0b,
        eDespawnPlayer = 0x0c,
        eMessage = 0x0d,
        eDisconnect = 0x0e,
        eUpdateUserType = 0x0f
    };

    struct ServerIdentification : public BasePacket
    {
        Byte ProtocolVersion;
        String ServerName;
        String MOTD;
        Byte UserType;
    };

    struct Ping : public BasePacket
    {
    };

    struct LevelInitialize : public BasePacket
    {
    };

    struct LevelDataChunk : public BasePacket
    {
        Short ChunkLength;
        ByteArray ChunkData;
        Byte PercentComplete;
    };

    struct LevelFinalize : public BasePacket
    {
        Short XSize;
        Short YSize;
        Short ZSize;
    };

    struct SetBlock : public BasePacket
    {
        Short X;
        Short Y;
        Short Z;
        Byte BlockType;
    };

    struct SpawnPlayer : public BasePacket
    {
        SByte PlayerID;
        String PlayerName;

        Short X;
        Short Y;
        Short Z;

        Byte Yaw;
        Byte Pitch;
    };

    struct PlayerTeleport : public BasePacket
    {
        SByte PlayerID;

        Short X;
        Short Y;
        Short Z;

        Byte Yaw;
        Byte Pitch;
    };

    struct PlayerUpdate : public BasePacket
    {
        SByte PlayerID;

        Short DX;
        Short DY;
        Short DZ;

        Byte Yaw;
        Byte Pitch;
    };

    struct PositionUpdate : public BasePacket
    {
        SByte PlayerID;

        SByte DX;
        SByte DY;
        SByte DZ;
    };

    struct OrientationUpdate : public BasePacket
    {
        SByte PlayerID;

        Byte Yaw;
        Byte Pitch;
    };

    struct DespawnPlayer : public BasePacket
    {
        SByte PlayerID;
    };

    struct Message : public BasePacket
    {
        SByte PlayerID;
        String Message;
    };

    struct Disconnect : public BasePacket
    {
        String Reason;
    };

    struct UpdateUserType : public BasePacket
    {
        Byte UserType;
    };

    auto readIncomingPacket(RefPtr<Network::ByteBuffer> byte_buffer) -> RefPtr<BasePacket>;
}