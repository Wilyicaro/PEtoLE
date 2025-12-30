/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "../Packet.hpp"

void UseItemPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void UseItemPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_USE_ITEM);
	bs->Write(m_pos.x);
	bs->Write(m_pos.z);
	bs->Write(m_pos.y);
	bs->Write(m_face);
	if (m_item)
	{
		bs->Write((int16_t)m_item->m_itemID);
		bs->Write((int8_t)m_item->m_count);
		bs->Write((int16_t)m_item->getAuxValue());
	}
	else bs->Write((int16_t)-1);
}

void UseItemPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_pos.x);
	bs->Read(m_pos.z);
	bs->Read(m_pos.y);
	bs->Read(m_face);
	int16_t id = -1;
	bs->Read(id);

	if (id >= 0)
	{
		int8_t count = 0;
		int16_t aux = 0;
		bs->Read(count);
		bs->Read(aux);
		m_item = std::make_shared<ItemInstance>(id, count, aux);
	}
	else
		m_item = nullptr;
}
