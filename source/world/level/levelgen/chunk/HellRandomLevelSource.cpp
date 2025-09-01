/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "HellRandomLevelSource.hpp"
#include "world/level/Level.hpp"
#include "world/tile/SandTile.hpp"

HellRandomLevelSource::HellRandomLevelSource(Level* level, int64_t seed) :
	m_random(seed),
	m_lperlinNoise1(&m_random, 16),
	m_lperlinNoise2(&m_random, 16),
	m_perlinNoise1(&m_random, 8),
	m_perlinNoise2(&m_random, 4),
	m_perlinNoise3(&m_random, 4),
	m_scaleNoise(&m_random, 10),
	m_depthNoise(&m_random, 16),
	m_pLevel(level),
	m_largeCaveFeature(true)
{
	m_sandBuffer.reserve(256);
	m_gravelBuffer.reserve(256);
	m_depthBuffer.reserve(256);


	LOG_I("Generating world with seed: %d", seed);

	LOG_I("random.get : %d", m_random.nextInt() >> 1);
}



LevelChunk* HellRandomLevelSource::getChunk(const ChunkPos& pos)
{
	uint64_t key = pos.key();
	LevelChunk* cachedChunk = m_chunks[key];
	if (cachedChunk)
		return cachedChunk;

	// have to generate the chunk
	m_random.setSeed(341873128712L * pos.x + 132897987541L * pos.z);

	TileID* pLevelData = new TileID[32768];

	LevelChunk* pChunk = new LevelChunk(m_pLevel, pLevelData, pos);
	m_chunks[key] = pChunk;

	prepareHeights(pos, pLevelData);
	buildSurfaces(pos, pLevelData);
	m_largeCaveFeature.apply(this, m_pLevel, pos.x, pos.z, pLevelData, 0);
	pChunk->recalcHeightmap();

	return pChunk;
}

LevelChunk* HellRandomLevelSource::getChunkDontCreate(const ChunkPos& pos)
{
	uint64_t key = pos.key();
	LevelChunk* cachedChunk = m_chunks[key];
	if (cachedChunk)
		return cachedChunk;

	// have to create the chunk. Create an empty one

	TileID* pLevelData = new TileID[32768];
	memset (pLevelData, 0, sizeof *pLevelData * 32768);

	LevelChunk* pChunk = new LevelChunk(m_pLevel, pLevelData, pos);
	m_chunks[key] = pChunk;

	return pChunk;
}

const std::vector<real>& HellRandomLevelSource::getHeights(std::vector<real>& fptr, int a3, int a4, int a5, int a6, int a7, int a8)
{
	fptr.resize(a6 * a7 * a8);

	constexpr real C_MAGIC_1 = 684.412;
	constexpr real C_MAGIC_2 = 2053.236;

	m_sr = m_scaleNoise.getRegion(m_sr, a3, a5, a6, a8, 1.0, 0.0, 1.0);
	m_dr = m_depthNoise.getRegion(m_dr, a3, a5, a6, a8, 100.0, 0.0, 100.0);
	m_pnr = m_perlinNoise1.getRegion(m_pnr, real(a3), real(a4), real(a5), a6, a7, a8, C_MAGIC_1 / 80, C_MAGIC_2 / 60, C_MAGIC_1 / 80);
	m_ar = m_lperlinNoise1.getRegion(m_ar, real(a3), real(a4), real(a5), a6, a7, a8, C_MAGIC_1, C_MAGIC_2, C_MAGIC_1);
	m_br = m_lperlinNoise2.getRegion(m_br, real(a3), real(a4), real(a5), a6, a7, a8, C_MAGIC_1, C_MAGIC_2, C_MAGIC_1);

	real* var14 = new real[a7];

	for (int var15 = 0; var15 < a7; ++var15) {
		var14[var15] = Mth::cos(var15 * M_PI * 6.0 / a7) * 2.0;
		real var16 = var15;
		if (var15 > a7 / 2) {
			var16 = (a7 - 1 - var15);
		}

		if (var16 < 4.0) {
			var16 = 4.0 - var16;
			var14[var15] -= var16 * var16 * var16 * 10.0;
		}
	}

	int k1 = 0;
	int l1 = 0;
	int i2 = 16 / a6;
	for (int j2 = 0; j2 < a6; j2++)
	{
		int k2 = j2 * i2 + i2 / 2;
		for (int l2 = 0; l2 < a8; l2++)
		{
			int i3 = l2 * i2 + i2 / 2;
			real d5 = (m_sr[l1] + 256.0) / 512.0;
			if (d5 > 1.0)
				d5 = 1.0;
			real d6 = m_dr[l1] / 8000.0;
			if (d6 < 0.0)
			{
				d6 = -d6 * 0.3;
			}
			d6 = d6 * 3.0 - 2.0;
			if (d6 < 0.0)
			{
				d6 /= 2.0;
				if (d6 < -1.0)
					d6 = -1.0;
				d6 /= 1.4;
				d6 /= 2.0;
				d5 = 0.0;
			}
			else
			{
				if (d6 > 1.0)
					d6 = 1.0;
				d6 /= 8.0;
			}
			if (d5 < 0.0)
				d5 = 0.0;
			d5 += 0.5;
			d6 = d6 * (real)a7 / 16.0;
			real d7 = (real)a7 / 2.0 + d6 * 4.0;
			l1++;
			for (int j3 = 0; j3 < a7; j3++)
			{
				real d8 = 0.0;
				real d9 = var14[j3];
				real d10 = m_ar[k1] / 512.0;
				real d11 = m_br[k1] / 512.0;
				real d12 = (m_pnr[k1] / 10.0 + 1.0) / 2.0;
				if (d12 < 0.0)
					d8 = d10;
				else if (d12 > 1.0)
					d8 = d11;
				else
					d8 = d10 + (d11 - d10) * d12;
				d8 -= d9;
				if (j3 > a7 - 4)
				{
					real d13 = (real)(j3 - (a7 - 4)) / 3.0;
					d8 = d8 * (1.0 - d13) + -10.0 * d13;
				}
				fptr[k1] = d8;
				k1++;
			}

		}

	}

	return fptr;
}

