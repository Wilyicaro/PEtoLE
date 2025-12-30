#include "../Packet.hpp"

void AnimatePacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void AnimatePacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_ANIMATE);
	bs->Write(m_id);
	bs->Write(m_action);
}

void AnimatePacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_id);
	bs->Read(m_action);
}
