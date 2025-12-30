/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "../Packet.hpp"

void PlayerActionPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void PlayerActionPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_PLAYER_ACTION);
	bs->Write(m_action);
	bs->Write(m_pos.x);
	bs->Write(m_pos.z);
	bs->Write(m_pos.y);
	bs->Write(m_face);
}

void PlayerActionPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_action);
	bs->Read(m_pos.x);
	bs->Read(m_pos.z);
	bs->Read(m_pos.y);
	bs->Read(m_face);
}
