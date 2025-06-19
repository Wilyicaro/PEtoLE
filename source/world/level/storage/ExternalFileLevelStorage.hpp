/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <list>
#include <unordered_map>
#include "LevelStorage.hpp"
#include "ChunkStorage.hpp"
#include "RegionFile.hpp"
#include <cstdint>

#ifndef DEMO

struct UnsavedLevelChunk
{
	int m_index;
	int m_foundTime;
	LevelChunk* m_pChunk;
};

class ExternalFileLevelStorage : public LevelStorage, ChunkStorage
{
public:
	ExternalFileLevelStorage(const std::string& a, const std::string& path);
	~ExternalFileLevelStorage();

	// LevelStorage
	LevelData* prepareLevel(Level* level) override;
	ChunkStorage* createChunkStorage(Dimension*) override;
	void saveLevelData(LevelData* levelData, std::vector<std::shared_ptr<Player>>& players) override;
	void savePlayerData(LevelData* levelData, std::vector<std::shared_ptr<Player>>& players) override;
	void closeAll() override;
	void tick() override;
	void flush() override;

	// ChunkStorage
	LevelChunk* load(Level* level, const ChunkPos& pos) override;
	void save(Level* level, LevelChunk* chunk) override;
	void saveEntities(Level* level, LevelChunk* chunk) override;

	static bool readLevelData(const std::string& path, LevelData* pLevelData);
	static bool readPlayerData(const std::string& path, LevelData* pLevelData);
	static bool writeLevelData(const std::string& path, LevelData* pLevelData);

public:
	RegionFile* createOrGetRegion(const ChunkPos& pos);
	std::string field_8;
	std::string m_levelDirPath;
	LevelData* m_pLevelData;
	std::unordered_map<uint64_t,RegionFile*> m_pRegionMap;
	Level* m_pLevel;
	int m_timer;
};

#endif
