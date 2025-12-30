#include "../Packet.hpp"

void GameEventPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void GameEventPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_GAME_EVENT);
	bs->Write(m_event);
}

void GameEventPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_event);
}
