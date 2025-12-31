#include "../Packet.hpp"

void MapItemDataPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void MapItemDataPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_MAP_ITEM_DATA);
	bs->Write(m_item);
	bs->Write(m_mapId);
	bs->Write((uint8_t)m_data.size());
	bs->WriteAlignedBytes(m_data.data(), m_data.size());
}

void MapItemDataPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_item);
	bs->Read(m_mapId);
	uint8_t size = 0;
	bs->Read(size);
	m_data.resize(size);
	if (size > 0)
		bs->ReadAlignedBytes(m_data.data(), size);
}
