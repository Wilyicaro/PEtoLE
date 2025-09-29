/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <cstring>
#include "ChunkSource.hpp"
#include "world/level/storage/ChunkStorage.hpp"
#include "world/level/Level.hpp"
#include <unordered_map>
#include <cstdint>

class Level;

class MultiPlayerChunkCache : public ChunkSource
{
public:
	MultiPlayerChunkCache(Level*);
	virtual ~MultiPlayerChunkCache();

	LevelChunk* create(const ChunkPos& pos) override;
	LevelChunk* getChunk(const ChunkPos& pos) override;
	bool hasChunk(const ChunkPos& pos) override;
	std::string gatherStats() override;
	bool shouldSave() override;
	int tick() override;
	LevelChunk* getChunkDontCreate(const ChunkPos& pos) override;
	void postProcess(ChunkSource*, const ChunkPos& pos) override;

public:
	LevelChunk* m_pEmptyChunk;
	std::unordered_map<uint64_t, LevelChunk*> m_chunkMap;
	std::unordered_map<uint64_t, LevelChunk*> m_fakeChunkMap;
	Level* m_pLevel;
};

