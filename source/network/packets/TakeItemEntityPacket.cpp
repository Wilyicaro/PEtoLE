#include "../Packet.hpp"

void TakeItemEntityPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void TakeItemEntityPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_TAKE_ITEM_ENTITY);
	bs->Write(m_itemID);
	bs->Write(m_id);
}

void TakeItemEntityPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_itemID);
	bs->Read(m_id);
}
