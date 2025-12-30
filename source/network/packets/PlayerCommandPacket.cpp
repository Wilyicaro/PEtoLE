/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "../Packet.hpp"

void PlayerCommandPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void PlayerCommandPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_PLAYER_COMMAND);
	bs->Write(m_id);
	bs->Write(m_action);
}

void PlayerCommandPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_id);
	bs->Read(m_action);
}
