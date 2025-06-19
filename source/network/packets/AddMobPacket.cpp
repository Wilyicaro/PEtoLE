/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "../Packet.hpp"

AddMobPacket::AddMobPacket(const Mob *mob)
{
	m_id = mob->m_EntityID;
	m_type = mob->getType().getId();
	m_pos = mob->m_pos;
	m_pos -= mob->m_heightOffset;
	m_rot = mob->m_rot;
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
	bs->Write(m_rot.y);
	bs->Write(m_rot.x);
}

void AddMobPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_id);
	bs->Read(m_type);
	bs->Read(m_pos.x);
	bs->Read(m_pos.y);
	bs->Read(m_pos.z);
	bs->Read(m_rot.y);
	bs->Read(m_rot.x);
}