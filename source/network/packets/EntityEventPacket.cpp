#include "../Packet.hpp"

void EntityEventPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void EntityEventPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_ENTITY_EVENT);
	bs->Write(m_id);
	bs->Write(m_event);
}

void EntityEventPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_id);
	bs->Read(m_event);
}
