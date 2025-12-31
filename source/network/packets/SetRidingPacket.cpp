#include "../Packet.hpp"

void SetRidingPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void SetRidingPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_SET_RIDING);
	bs->Write(m_rider);
	bs->Write(m_ridden);
}

void SetRidingPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_rider);
	bs->Read(m_ridden);
}
