#include "../Packet.hpp"

void ContainerClickPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void ContainerClickPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_CONTAINER_CLICK);
	bs->Write(m_containerId);
	bs->Write(m_slot);
	bs->Write(m_button);
	bs->Write(m_uid);
	bs->Write(m_bQuickMove);
	if (m_item)
	{
		bs->Write((int16_t)m_item->m_itemID);
		bs->Write((int8_t)m_item->m_count);
		bs->Write((int16_t)m_item->getAuxValue());
	}
	else bs->Write((int16_t)-1);
}

void ContainerClickPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_containerId);
	bs->Read(m_slot);
	bs->Read(m_button);
	bs->Read(m_uid);
	bs->Read(m_bQuickMove);
	int16_t id = -1;
	bs->Read(id);

	if (id >= 0)
	{
		int8_t count = 0;
		int16_t aux = 0;
		bs->Read(count);
		bs->Read(aux);
		m_item = std::make_shared<ItemInstance>(id, count, aux);
	}
	else
		m_item = nullptr;
}
