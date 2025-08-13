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
}

void LevelData::_init(int64_t seed, int storageVersion, const std::string& name)
{
	_init(seed, storageVersion);
	m_levelName = name;
}

void LevelData::deserialize(std::shared_ptr<CompoundTag> tag)
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
	if (m_LocalPlayerData) m_dimension = m_LocalPlayerData->getInt("Dimension");
}

std::shared_ptr<CompoundTag> LevelData::serialize() const
{
	auto tag = std::make_shared<CompoundTag>();
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
	if (m_LocalPlayerData) tag->put("Player", m_LocalPlayerData);
	return tag;
}

GameType LevelData::getGameType() const
{
	return GAME_TYPE_SURVIVAL;
}

void LevelData::setLoadedPlayerTo(Player* player)
{
	if (m_LocalPlayerData) 
	{
		player->load(m_LocalPlayerData);
		m_LocalPlayerData = nullptr;
	}
}
