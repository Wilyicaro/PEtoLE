#include "../Packet.hpp"

void ContainerSetContentPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void ContainerSetContentPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_CONTAINER_SET_CONTENT);
	bs->Write(m_containerId);
	bs->Write((int16_t)m_items.size());

	for (const std::shared_ptr<ItemInstance>& item : m_items)
	{
		if (item)
		{
			bs->Write((int16_t)item->m_itemID);
			bs->Write((int8_t)item->m_count);
			bs->Write((int16_t)item->getAuxValue());
		}
		else
			bs->Write((int16_t)-1);
	}
}

void ContainerSetContentPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_containerId);
	int16_t size = 0;
	bs->Read(size);
	m_items.resize(size);

	for (int i = 0; i < size; i++)
	{
		int16_t id = -1;
		bs->Read(id);
		if (id >= 0)
		{
			int8_t count = 0;
			bs->Read(count);
			int16_t aux = 0;
			bs->Read(aux);
			m_items[i] = std::make_shared<ItemInstance>(id, count, aux);
		}
	}
}
