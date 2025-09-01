/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "FlatLevelSource.hpp"
#include "world/level/Level.hpp"
#include "world/tile/SandTile.hpp"

FlatLevelSource::FlatLevelSource(Level* level, int64_t seed) :
	m_random(seed),
	m_pLevel(level)
{
	LOG_I("Generating world with seed: %d", seed);

	LOG_I("random.get : %d", m_random.nextInt() >> 1);
}

LevelChunk* FlatLevelSource::getChunk(const ChunkPos& pos)
{
	m_random.setSeed(341873128712L * pos.x + 132897987541L * pos.z);

	TileID* pLevelData = new TileID[32768];

	LevelChunk* pChunk = new LevelChunk(m_pLevel, pLevelData, pos);

	buildSurfaces(pos, pLevelData);
	pChunk->recalcHeightmap();

	return pChunk;
}

LevelChunk* FlatLevelSource::getChunkDontCreate(const ChunkPos& pos)
{
	// have to create the chunk. Create an empty one

	TileID* pLevelData = new TileID[32768];
	memset (pLevelData, 0, sizeof *pLevelData * 32768);

	LevelChunk* pChunk = new LevelChunk(m_pLevel, pLevelData, pos);

	return pChunk;
}

void FlatLevelSource::buildSurfaces(const ChunkPos& pos, TileID* tiles)
{
	for (int k = 0; k < 16; k++)
	{
		for (int l = 0; l < 16; l++)
		{
			for (int k1 = 127; k1 >= 0; k1--)
			{
				int l1 = (l * 16 + k) * 128 + k1;
				if (k1 == 0)
				{
					tiles[l1] = Tile::bedrock->m_ID;
					continue;
				}

				if (k1 <= 2)
				{
					tiles[l1] = Tile::dirt->m_ID;
					continue;
				}

				if (k1 == 3)
				{
					tiles[l1] = Tile::grass->m_ID;
					continue;
				}

				tiles[l1] = TILE_AIR;
			}
		}
	}
}

void FlatLevelSource::postProcess(ChunkSource* src, const ChunkPos& pos)
{

}

int FlatLevelSource::tick()
{
	return 0;
}

bool FlatLevelSource::shouldSave()
{
	return true;
}

bool FlatLevelSource::hasChunk(const ChunkPos& pos)
{
	return true;
}

LevelChunk* FlatLevelSource::create(const ChunkPos& pos)
{
	return getChunk(pos);
}

std::string FlatLevelSource::gatherStats()
{
	return "FlatLevelSource";
}
