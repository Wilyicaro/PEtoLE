/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ChunkSource.hpp"
#include "FakeLevelChunk.hpp"
#include "world/level/Level.hpp"

ChunkSource::~ChunkSource()
{
}

void ChunkSource::saveAll()
{

}

LevelChunk* ChunkSource::createFakeChunk(Level* level, const ChunkPos& pos)
{
	return new FakeLevelChunk(level, nullptr, pos, level->getLevelData().isFlat() ? FakeLevelChunk::ContentType::FLAT : level->m_pDimension->m_bHasCeiling ? FakeLevelChunk::ContentType::NONE : FakeLevelChunk::ContentType::OCEAN);
}

void ChunkSource::save(bool force)
{

}

bool ChunkSource::save(bool force, ProgressListener&)
{
	return false;
}

void ChunkSource::unloadFarChunks()
{
}
