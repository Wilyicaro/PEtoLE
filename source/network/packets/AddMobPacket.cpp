#include "../Packet.hpp"

AddMobPacket::AddMobPacket(const std::shared_ptr<Mob>& mob) : m_entityData(mob->getEntityData())
{
	m_id = mob->m_EntityID;
	m_type = mob->getType()->getId();
	m_pos = mob->m_pos * 32;
	m_rotY = mob->m_rot.y * 256 / 360;
	m_rotX = mob->m_rot.x * 256 / 360;
}

void AddMobPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void AddMobPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_ADD_MOB);
	bs->Write(m_id);
	bs->Write(m_type);
	bs->Write(m_pos.x);
	bs->Write(m_pos.y);
	bs->Write(m_pos.z);
	bs->Write(m_rotX);
	bs->Write(m_rotY);
	m_entityData.packAll(*bs);
}

void AddMobPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_id);
	bs->Read(m_type);
	bs->Read(m_pos.x);
	bs->Read(m_pos.y);
	bs->Read(m_pos.z);
	bs->Read(m_rotX);
	bs->Read(m_rotY);
	m_unpack = SynchedEntityData::unpack(*bs);
}