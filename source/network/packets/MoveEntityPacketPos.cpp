#include "../Packet.hpp"

void MoveEntityPacketPos::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_MOVE_ENTITY);
	MoveEntityPacket::write(bs);
	bs->Write(m_pos.x);
	bs->Write(m_pos.y);
	bs->Write(m_pos.z);
}

void MoveEntityPacketPos::read(RakNet::BitStream* bs)
{
	MoveEntityPacket::read(bs);
	bs->Read(m_pos.x);
	bs->Read(m_pos.y);
	bs->Read(m_pos.z);
}
