#include "../Packet.hpp"

void PlayerChangeDimensionPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void PlayerChangeDimensionPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_PLAYER_CHANGE_DIMENSION);
	bs->Write(m_dim);
}

void PlayerChangeDimensionPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_dim);
}