void HellRandomLevelSource::prepareHeights(const ChunkPos& pos, TileID* tiles)
{
	auto& heightMap = getHeights(m_buffer, pos.x * 4, 0, pos.z * 4, 5, 17, 5);

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			for (int k = 0; k < 16; ++k)
			{
				real d1 = heightMap[(j + i * 5) * 17 + k];
				real d2 = heightMap[(j + 1 + i * 5) * 17 + k];
				real d3 = heightMap[(j + (i + 1) * 5) * 17 + k];
				real d4 = heightMap[(j + 1 + (i + 1) * 5) * 17 + k];
				real d5 = (heightMap[(j + i * 5) * 17 + k + 1] - d1) * 0.125;
				real d6 = (heightMap[(j + 1 + i * 5) * 17 + k + 1] - d2) * 0.125;
				real d7 = (heightMap[(j + (i + 1) * 5) * 17 + k + 1] - d3) * 0.125;
				real d8 = (heightMap[(j + 1 + (i + 1) * 5) * 17 + k + 1] - d4) * 0.125;

				for (int l = 0; l < 8; ++l)
				{
					real d9 = d1;
					real d10 = d2;
					real d11 = (d3 - d1) * 0.25;
					real d12 = (d4 - d2) * 0.25;

					for (int m = 0; m < 4; ++m)
					{
						int x = i * 4 + m;
						int z = j * 4;
						int y = k * 8 + l;
						int index = (x * 16 + z) * 128 + y;

						real density = d9;
						real d13 = (d10 - d9) * 0.25;

						for (int n = 0; n < 4; ++n)
						{
							TileID tile = TILE_AIR;
							int globalY = y;

							if (globalY < 32)
							{
								tile = Tile::calmLava->m_ID;
							}

							if (density > 0.0)
								tile = Tile::netherrack->m_ID;

							tiles[index] = tile;
							index += 128;
							density += d13;
						}
						d9 += d11;
						d10 += d12;
					}

					d1 += d5;
					d2 += d6;
					d3 += d7;
					d4 += d8;
				}
			}
		}
	}
}


void HellRandomLevelSource::buildSurfaces(const ChunkPos& pos, TileID* tiles)
{
	m_perlinNoise2.getRegion(m_sandBuffer, real(pos.x) * 16.0, real(pos.z) * 16.0, 0.0, 16, 16, 1, 1.0 / 32.0, 1.0 / 32.0, 1.0);
	m_perlinNoise2.getRegion(m_gravelBuffer, real(pos.x) * 16.0, 109.0134, real(pos.z) * 16.0, 16, 1, 16, 1.0 / 32.0, 1.0, 1.0 / 32.0);
	m_perlinNoise3.getRegion(m_depthBuffer, real(pos.x) * 16.0, real(pos.z) * 16.0, 0.0f, 16, 16, 1, 1.0 / 16.0, 1.0 / 16.0, 1.0 / 16.0);

	DimensionLimit limit = m_pLevel->getLevelData().getLimit(m_pLevel->m_pDimension->m_ID);

	constexpr int byte0 = 64;
	for (int k = 0; k < 16; k++)
	{
		for (int l = 0; l < 16; l++)
		{
			bool hasSurfaceNoise = m_sandBuffer[k + l * 16] + m_random.nextDouble() * 0.2 > 0.0;
			bool hasGravelNoise = m_gravelBuffer[k + l * 16] + m_random.nextDouble() * 0.2 > 3.0;
			int i1 = (int)(m_depthBuffer[k + l * 16] / 3.0 + 3.0 + m_random.nextDouble() * 0.25);
			int j1 = -1;

			TileID topBlock = Tile::netherrack->m_ID;
			TileID fillerBlock = Tile::netherrack->m_ID;

			for (int k1 = 127; k1 >= 0; k1--)
			{
				int l1 = (l * 16 + k) * 128 + k1;
				if (k1 >= 127 - m_random.nextInt(5) || k1 <= m_random.nextInt(5))
				{
					tiles[l1] = Tile::bedrock->m_ID;
					continue;
				}

				if (limit != DimensionLimit::ZERO && (
					pos.x * 16 + l <= limit.m_minPos.x * 16 + m_random.nextInt(5) ||
					pos.z * 16 + k <= limit.m_minPos.z * 16 + m_random.nextInt(5) ||
					pos.x * 16 + l >= limit.m_maxPos.x * 16 - 1 - m_random.nextInt(5) ||
					pos.z * 16 + k >= limit.m_maxPos.z * 16 - 1 - m_random.nextInt(5)))
				{
					tiles[l1] = Tile::bedrock->m_ID;
					continue;
				}

				TileID byte3 = tiles[l1];
				if (byte3 == 0)
				{
					j1 = -1;
					continue;
				}

				if (byte3 != Tile::netherrack->m_ID)
					continue;

				if (j1 == -1)
				{
					if (i1 <= 0)
					{
						topBlock = TILE_AIR;
						fillerBlock = Tile::netherrack->m_ID;
					}
					else if (k1 >= byte0 - 4 && k1 <= byte0 + 1)
					{
						topBlock = Tile::netherrack->m_ID;
						fillerBlock = Tile::netherrack->m_ID;
						if (hasGravelNoise) {
							topBlock = Tile::gravel->m_ID;
							fillerBlock = Tile::netherrack->m_ID;
						}
						if (hasSurfaceNoise) {
							topBlock = Tile::soulSand->m_ID;
							fillerBlock = Tile::soulSand->m_ID;
						}
					}

					if (k1 < byte0 && topBlock == TILE_AIR)
						topBlock = Tile::calmLava->m_ID;

					j1 = i1;

					if (k1 >= byte0 - 1)
						tiles[l1] = topBlock;
					else
						tiles[l1] = fillerBlock;

					continue;
				}

				if (j1 <= 0)
					continue;

				j1--;
				tiles[l1] = fillerBlock;
			}
		}
	}
}

