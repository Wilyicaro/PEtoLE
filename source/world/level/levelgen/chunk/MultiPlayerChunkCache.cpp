/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LevelChunk.hpp"
#include "MultiPlayerChunkCache.hpp"
#include "world/level/Level.hpp"
#include "FakeLevelChunk.hpp"

MultiPlayerChunkCache::MultiPlayerChunkCache(Level* pLevel)
{
	m_chunkMap.reserve(1024);
	m_pLevel = nullptr;

	m_pLevel = pLevel;

	m_pFakeChunk = new FakeLevelChunk(m_pLevel, nullptr, ChunkPos(0, 0), FakeLevelChunk::NONE);
}


LevelChunk* MultiPlayerChunkCache::create(const ChunkPos& pos)
{
	LevelChunk* chunk = new LevelChunk(m_pLevel, new TileID[16 * 16 * 128], pos);
	std::fill(chunk->m_lightSky, chunk->m_lightSky + 16384, -1);
	std::fill(chunk->m_pBlockData, chunk->m_pBlockData + 32768, TILE_AIR);

	auto it = m_chunkMap.find(pos.key());
	if (it != m_chunkMap.end())
	{
		SAFE_DELETE(it->second);
		m_chunkMap.erase(it);
	}

	m_chunkMap[pos.key()] = chunk;
	chunk->m_bLoaded = true;
	return chunk;
}

LevelChunk* MultiPlayerChunkCache::getChunk(const ChunkPos& pos)
{
	if (!m_pLevel->isValidPos(pos))
	{
		auto it = m_fakeChunkMap.find(pos.key());
		if (it != m_fakeChunkMap.end())
			return it->second;
		LevelChunk* fakeChunk = new FakeLevelChunk(m_pLevel, nullptr, pos);
		m_fakeChunkMap[pos.key()] = fakeChunk;
		return fakeChunk;
	}

	auto& chunkMap = m_pLevel->isValidPos(pos) ? m_chunkMap : m_fakeChunkMap;

	auto it = m_chunkMap.find(pos.key());
	if (m_chunkMap.find(pos.key()) != m_chunkMap.end())
		return it->second;
	return m_pFakeChunk;
}


bool MultiPlayerChunkCache::hasChunk(const ChunkPos& pos)
{
	if (!m_pLevel->isValidPos(pos))
	{
		return m_fakeChunkMap.find(pos.key()) != m_fakeChunkMap.end();
	}

	return m_chunkMap.find(pos.key()) != m_chunkMap.end();
}

bool MultiPlayerChunkCache::shouldSave()
{
	return false;
}

int MultiPlayerChunkCache::tick()
{
	return 0;
}

LevelChunk* MultiPlayerChunkCache::getChunkDontCreate(const ChunkPos& pos)
{
	return getChunk(pos);
}

void MultiPlayerChunkCache::postProcess(ChunkSource*, const ChunkPos& pos)
{
}

std::string MultiPlayerChunkCache::gatherStats()
{
	return "MultiPlayerChunkCache: " + std::to_string(m_chunkMap.size());
}

MultiPlayerChunkCache::~MultiPlayerChunkCache()
{
	SAFE_DELETE(m_pFakeChunk);

	ChunkPos pos = ChunkPos(0, 0);


	for (auto it = m_chunkMap.begin(); it != m_chunkMap.end(); ) {
		if (!it->second) {
			++it;
			continue;
		}
		it->second->deleteBlockData();
		SAFE_DELETE(it->second);
		it = m_chunkMap.erase(it);
	}

	for (auto it = m_fakeChunkMap.begin(); it != m_fakeChunkMap.end(); ) {
		if (!it->second) {
			++it;
			continue;
		}
		it->second->deleteBlockData();
		SAFE_DELETE(it->second);
		it = m_fakeChunkMap.erase(it);
	}
}

