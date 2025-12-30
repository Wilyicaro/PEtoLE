/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "../Packet.hpp"

AddPlayerPacket::AddPlayerPacket(const std::shared_ptr<Player>& player)
{
	m_guid = player->m_guid;
	m_name = RakNet::RakString(player->m_name.c_str());
	m_id = player->m_EntityID;
	m_pos = PacketUtil::packPos(player->m_pos);
	m_rotY = PacketUtil::packRot(player->m_rot.y);
	m_rotX = PacketUtil::packRot(player->m_rot.x);
	auto selected = player->getSelectedItem();
	m_itemID = !selected ? 0 : selected->m_itemID;;
}

void AddPlayerPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void AddPlayerPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_ADD_PLAYER);
	bs->Write(m_guid);
	bs->Write(m_name);
	bs->Write(m_id);
	bs->Write(m_pos.x);
	bs->Write(m_pos.y);
	bs->Write(m_pos.z);
	bs->Write(m_rotY);
	bs->Write(m_rotX);
	bs->Write(m_itemID);
}

void AddPlayerPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_guid);
	bs->Read(m_name);
	bs->Read(m_id);
	bs->Read(m_pos.x);
	bs->Read(m_pos.y);
	bs->Read(m_pos.z);
	bs->Read(m_rotY);
	bs->Read(m_rotX);
	bs->Read(m_itemID);
}