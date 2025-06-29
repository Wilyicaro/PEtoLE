/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>
#include "BitStream.h"
#include "common/Utils.hpp"
#include "world/phys/Vec3T.hpp"
#include "world/item/Inventory.hpp"

struct LevelData
{
private:
	int64_t m_seed;
	TilePos m_spawnPos;
	int64_t m_time;
	int64_t m_lastPlayed;
	int64_t m_sizeOnDisk;
	int field_1C;
	int m_storageVersion;
	int m_generatorVersion;
	std::string m_levelName;

private:
	void _init(int64_t seed = 0, int x = 19132);
	void _init(int64_t seed, int storageVersion, const std::string& name);

public:

	std::shared_ptr<CompoundTag> m_LocalPlayerData = nullptr;
	int m_nPlayers;

	LevelData() { _init(); }
	LevelData(int64_t seed, const std::string& name, int storageVersion) { _init(seed, storageVersion, name); }


	void deserialize(std::shared_ptr<CompoundTag> tag);
	std::shared_ptr<CompoundTag> serialize();

	/* Getters & Setters */

	int64_t getSeed() const { return m_seed; }
	int getXSpawn() const { return m_spawnPos.x; }
	int getYSpawn() const { return m_spawnPos.y; }
	int getZSpawn() const { return m_spawnPos.z; }
	const TilePos& getSpawn() const { return m_spawnPos; }
	int64_t getTime() const { return m_time; }
	int64_t getSizeOnDisk() const { return m_sizeOnDisk; }
	//CompoundTag getLoadedPlayerTag(); // Return type may actually be a pointer, not sure

	void setSeed(int64_t seed) { m_seed = seed; }
	void setXSpawn(int xSpawn) { m_spawnPos.x = xSpawn; }
	void setYSpawn(int ySpawn) { m_spawnPos.y = ySpawn; }
	void setZSpawn(int zSpawn) { m_spawnPos.z = zSpawn; }
	void setTime(int64_t time) { m_time = time; }
	void setSizeOnDisk(int64_t sizeOnDisk) { m_sizeOnDisk = sizeOnDisk; }

	void setSpawn(const TilePos& pos) { m_spawnPos = pos; }

	int getGeneratorVersion() const { return m_generatorVersion; }
	void setGeneratorVersion(int generatorVersion) { m_generatorVersion = generatorVersion; }

	int64_t getLastPlayed() const { return m_lastPlayed; }

	// inlined in 0.1.0 demo
	int getStorageVersion() const { return m_storageVersion; }
	void setStorageVersion(int storageVersion) { m_storageVersion = storageVersion; }

	GameType getGameType() const;
	void setGameType(GameType gameType) { /* Empty in 0.2.1 */ }

	// @TODO: Not Implemented
	bool getSpawnMobs() { return false; }
	void setSpawnMobs(bool spawnMobs) { }

	std::string getLevelName() const { return m_levelName; }
	void setLevelName(const std::string& name) { m_levelName = name; }

	void setLoadedPlayerTo(Player* player);
};

