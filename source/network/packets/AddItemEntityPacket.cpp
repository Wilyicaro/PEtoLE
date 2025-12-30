#include "../Packet.hpp"

void AddItemEntityPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void AddItemEntityPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_ADD_ITEM_ENTITY);
	bs->Write(m_id);
	bs->Write(m_itemID);
	bs->Write(m_count);
	bs->Write(m_auxValue);
	bs->Write(m_pos.x);
	bs->Write(m_pos.y);
	bs->Write(m_pos.z);
	bs->Write(m_vel.x);
	bs->Write(m_vel.y);
	bs->Write(m_vel.z);
}

void AddItemEntityPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_id);
	bs->Read(m_itemID);
	bs->Read(m_count);
	bs->Read(m_auxValue);
	bs->Read(m_pos.x);
	bs->Read(m_pos.y);
	bs->Read(m_pos.z);
	bs->Read(m_vel.x);
	bs->Read(m_vel.y);
	bs->Read(m_vel.z);
}
