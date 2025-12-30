#include "../Packet.hpp"

void ExplodePacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void ExplodePacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_EXPLODE);
	bs->Write(m_pos);
	bs->Write(m_range);
	bs->Write((int16_t)m_toBlow.size());

	for (const TilePos& pos : m_toBlow)
	{
		bs->Write(pos);
	}
}

void ExplodePacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_pos);
	bs->Read(m_range);
	int16_t size = 0;
	bs->Read(size);

	for (int i = 0; i < size; i++)
	{
		TilePos pos;
		bs->Read(pos);
		m_toBlow.insert(pos);
	}
}
