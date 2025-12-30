#include "../Packet.hpp"

void SetCarriedItemPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void SetCarriedItemPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_SET_CARRIED_ITEM);
	bs->Write(m_slot);
}

void SetCarriedItemPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_slot);
}
