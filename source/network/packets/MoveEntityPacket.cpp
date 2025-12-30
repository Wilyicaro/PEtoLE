#include "../Packet.hpp"

void MoveEntityPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void MoveEntityPacket::write(RakNet::BitStream* bs)
{
	bs->Write(m_id);
}

void MoveEntityPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_id);
}
