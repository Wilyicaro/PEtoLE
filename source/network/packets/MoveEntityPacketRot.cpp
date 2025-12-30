#include "../Packet.hpp"

void MoveEntityPacketRot::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_MOVE_ENTITY_ROT);
	MoveEntityPacket::write(bs);
	bs->Write(m_rotY);
	bs->Write(m_rotX);
}

void MoveEntityPacketRot::read(RakNet::BitStream* bs)
{
	MoveEntityPacket::read(bs);
	bs->Read(m_rotY);
	bs->Read(m_rotX);
}
