#include "storage/MinecraftServer.hpp"
#include "ServerEntity.hpp"

class EntityTracker
{
public:
	EntityTracker(MinecraftServer* server = nullptr, int dim = 0);

	void addTrackedEntity(const std::shared_ptr<Entity>& ent);
	void addEntity(const std::shared_ptr<Entity>& ent, int range, int updateInterval, bool trackedInterval = false);
	void untrackEntity(const std::shared_ptr<Entity>& ent);
	void tick();
	void broadcast(const std::shared_ptr<Entity>& ent, Packet*, bool free = true);
	void broadcastAndSend(const std::shared_ptr<Entity>& ent, Packet*, bool free = true);
	void removePairing(const std::shared_ptr<Player>& ent);

	bool needsBroadcasting() const;

private:
	std::unordered_map<int, ServerEntity> m_entityMap;
	MinecraftServer* m_pServer;
	int m_chunkRange;
	int m_dimension;
};