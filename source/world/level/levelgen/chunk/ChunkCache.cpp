/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LevelChunk.hpp"
#include "ChunkCache.hpp"
#include "world/level/Level.hpp"
#include "FakeLevelChunk.hpp"

ChunkCache::ChunkCache(Level* pLevel, ChunkStorage* pStor, ChunkSource* pSrc)
{
	m_chunkMap.reserve(1024);
	field_4 = true;
	m_pLevel = nullptr;
	m_pLastChunk = nullptr;
	m_lastChunkPos = ChunkPos(-999999999, -999999999);

	m_pChunkSource = pSrc;
	m_pChunkStorage = pStor;
	m_pLevel = pLevel;

	m_pFakeChunk = new FakeLevelChunk(m_pLevel, nullptr, ChunkPos(0, 0), FakeLevelChunk::NONE);
}


LevelChunk* ChunkCache::create(const ChunkPos& pos)
{
	return getChunk(pos);
}

LevelChunk* ChunkCache::getChunk(const ChunkPos& pos)
{
	// get the last chunk quickly if needed
	if (m_lastChunkPos == pos && m_pLastChunk)
	{
		return m_pLastChunk;
	}

	auto& chunkMap = pos.x < C_MIN_CHUNKS_X || pos.z < C_MIN_CHUNKS_Z || pos.x >= C_MAX_CHUNKS_X || pos.z >= C_MAX_CHUNKS_Z ? m_fakeChunkMap : m_chunkMap;

	if (!hasChunk(pos))
	{
		LevelChunk* pOldChunk = chunkMap[pos.key()];
		if (pOldChunk)
		{
			pOldChunk->unload();
			save(pOldChunk);
		}

		LevelChunk* pChunk = load(pos);
		if (pChunk)
		{
			chunkMap[pos.key()] = pChunk;
		}
		else
		{
			pChunk = m_pFakeChunk;
			if (m_pChunkSource)
				pChunk = m_pChunkSource->getChunk(pos);

			chunkMap[pos.key()] = pChunk;
			pChunk->lightLava();
		}

		pChunk = chunkMap[pos.key()];
		if (pChunk)
			pChunk->load();

		if (pChunk && !pChunk->m_bIsTerrainPopulated && hasChunk(ChunkPos(pos.x + 1, pos.z + 1)) && hasChunk(ChunkPos(pos.x, pos.z + 1)) && hasChunk(ChunkPos(pos.x + 1, pos.z)))
			postProcess(this, pos);

		//@OVERSIGHT: redundant call twice to hasChunk(x-1, z), hasChunk(x,z-1), and hasChunk(x-1,z-1)
		if (hasChunk(ChunkPos(pos.x - 1, pos.z)) && !getChunk(ChunkPos(pos.x - 1, pos.z))->m_bIsTerrainPopulated && hasChunk(ChunkPos(pos.x - 1, pos.z + 1)) && hasChunk(ChunkPos(pos.x, pos.z + 1)) && hasChunk(ChunkPos(pos.x - 1, pos.z)))
			postProcess(this, ChunkPos(pos.x - 1, pos.z));

		if (hasChunk(ChunkPos(pos.x, pos.z - 1)) && !getChunk(ChunkPos(pos.x, pos.z - 1))->m_bIsTerrainPopulated && hasChunk(ChunkPos(pos.x + 1, pos.z - 1)) && hasChunk(ChunkPos(pos.x + 1, pos.z)) && hasChunk(ChunkPos(pos.x, pos.z - 1)))
			postProcess(this, ChunkPos(pos.x, pos.z - 1));

		if (hasChunk(pos - 1) && !getChunk(pos - 1)->m_bIsTerrainPopulated && hasChunk(pos - 1) && hasChunk(ChunkPos(pos.x, pos.z - 1)) && hasChunk(ChunkPos(pos.x - 1, pos.z)))
			postProcess(this, pos - 1);
	}

	m_lastChunkPos = pos;
	m_pLastChunk = chunkMap[pos.key()];
	return m_pLastChunk;
}

LevelChunk* ChunkCache::getChunkDontCreate(const ChunkPos& pos)
{
	// get the last chunk quickly if needed
	if (m_lastChunkPos == pos)
	{
		if (m_pLastChunk)
			return m_pLastChunk;
	}

	if (pos.x < C_MIN_CHUNKS_X || pos.z < C_MIN_CHUNKS_Z || pos.x >= C_MAX_CHUNKS_X || pos.z >= C_MAX_CHUNKS_Z)
		return m_pFakeChunk;

	if (!hasChunk(pos))
	{
		LevelChunk* pOldChunk = m_chunkMap[pos.key()];
		if (pOldChunk)
		{
			pOldChunk->unload();
			save(pOldChunk);
			if (m_pChunkStorage)
				m_pChunkStorage->saveEntities(m_pLevel, pOldChunk);
		}

		// create an empty chunk
		LevelChunk* pChunk = m_pFakeChunk;
		if (m_pChunkSource)
			pChunk = m_pChunkSource->getChunkDontCreate(pos);

		m_chunkMap[pos.key()] = pChunk;
	}

	m_lastChunkPos = pos;
	m_pLastChunk = m_chunkMap[pos.key()];
	return m_pLastChunk;
}

