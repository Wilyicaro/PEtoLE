#pragma once

#include "Level.hpp"


class MultiPlayerLevel : public Level
{
private:
	struct ResetInfo
	{
		TilePos m_pos;
		int m_ticks;
		TileID m_tile;
		int m_data;

		ResetInfo(TilePos pos, TileID tile, int data) : m_pos(pos), m_ticks(80), m_tile(tile), m_data(data)
		{
		}
	};

	mutable LevelData m_levelData;
	std::unordered_map<int, std::shared_ptr<Entity>> m_entitiesById;
	std::unordered_set<std::shared_ptr<Entity>> m_reEntries;
	std::unordered_set<std::shared_ptr<Entity>> m_forced;
	std::vector<ResetInfo> m_updatesToReset;

public:

	MultiPlayerLevel(int64_t seed, Dimension* pDimension = nullptr);

	LevelData& getLevelData() const override { return m_levelData; }

	void tick() override;
	void clearResetRegion(const TilePos& pos, const TilePos& pos1);
	ChunkSource* createChunkSource() override;
	void tickTiles() override;
	bool tickPendingTicks(bool b) override;
	void addToTickNextTick(const TilePos& tilePos, int, int) override;
	bool addEntity(std::shared_ptr<Entity>) override;
	bool removeEntity(const std::shared_ptr<Entity>&) override;
	void entityAdded(const std::shared_ptr<Entity>& pEnt) override;
	void entityRemoved(const std::shared_ptr<Entity>& pEnt) override;
	void putEntity(int, std::shared_ptr<Entity>);
	std::shared_ptr<Entity> getEntity(int);
	std::shared_ptr<Entity> removeEntity(int);
	bool setDataNoUpdate(const TilePos&, int data) override;
	bool setTileAndDataNoUpdate(const TilePos& pos, TileID tile, int data) override;
	bool setTileNoUpdate(const TilePos& pos, TileID tile) override;
	bool doSetTileAndDataNoUpdate(const TilePos& pos, TileID tile, int data);
};