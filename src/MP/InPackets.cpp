#include "InPackets.hpp"

const size_t MAX_SIZE = 1028;

namespace CrossCraft::MP::Incoming
{
    auto readIncomingPacket(RefPtr<Network::ByteBuffer> byte_buffer) -> RefPtr<BasePacket>
    {
        Byte id;
        byte_buffer->ReadU8(id);

        switch (static_cast<InPacketTypes>(id))
        {

        case eServerIdentification:
        {
            auto ptr = create_refptr<ServerIdentification>();
            ptr->PacketID = id;

            byte_buffer->ReadU8(ptr->ProtocolVersion);
            byte_buffer->ReadBuf(ptr->ServerName.contents, STRING_LENGTH);
            byte_buffer->ReadBuf(ptr->MOTD.contents, STRING_LENGTH);
            byte_buffer->ReadU8(ptr->UserType);

            return ptr;
        }

        case ePing:
        {
            auto ptr = create_refptr<Ping>();
            ptr->PacketID = id;

            return ptr;
        }
        case eLevelInitialize:
        {
            auto ptr = create_refptr<LevelInitialize>();
            ptr->PacketID = id;

            return ptr;
        }
        case eLevelDataChunk:
        {
            auto ptr = create_refptr<LevelDataChunk>();
            ptr->PacketID = id;

            byte_buffer->ReadI16(ptr->ChunkLength);
            byte_buffer->ReadBuf(ptr->ChunkData.contents, ARRAY_LENGTH);
            byte_buffer->ReadU8(ptr->PercentComplete);

            return ptr;
        }
        case eLevelFinalize:
        {
            auto ptr = create_refptr<LevelFinalize>();
            ptr->PacketID = id;

            byte_buffer->ReadI16(ptr->XSize);
            byte_buffer->ReadI16(ptr->YSize);
            byte_buffer->ReadI16(ptr->ZSize);

            return ptr;
        }

        case eSetBlock:
        {
            auto ptr = create_refptr<SetBlock>();
            ptr->PacketID = id;

            byte_buffer->ReadI16(ptr->X);
            byte_buffer->ReadI16(ptr->Y);
            byte_buffer->ReadI16(ptr->Z);
            byte_buffer->ReadU8(ptr->BlockType);

            return ptr;
        }
        case eSpawnPlayer:
        {
            auto ptr = create_refptr<SpawnPlayer>();
            ptr->PacketID = id;

            byte_buffer->ReadI8(ptr->PlayerID);
            byte_buffer->ReadBuf(ptr->PlayerName.contents, STRING_LENGTH);
            byte_buffer->ReadI16(ptr->X);
            byte_buffer->ReadI16(ptr->Y);
            byte_buffer->ReadI16(ptr->Z);
            byte_buffer->ReadU8(ptr->Yaw);
            byte_buffer->ReadU8(ptr->Pitch);

            return ptr;
        }
        case ePlayerTeleport:
        {
            auto ptr = create_refptr<PlayerTeleport>();
            ptr->PacketID = id;

            byte_buffer->ReadI8(ptr->PlayerID);
            byte_buffer->ReadI16(ptr->X);
            byte_buffer->ReadI16(ptr->Y);
            byte_buffer->ReadI16(ptr->Z);
            byte_buffer->ReadU8(ptr->Yaw);
            byte_buffer->ReadU8(ptr->Pitch);

            return ptr;
        }
        case ePlayerUpdate:
        {
            auto ptr = create_refptr<PlayerUpdate>();
            ptr->PacketID = id;

            byte_buffer->ReadI8(ptr->PlayerID);
            byte_buffer->ReadI16(ptr->DX);
            byte_buffer->ReadI16(ptr->DY);
            byte_buffer->ReadI16(ptr->DZ);
            byte_buffer->ReadU8(ptr->Yaw);
            byte_buffer->ReadU8(ptr->Pitch);

            return ptr;
        }
        case ePositionUpdate:
        {
            auto ptr = create_refptr<PositionUpdate>();
            ptr->PacketID = id;

            byte_buffer->ReadI8(ptr->PlayerID);
            byte_buffer->ReadI8(ptr->DX);
            byte_buffer->ReadI8(ptr->DY);
            byte_buffer->ReadI8(ptr->DZ);

            return ptr;
        }
        case eOrientationUpdate:
        {
            auto ptr = create_refptr<OrientationUpdate>();
            ptr->PacketID = id;

            byte_buffer->ReadI8(ptr->PlayerID);
            byte_buffer->ReadU8(ptr->Yaw);
            byte_buffer->ReadU8(ptr->Pitch);

            return ptr;
        }
        case eDespawnPlayer:
        {
            auto ptr = create_refptr<DespawnPlayer>();
            ptr->PacketID = id;

            byte_buffer->ReadI8(ptr->PlayerID);

            return ptr;
        }
        case eMessage:
        {
            auto ptr = create_refptr<Message>();
            ptr->PacketID = id;

            byte_buffer->ReadI8(ptr->PlayerID);
            byte_buffer->ReadBuf(ptr->Message.contents, STRING_LENGTH);

            return ptr;
        }
        case eDisconnect:
        {
            auto ptr = create_refptr<Disconnect>();
            ptr->PacketID = id;

            byte_buffer->ReadBuf(ptr->Reason.contents, STRING_LENGTH);

            return ptr;
        }
        case eUpdateUserType:
        {
            auto ptr = create_refptr<UpdateUserType>();
            ptr->PacketID = id;

            byte_buffer->ReadU8(ptr->UserType);

            return ptr;
        }
        }
        return nullptr;
    }
}