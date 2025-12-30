#include "../Packet.hpp"

void ContainerSetDataPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void ContainerSetDataPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_CONTAINER_SET_DATA);
	bs->Write(m_containerId);
	bs->Write(m_id);
	bs->Write(m_value);
}

void ContainerSetDataPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_containerId);
	bs->Read(m_id);
	bs->Read(m_value);
}
