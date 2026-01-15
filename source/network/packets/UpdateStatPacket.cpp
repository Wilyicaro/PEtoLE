#include "../Packet.hpp"

void UpdateStatPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void UpdateStatPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_UPDATE_STAT);
	bs->Write(m_id);
	bs->Write(m_amount);
}

void UpdateStatPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_id);
	bs->Read(m_amount);
}