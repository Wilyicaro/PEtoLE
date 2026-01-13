/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ServerLevelListener.hpp"
#include "network/ServerSideNetworkHandler.hpp"
#include "world/level/EntityTracker.hpp"

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
	m_pLevel->broadcastAll(new TileUpdatePacket(pos, m_pLevel));
}

void ServerLevelListener::entityAdded(const std::shared_ptr<Entity>& ent)
{
	m_pLevel->getServer()->getEntityTracker(m_pLevel->m_pDimension->m_ID).addTrackedEntity(ent);
}

void ServerLevelListener::entityRemoved(const std::shared_ptr<Entity>& ent)
{
	m_pLevel->getServer()->getEntityTracker(m_pLevel->m_pDimension->m_ID).untrackEntity(ent);
}

void ServerLevelListener::levelEvent(Player* player, LevelEvent event, const TilePos& pos, int data)
{
	m_pLevel->broadcastToAllInRange(player, pos, 64, new LevelEventPacket(event, pos, data));
}
