#include "EntityTracker.hpp"
#include "Level.hpp"

EntityTracker::EntityTracker(MinecraftServer* server, int dim) : m_pServer(server), m_dimension(dim), m_chunkRange((server->getLevel(dim)->m_viewDistance - 1) * 16)
{
}

void EntityTracker::addTrackedEntity(const std::shared_ptr<Entity>& ent)
{
	addEntity(ent, ent->getType()->getRange(), ent->getType()->getUpdateInterval(), ent->getType()->trackDelta());
}

void EntityTracker::addEntity(const std::shared_ptr<Entity>& ent, int range, int updateInterval, bool trackDelta)
{
	if (m_entityMap.find(ent->m_EntityID) != m_entityMap.end())
		LOG_W("Entity is already tracked!");
	else
	{
		m_entityMap.try_emplace(ent->m_EntityID, ent, Mth::min(range, m_chunkRange), updateInterval, trackDelta);
		m_entityMap[ent->m_EntityID].updatePlayers(m_pServer->getLevel(m_dimension)->m_players);
	}
}

void EntityTracker::untrackEntity(const std::shared_ptr<Entity>& ent)
{
	if (ent->isPlayer())
	{
		std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(ent);
		for (auto& it : m_entityMap)
		{
			it.second.untrackEntity(player);
		}
	}

	auto it = m_entityMap.find(ent->m_EntityID);
	if (it != m_entityMap.end())
	{
		it->second.broadcastRemoved();
		m_entityMap.erase(it);
	}

}

void EntityTracker::tick()
{
	if (!needsBroadcasting()) return;

	std::vector<std::shared_ptr<Player>> sentChanges;

	for (auto& it : m_entityMap)
	{
		it.second.sendChanges(m_pServer->getLevel(m_dimension)->m_players);
		if (it.second.m_bSentChanges && it.second.m_entity->isPlayer())
			sentChanges.push_back(std::dynamic_pointer_cast<Player>(it.second.m_entity));
	}

	for (auto& p : sentChanges)
	{
		for (auto& it : m_entityMap)
		{
			if (it.second.m_entity != p)
				it.second.updatePlayer(p);
		}
	}
}

void EntityTracker::broadcast(const std::shared_ptr<Entity>& ent, Packet* packet, bool free)
{
	auto it = m_entityMap.find(ent->m_EntityID);
	if (it != m_entityMap.end())
	{
		it->second.broadcast(packet, false);
	}
	if (free)
		delete packet;
}

void EntityTracker::broadcastAndSend(const std::shared_ptr<Entity>& ent, Packet* packet, bool free)
{
	auto it = m_entityMap.find(ent->m_EntityID);
	if (it != m_entityMap.end())
	{
		it->second.broadcastAndSend(packet, false);
	}
	if (free)
		delete packet;
}

void EntityTracker::removePairing(const std::shared_ptr<Player>& ent)
{
	auto it = m_entityMap.find(ent->m_EntityID);
	if (it != m_entityMap.end())
	{
		it->second.removePairing(ent);
	}
}

bool EntityTracker::needsBroadcasting() const
{
	std::vector<std::shared_ptr<Player>>& players = m_pServer->getLevel(m_dimension)->m_players;
	//@Note: It doesn't really to check if it's a local player, as there isn't a dedicated server implementation yet, but why not? 
	return players.size() > 1 || (!players.empty() && !players[0]->isLocalPlayer());
}
