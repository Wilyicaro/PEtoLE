#include "../Packet.hpp"

void ContainerOpenPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void ContainerOpenPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_CONTAINER_OPEN);
	bs->Write(m_containerId);
	bs->Write(m_type);
	bs->Write(m_title);
	bs->Write(m_size);
}

void ContainerOpenPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_containerId);
	bs->Read(m_type);
	bs->Read(m_title);
	bs->Read(m_size);
}
