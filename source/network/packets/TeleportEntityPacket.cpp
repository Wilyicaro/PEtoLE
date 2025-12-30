#include "../Packet.hpp"

void TeleportEntityPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void TeleportEntityPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_TELEPORT_ENTITY);
	bs->Write(m_id);
	bs->Write(m_pos.x);
	bs->Write(m_pos.y);
	bs->Write(m_pos.z);
	bs->Write(m_rotY);
	bs->Write(m_rotX);
}

void TeleportEntityPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_id);
	bs->Read(m_pos.x);
	bs->Read(m_pos.y);
	bs->Read(m_pos.z);
	bs->Read(m_rotY);
	bs->Read(m_rotX);
}
