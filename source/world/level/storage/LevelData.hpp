/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>
#include "common/Utils.hpp"
#include "world/phys/Vec3T.hpp"
#include "world/item/Inventory.hpp"


struct DimensionLimit {
	ChunkPos m_minPos, m_maxPos;
	DimensionLimit() : m_minPos(ChunkPos()), m_maxPos(ChunkPos())
	{
	}
	DimensionLimit(ChunkPos pos) : m_minPos(-pos), m_maxPos(pos)
	{
	}
	DimensionLimit(ChunkPos minPos, ChunkPos maxPos) : m_minPos(minPos), m_maxPos(maxPos)
	{
	}

	bool operator==(const DimensionLimit& other) const
	{
		return other.m_minPos == m_minPos && other.m_maxPos == m_maxPos;
	}

	bool operator!=(const DimensionLimit& other) const
	{
		return !(other == *this);
	}

	static DimensionLimit ZERO;
};

struct LevelData
{
private:
	int64_t m_seed;
	TilePos m_spawnPos;
	int64_t m_time;
	int64_t m_lastPlayed;
	int64_t m_sizeOnDisk;
	int m_version;
	int m_dimension;
	int m_rainTime;
	bool m_bRaining;
	int m_thunderTime;
	bool m_bThundering;
	bool m_bIsFlat;
	std::string m_levelName;
	GameType m_gameType;
	std::unordered_map<int, DimensionLimit> m_levelLimit;

private:
	void _init(int64_t seed = 0, int x = 19132);
	void _init(int64_t seed, int storageVersion, const std::string& name);

public:

	std::shared_ptr<CompoundTag> m_LocalPlayerData = nullptr;

	LevelData() { _init(); }
	LevelData(int64_t seed, const std::string& name, int storageVersion) { _init(seed, storageVersion, name); }


	void load(CompoundIO tag);
	void save(CompoundIO) const;

	/* Getters & Setters */

	int64_t getSeed() const { return m_seed; }
	int getXSpawn() const { return m_spawnPos.x; }
	int getYSpawn() const { return m_spawnPos.y; }
	int getZSpawn() const { return m_spawnPos.z; }
	const TilePos& getSpawn() const { return m_spawnPos; }
	int64_t getTime() const { return m_time; }
	int64_t getSizeOnDisk() const { return m_sizeOnDisk; }

	void setSeed(int64_t seed) { m_seed = seed; }
	void setXSpawn(int xSpawn) { m_spawnPos.x = xSpawn; }
	void setYSpawn(int ySpawn) { m_spawnPos.y = ySpawn; }
	void setZSpawn(int zSpawn) { m_spawnPos.z = zSpawn; }
	void setTime(int64_t time) { m_time = time; }
	void setSizeOnDisk(int64_t sizeOnDisk) { m_sizeOnDisk = sizeOnDisk; }

	void setSpawn(const TilePos& pos) { m_spawnPos = pos; }

	int getDimension() const { return m_dimension; }

	int64_t getLastPlayed() const { return m_lastPlayed; }

	int getVersion() const { return m_version; }
	void setVersion(int version) { m_version = version; }

	GameType getGameType() const;
	void setGameType(GameType gameType) { m_gameType = gameType; }

	// @TODO: Not Implemented
	bool getSpawnMobs() { return false; }
	void setSpawnMobs(bool spawnMobs) { }

	std::string getLevelName() const { return m_levelName; }
	void setLevelName(const std::string& name) { m_levelName = name; }

	bool isThundering() const { return m_bThundering; }
	void setThundering(bool thunder) { m_bThundering = thunder; }

	int getThunderTime() const { return m_thunderTime; }
	void setThunderTime(int time) { m_thunderTime = time; }

	bool isRaining() const { return m_bRaining; }
	void setRaining(bool raining) { m_bRaining = raining; }

	int getRainTime() const { return m_rainTime; }
	void setRainTime(int time) { m_rainTime = time; }

	void setLoadedPlayerTo(Player* player);

	void setLimit(int dim, DimensionLimit);

	bool isValidPos(int dim, const ChunkPos& pos) const;
	const DimensionLimit& getLimit(int dim) const;

	bool isFlat() const { return m_bIsFlat; }
	void setFlat(bool flat) { m_bIsFlat = flat; }
};

