/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ServerLevelListener.hpp"
#include "network/ServerSideNetworkHandler.hpp"

ServerLevelListener::ServerLevelListener(ServerSideNetworkHandler* connection, Level* level) : m_pConnection(connection), m_pLevel(level)
{
	level->addListener(this);
}

ServerLevelListener::~ServerLevelListener()
{
	m_pLevel->removeListener(this);
}

void ServerLevelListener::tileChanged(const TilePos& pos)
{
	UpdateBlockPacket ubp;

	int tile = m_pLevel->getTile(pos);
	int data = m_pLevel->getData(pos);

	ubp.m_pos = pos;
	ubp.m_tile = uint8_t(tile);
	ubp.m_data = uint8_t(data);

	RakNet::BitStream bs;
	ubp.write(&bs);

	m_pConnection->m_pRakNetPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::AddressOrGUID(), true);
}