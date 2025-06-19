/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LevelData.hpp"

#define FORCE_SURVIVAL_MODE (TEST_SURVIVAL_MODE || 0)

void LevelData::_init(int64_t seed, int storageVersion)
{
	m_seed = seed;
	m_time = 0;
	m_lastPlayed = 0;
	m_sizeOnDisk = 0;
	field_1C = 0;
	m_storageVersion = storageVersion;
	m_generatorVersion = 1; // pre-0.2.1 versions used storageVersion instead
	m_nPlayers = -1;
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
	m_LocalPlayerData = tag->getCompound("Player");
}

std::shared_ptr<CompoundTag> LevelData::serialize()
{
	auto tag = std::make_shared<CompoundTag>();
	tag->putLong("RandomSeed", m_seed);
	tag->putInt("SpawnX", m_spawnPos.x);
	tag->putInt("SpawnY", m_spawnPos.y);
	tag->putInt("SpawnZ", m_spawnPos.z);
	tag->putLong("Time", m_time);
	tag->putLong("LastPlayed", getEpochTimeS());
	tag->putLong("SizeOnDisk", m_sizeOnDisk);
	tag->putString("LevelName", m_levelName);
	if (m_LocalPlayerData) tag->put("Player", m_LocalPlayerData);
	return tag;
}

GameType LevelData::getGameType() const
{
	return GAME_TYPE_SURVIVAL;
}

void LevelData::setLoadedPlayerTo(Player* player)
{
	if (m_LocalPlayerData) player->load(m_LocalPlayerData);
}
