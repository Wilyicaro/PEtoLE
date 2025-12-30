#include "../Packet.hpp"

void AddPaintingPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void AddPaintingPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_ADD_PAINTING);
	bs->Write(m_id);
	bs->Write(m_motive);
	bs->Write(m_pos.x);
	bs->Write(m_pos.y);
	bs->Write(m_pos.z);
	bs->Write(m_dir);
}

void AddPaintingPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_id);
	bs->Read(m_motive);
	bs->Read(m_pos.x);
	bs->Read(m_pos.y);
	bs->Read(m_pos.z);
	bs->Read(m_dir);
}
