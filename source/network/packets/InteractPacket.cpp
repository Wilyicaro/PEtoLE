#include "../Packet.hpp"

void InteractPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void InteractPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_INTERACT);
	bs->Write(m_source);
	bs->Write(m_target);
	bs->Write(m_action);
}

void InteractPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_source);
	bs->Read(m_target);
	bs->Read(m_action);
}
