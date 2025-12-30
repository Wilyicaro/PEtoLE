#include "../Packet.hpp"

void SetEntityDataPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void SetEntityDataPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_SET_ENTITY_DATA);
	bs->Write(m_id);
	SynchedEntityData::pack(m_packedItems, *bs);
}

void SetEntityDataPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_id);
	m_unpack = SynchedEntityData::unpack(*bs);
}