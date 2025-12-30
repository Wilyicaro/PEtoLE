#include "../Packet.hpp"

void AddEntityPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void AddEntityPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_ADD_ENTITY);
	bs->Write(m_id);
	bs->Write(m_addId);
	bs->Write(m_pos.x);
	bs->Write(m_pos.y);
	bs->Write(m_pos.z);
	bs->Write(m_data);
	if (m_data > 0)
	{
		bs->Write(m_vel.x);
		bs->Write(m_vel.y);
		bs->Write(m_vel.z);
	}
}

void AddEntityPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_id);
	bs->Read(m_addId);
	bs->Read(m_pos.x);
	bs->Read(m_pos.y);
	bs->Read(m_pos.z);
	bs->Read(m_data);
	if (m_data > 0)
	{
		bs->Read(m_vel.x);
		bs->Read(m_vel.y);
		bs->Read(m_vel.z);
	}
}
