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

class McRegionLevelStorage : public ChunkStorage
{
public:
	McRegionLevelStorage(int i, const std::string& path);
	McRegionLevelStorage(const std::string& path);
	~McRegionLevelStorage();

	// ChunkStorage
	LevelChunk* load(Level* level, const ChunkPos& pos) override;
	void save(Level* level, LevelChunk* chunk) override;
	void saveEntities(Level* level, LevelChunk* chunk) override;

	RegionFile* createOrGetRegion(const ChunkPos& pos);

public:
	std::string m_path;
	std::unordered_map<uint64_t,RegionFile*> m_pRegionMap;
};

#endif
