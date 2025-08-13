/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "../Packet.hpp"

void SignUpdatePacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void SignUpdatePacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_SIGN_UPDATE);
	bs->Write(m_pos.x);
	bs->Write(m_pos.z);
	bs->Write(m_pos.y);
	for (auto& s : m_messages)
		bs->Write(s);
}

void SignUpdatePacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_pos.x);
	bs->Read(m_pos.z);
	bs->Read(m_pos.y);
	for (int i = 0; i < m_messages.size(); ++i)
		bs->Read(m_messages[i]);
}
