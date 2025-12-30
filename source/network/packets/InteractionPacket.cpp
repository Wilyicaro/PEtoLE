#include "../Packet.hpp"

void InteractionPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void InteractionPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_INTERACTION);
	bs->Write(m_id);
	bs->Write(m_pos.x);
	bs->Write(m_pos.z);
	bs->Write(m_pos.y);
	bs->Write(m_type);
}

void InteractionPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_id);
	bs->Read(m_pos.x);
	bs->Read(m_pos.z);
	bs->Read(m_pos.y);
	bs->Read(m_type);
}
