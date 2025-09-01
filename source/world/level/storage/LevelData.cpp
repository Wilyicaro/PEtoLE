/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LevelData.hpp"

#define FORCE_SURVIVAL_MODE (TEST_SURVIVAL_MODE || 0)

void LevelData::_init(int64_t seed, int version)
{
	m_seed = seed;
	m_time = 0;
	m_lastPlayed = 0;
	m_sizeOnDisk = 0;
	m_version = version;
	m_dimension = 0;
	m_bThundering = false;
	m_thunderTime = 0;
	m_bRaining = false;
	m_rainTime = 0;
	m_bIsFlat = false;
	m_gameType = GAME_TYPE_SURVIVAL;
}

void LevelData::_init(int64_t seed, int storageVersion, const std::string& name)
{
	_init(seed, storageVersion);
	m_levelName = name;
}

void LevelData::load(CompoundIO tag)
{
	m_seed = tag->getLong("RandomSeed");
	m_spawnPos = TilePos(tag->getInt("SpawnX"), tag->getInt("SpawnY"), tag->getInt("SpawnZ"));
	m_time = tag->getLong("Time");
	m_lastPlayed = tag->getLong("LastPlayed");
	m_sizeOnDisk = tag->getLong("SizeOnDisk");
	m_levelName = tag->getString("LevelName");
	m_version = tag->getInt("version");
	m_LocalPlayerData = tag->getCompound("Player");
	m_rainTime = tag->getInt("rainTime");
	m_bRaining = tag->getBoolean("raining");
	m_thunderTime = tag->getInt("thunderTime");
	m_bThundering = tag->getBoolean("thundering");
	m_bIsFlat = tag->getString("generatorName") == "FLAT";
	m_gameType = GameType(tag->getInt("GameType"));
	auto list = tag->getList("LevelLimit");
	if (list)
	{
		for (auto& tag : list->getValue())
		{
			auto limit = std::dynamic_pointer_cast<CompoundTag>(tag);
			if (limit)
				m_levelLimit[limit->getInt("Dimension")] = DimensionLimit(ChunkPos(limit->getInt("MinX"), limit->getInt("MinZ")), ChunkPos(limit->getInt("MaxX"), limit->getInt("MaxZ")));
		}
	}
	if (m_LocalPlayerData) m_dimension = m_LocalPlayerData->getInt("Dimension");
}

void LevelData::save(CompoundIO tag) const
{
	tag->putLong("RandomSeed", m_seed);
	tag->putInt("SpawnX", m_spawnPos.x);
	tag->putInt("SpawnY", m_spawnPos.y);
	tag->putInt("SpawnZ", m_spawnPos.z);
	tag->putLong("Time", m_time);
	tag->putLong("LastPlayed", getMillis());
	tag->putLong("SizeOnDisk", m_sizeOnDisk);
	tag->putString("LevelName", m_levelName);
	tag->putInt("version", 19132);
	tag->putInt("rainTime", m_rainTime);
	tag->putBoolean("raining", m_bRaining);
	tag->putInt("thunderTime", m_thunderTime);
	tag->putBoolean("thundering", m_bThundering);
	tag->putString("generatorName", m_bIsFlat ? "FLAT" : "DEFAULT");
	tag->putInt("GameType", m_gameType);

	if (!m_levelLimit.empty())
	{
		auto list = std::make_shared<ListTag>();
		for (auto& p : m_levelLimit)
		{
			if (p.second == DimensionLimit::ZERO) continue;
			auto limit = std::make_shared<CompoundTag>();
			limit->putInt("Dimension", p.first);
			limit->putInt("MinX", p.second.m_minPos.x);
			limit->putInt("MinZ", p.second.m_minPos.z);
			limit->putInt("MaxX", p.second.m_maxPos.x);
			limit->putInt("MaxZ", p.second.m_maxPos.z);
			list->add(limit);
		}
		tag->put("LevelLimit", list);
	}
	if (m_LocalPlayerData) tag->put("Player", m_LocalPlayerData);
}

GameType LevelData::getGameType() const
{
	return m_gameType;
}

void LevelData::setLoadedPlayerTo(Player* player)
{
	if (m_LocalPlayerData) 
	{
		player->load(m_LocalPlayerData);
		m_LocalPlayerData = nullptr;
	}
}

void LevelData::setLimit(int dim, DimensionLimit limit)
{
	m_levelLimit[dim] = limit;
}

bool LevelData::isValidPos(int dim, const ChunkPos& pos) const
{
	auto it = m_levelLimit.find(dim);
	return it == m_levelLimit.end() || (pos.x >= it->second.m_minPos.x && pos.z >= it->second.m_minPos.z && pos.x < it->second.m_maxPos.x && pos.z < it->second.m_maxPos.z);
}

const DimensionLimit& LevelData::getLimit(int dim) const
{
	auto it = m_levelLimit.find(dim);
	return it == m_levelLimit.end() ? DimensionLimit::ZERO : it->second;
}

DimensionLimit DimensionLimit::ZERO = DimensionLimit();