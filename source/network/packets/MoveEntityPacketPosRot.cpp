#include "../Packet.hpp"

void MoveEntityPacketPosRot::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_MOVE_ENTITY_POS_ROT);
	MoveEntityPacket::write(bs);
	bs->Write(m_pos.x);
	bs->Write(m_pos.y);
	bs->Write(m_pos.z);
	bs->Write(m_rotY);
	bs->Write(m_rotX);
}

void MoveEntityPacketPosRot::read(RakNet::BitStream* bs)
{
	MoveEntityPacket::read(bs);
	bs->Read(m_pos.x);
	bs->Read(m_pos.y);
	bs->Read(m_pos.z);
	bs->Read(m_rotY);
	bs->Read(m_rotX);
}