bool ChunkCache::hasChunk(const ChunkPos& pos)
{
	if (m_lastChunkPos == pos)
		return true;

	if (pos.x < C_MIN_CHUNKS_X || pos.z < C_MIN_CHUNKS_Z || pos.x >= C_MAX_CHUNKS_X || pos.z >= C_MAX_CHUNKS_Z)
	{
		return m_fakeChunkMap.find(pos.key()) != m_fakeChunkMap.end();
	}

	LevelChunk* pChunk = m_chunkMap[pos.key()];
	if (!pChunk)
		return false;

	return pChunk->isAt(pos);
}

int ChunkCache::tick()
{
	if (m_pChunkStorage)
		m_pChunkStorage->tick();

	return m_pChunkSource->tick();
}

void ChunkCache::postProcess(ChunkSource* pChkSrc, const ChunkPos& pos)
{
	if (pos.x < C_MIN_CHUNKS_X || pos.z < C_MIN_CHUNKS_Z || pos.x >= C_MAX_CHUNKS_X || pos.z >= C_MAX_CHUNKS_Z)
		return;

	LevelChunk* pChunk = getChunk(pos);
	if (!pChunk->m_bIsTerrainPopulated)
	{
		pChunk->m_bIsTerrainPopulated = true;
		if (m_pChunkSource)
		{
			m_pChunkSource->postProcess(m_pChunkSource, pos);
			pChunk->markUnsaved();
		}
	}
}

void ChunkCache::save(LevelChunk* pChunk)
{
	if (m_pChunkStorage)
	{
		pChunk->m_lastSaveTime = m_pLevel->getTime();
		m_pChunkStorage->save(m_pLevel, pChunk);
	}
}

void ChunkCache::saveAll()
{
	if (!m_pChunkStorage) return;

	for (auto p : m_chunkMap) {
		if (p.second) m_pChunkStorage->save(m_pLevel, p.second);
	}
}

void ChunkCache::save(bool force)
{
	if (!m_pChunkStorage) return;

	int savedChunks = 0;

	for (auto it = m_chunkMap.begin(); it != m_chunkMap.end(); ) {
		if (!force && savedChunks == 24)
			return;

		LevelChunk* chunk = it->second;
		if (!chunk) {
			++it;
			continue;
		}

		if (chunk->shouldSave(force)) {
			savedChunks++;
			m_pChunkStorage->save(m_pLevel, chunk);
		}

		if (!m_pLevel->canChunkExist(chunk->m_chunkPos)) {
			chunk->unload();
			chunk->deleteBlockData();
			SAFE_DELETE(chunk);
			it = m_chunkMap.erase(it);
		}
		else {
			++it;
		}
	}
}

bool ChunkCache::save(bool force, ProgressListener& progress)
{
	int var3 = 0;
	int totalChunksToSave = 0;

	for (auto& it : m_chunkMap)
	{
		if (it.second && it.second->shouldSave(force))
			++totalChunksToSave;
	}

	int chunksSaved = 0;

	for (auto it = m_chunkMap.begin(); it != m_chunkMap.end(); ++it)
	{
		LevelChunk* chunk = it->second;

		if (chunk && chunk->shouldSave(force))
		{
			save(chunk);
			chunk->m_bUnsaved = false;
			++var3;
			if (var3 == 2 && !force)
				return false;

			++chunksSaved;
			if (chunksSaved % 10 == 0)
				progress.progressStagePercentage(chunksSaved * 100 / totalChunksToSave);
		}
	}

	if (force) {
		if (!m_pChunkStorage)
			return true;

		m_pChunkStorage->flush();
	}

	return true;
}

void ChunkCache::unloadChunk(std::pair< uint64_t, LevelChunk*> p)
{
	p.second->unload();
	p.second->deleteBlockData();
	SAFE_DELETE(p.second);
	m_chunkMap.erase(p.first);

}

bool ChunkCache::shouldSave()
{
	return true;
}

std::string ChunkCache::gatherStats()
{
	return "ChunkCache: " + std::to_string(m_chunkMap.size());
}

ChunkCache::~ChunkCache()
{
	SAFE_DELETE(m_pChunkSource);
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

LevelChunk* ChunkCache::load(const ChunkPos& pos)
{
	if (!m_pChunkStorage)
		return m_pFakeChunk;

	if (pos.x < C_MIN_CHUNKS_X || pos.z < C_MIN_CHUNKS_Z || pos.x >= C_MAX_CHUNKS_X || pos.z >= C_MAX_CHUNKS_Z)
	{
		return new FakeLevelChunk(m_pLevel, nullptr, pos);
	}

	LevelChunk* pChk = m_pChunkStorage->load(m_pLevel, pos);
	if (pChk)
		pChk->m_lastSaveTime = m_pLevel->getTime();
	
	return pChk;
}
