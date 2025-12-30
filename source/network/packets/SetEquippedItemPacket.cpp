#include "../Packet.hpp"

void SetEquippedItemPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void SetEquippedItemPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_SET_EQUIPPED_ITEM);
	bs->Write(m_id);
	bs->Write(m_slot);
	bs->Write(m_itemID);
	bs->Write(m_auxValue);
}

void SetEquippedItemPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_id);
	bs->Read(m_slot);
	bs->Read(m_itemID);
	bs->Read(m_auxValue);
}
