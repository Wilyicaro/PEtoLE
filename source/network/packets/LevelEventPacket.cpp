#include "../Packet.hpp"

void LevelEventPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void LevelEventPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_LEVEL_EVENT);
	bs->Write(m_type);
	bs->Write(m_pos.x);
	bs->Write(m_pos.z);
	bs->Write(m_pos.y);
	bs->Write(m_data);
}

void LevelEventPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_type);
	bs->Read(m_pos.x);
	bs->Read(m_pos.z);
	bs->Read(m_pos.y);
	bs->Read(m_data);
}
