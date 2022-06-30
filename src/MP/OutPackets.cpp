#include "OutPackets.hpp"

namespace CrossCraft::MP::Outgoing
{
    const int MAX_SIZE = 256;

    auto createOutgoingPacket(BasePacket *base_packet) -> RefPtr<Network::ByteBuffer>
    {
        auto id = base_packet->PacketID;

        switch (static_cast<OutPacketTypes>(id)) {
        case OutPacketTypes::ePlayerIdentification: {
            auto ptr = create_refptr<Network::ByteBuffer>(MAX_SIZE);
            auto data = reinterpret_cast<PlayerIdentification*>(base_packet);

            ptr->WriteU8(data->PacketID);
            ptr->WriteU8(data->ProtocolVersion);
            ptr->WriteBuf(data->Username.contents, STRING_LENGTH);
            ptr->WriteBuf(data->VerificationKey.contents, STRING_LENGTH);
            ptr->WriteU8(data->Unused);

            return ptr;
        }

        case OutPacketTypes::eSetBlock: {
            auto ptr = create_refptr<Network::ByteBuffer>(MAX_SIZE);
            auto data = reinterpret_cast<SetBlock*>(base_packet);

            ptr->WriteU8(data->PacketID);
            ptr->WriteI16(data->X);
            ptr->WriteI16(data->Y);
            ptr->WriteI16(data->Z);
            ptr->WriteU8(data->Mode);
            ptr->WriteU8(data->BlockType);
            
            return ptr;
        }

        case OutPacketTypes::ePositionAndOrientation: {
            auto ptr = create_refptr<Network::ByteBuffer>(MAX_SIZE);
            auto data = reinterpret_cast<PositionAndOrientation*>(base_packet);

            ptr->WriteU8(data->PacketID);
            ptr->WriteU8(data->PlayerID);
            ptr->WriteI16(data->X);
            ptr->WriteI16(data->Y);
            ptr->WriteI16(data->Z);
            ptr->WriteU8(data->Yaw);
            ptr->WriteU8(data->Pitch); 

            return ptr;
        }

        case OutPacketTypes::eMessage: {
            auto ptr = create_refptr<Network::ByteBuffer>(MAX_SIZE);
            auto data = reinterpret_cast<Message*>(base_packet);

            ptr->WriteU8(data->PacketID);
            ptr->WriteU8(data->Unused);
            ptr->WriteBuf(data->Message.contents, STRING_LENGTH);

            return ptr;
        }

        }

        return nullptr;
    }
}