void HellRandomLevelSource::postProcess(ChunkSource* src, const ChunkPos& pos)
{
	m_pLevel->m_bPostProcessing = true;
	SandTile::instaFall = true;
	
	TilePos tp = TilePos(pos, 0);

	TilePos fTp;
	for (int var6 = 0; var6 < 8; ++var6) {
		fTp.x = tp.x + m_random.nextInt(16) + 8;
		fTp.y = m_random.nextInt(120) + 4;
		fTp.z = tp.z + m_random.nextInt(16) + 8;
		(HellSpringFeature(Tile::lava->m_ID)).place(m_pLevel, &m_random, fTp);
	}

	for (int var7 = 0; var7 < m_random.nextInt(m_random.nextInt(10) + 1) + 1; ++var7) {
		fTp.x = tp.x + m_random.nextInt(16) + 8;
		fTp.y = m_random.nextInt(120) + 4;
		fTp.z = tp.z + m_random.nextInt(16) + 8;
		(HellFireFeature()).place(m_pLevel, &m_random, fTp);
	}

	for (int var7 = 0; var7 < m_random.nextInt(m_random.nextInt(10) + 1); ++var7) {
		fTp.x = tp.x + m_random.nextInt(16) + 8;
		fTp.y = m_random.nextInt(120) + 4;
		fTp.z = tp.z + m_random.nextInt(16) + 8;
		(LightGemFeature()).place(m_pLevel, &m_random, fTp);
	}

	for (int var7 = 0; var7 < 10; ++var7) {
		fTp.x = tp.x + m_random.nextInt(16) + 8;
		fTp.y = m_random.nextInt(128);
		fTp.z = tp.z + m_random.nextInt(16) + 8;
		//This should be the "HellPortalFeature", but it's simply a copy of LightGemFeature in the original code, certainly a testing feature
		(LightGemFeature()).place(m_pLevel, &m_random, fTp);
	}

	if (m_random.nextInt(1) == 0) {
		fTp.x = tp.x + m_random.nextInt(16) + 8;
		fTp.y = m_random.nextInt(128);
		fTp.z = tp.z + m_random.nextInt(16) + 8;
		(FlowerFeature(Tile::mushroom1->m_ID)).place(m_pLevel, &m_random, fTp);
	}

	if (m_random.nextInt(1) == 0) {
		fTp.x = tp.x + m_random.nextInt(16) + 8;
		fTp.y = m_random.nextInt(128);
		fTp.z = tp.z + m_random.nextInt(16) + 8;
		(FlowerFeature(Tile::mushroom2->m_ID)).place(m_pLevel, &m_random, fTp);
	}


	SandTile::instaFall = false;
	m_pLevel->m_bPostProcessing = false;
}

int HellRandomLevelSource::tick()
{
	return 0;
}

bool HellRandomLevelSource::shouldSave()
{
	return true;
}

bool HellRandomLevelSource::hasChunk(const ChunkPos& pos)
{
	return true;
}

LevelChunk* HellRandomLevelSource::create(const ChunkPos& pos)
{
	return getChunk(pos);
}

std::string HellRandomLevelSource::gatherStats()
{
	return "HellRandomLevelSource";
}
