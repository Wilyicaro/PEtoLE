#pragma once
#include <world/entity/Entity.hpp>
#include <unordered_set>

class ServerEntity
{

public:
	ServerEntity();
	ServerEntity(const std::shared_ptr<Entity>& entity, int range, int updateInterval, bool trackDelta);

	int hashCode() const;
	bool operator==(const ServerEntity& other) const;

	void sendChanges(const std::vector<std::shared_ptr<Player>>& players);
	void broadcast(Packet*, bool free = true);
	void broadcastAndSend(Packet*, bool free = true);
	void broadcastRemoved();
	bool untrackEntity(const std::shared_ptr<Player>&);
	void updatePlayer(const std::shared_ptr<Player>&);
	void updatePlayers(const std::vector<std::shared_ptr<Player>>& player);
	void removePairing(const std::shared_ptr<Player>&);

private:
	Packet* getAddEntityPacket();

public:
	std::shared_ptr<Entity> m_entity;
	int m_range;
	int m_updateInterval;
	Vec3i m_pPos;
	int m_yRotp, m_xRotp;
	Vec3 m_vel;
	int m_tickCount;
	bool m_bSentChanges;
	std::unordered_set<std::shared_ptr<Player>> m_seenBy;

private:
	Vec3 m_pos;
	bool m_bForcedLoading;
	bool m_bTrackDelta;
	int m_teleportDelay;
};	

namespace std {
	template <>
	struct hash<ServerEntity>
	{
		std::size_t operator()(const ServerEntity& tick) const noexcept
		{
			return tick.hashCode();
		}
	};
}