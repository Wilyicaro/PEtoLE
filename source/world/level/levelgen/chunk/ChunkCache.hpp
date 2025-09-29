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

class ChunkCache : public ChunkSource
{
public:
	ChunkCache(Level*, ChunkStorage*, ChunkSource*);
	virtual ~ChunkCache();

	LevelChunk* create(const ChunkPos& pos) override;
	LevelChunk* getChunk(const ChunkPos& pos) override;
	LevelChunk* getChunkDontCreate(const ChunkPos& pos) override;
	bool hasChunk(const ChunkPos& pos) override;
	std::string gatherStats() override;
	void postProcess(ChunkSource*, const ChunkPos& pos) override;
	bool shouldSave() override;
	void saveAll() override;
	int tick() override;
	void save(bool force = true) override;
	bool save(bool force, ProgressListener&) override;

	void unloadChunk(std::pair<uint64_t, LevelChunk*> p);

	LevelChunk* load(const ChunkPos& pos);
	void save(LevelChunk*);

public:
	bool field_4;
	LevelChunk* m_pEmptyChunk;
	ChunkSource* m_pChunkSource;
	ChunkStorage* m_pChunkStorage;
	std::unordered_map<uint64_t, LevelChunk*> m_chunkMap;
	std::unordered_map<uint64_t, LevelChunk*> m_fakeChunkMap;
	Level* m_pLevel;
	LevelChunk* m_pLastChunk;
	ChunkPos m_lastChunkPos;
};

