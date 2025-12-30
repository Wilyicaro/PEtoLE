#include "../Packet.hpp"

void SetEntityMotionPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void SetEntityMotionPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_SET_ENTITY_MOTION);
	bs->Write(m_id);
	bs->Write(m_vel.x);
	bs->Write(m_vel.y);
	bs->Write(m_vel.z);
}

void SetEntityMotionPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_id);
	bs->Read(m_vel.x);
	bs->Read(m_vel.y);
	bs->Read(m_vel.z);
}
