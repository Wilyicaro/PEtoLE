/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "RandomLevelSource.hpp"
#include "world/level/Level.hpp"
#include "world/tile/SandTile.hpp"


const real RandomLevelSource::SNOW_CUTOFF = 0.5;
const real RandomLevelSource::SNOW_SCALE  = 0.3;

real g_timeSpentInPostProcessing = 0;

RandomLevelSource::RandomLevelSource(Level* level, int64_t seed, int x) :
	m_random(seed),
	m_perlinNoise1(&m_random, 16),
	m_perlinNoise2(&m_random, 16),
	m_perlinNoise3(&m_random, 8),
	m_perlinNoise4(&m_random, 4),
	m_perlinNoise5(&m_random, 4),
	m_perlinNoise6(&m_random, 10),
	m_perlinNoise7(&m_random, 16),
	m_perlinNoise8(&m_random, 8),
	m_pLevel(level)
{
	field_4 = false;
	field_19F0 = 1.0;
	field_7280 = nullptr;
	m_pnr = nullptr;
	m_ar = nullptr;
	m_br = nullptr;
	m_sr = nullptr;
	m_dr = nullptr;

	LOG_I("Generating world with seed: %d", seed);

	LOG_I("random.get : %d", m_random.nextInt() >> 1);
}



LevelChunk* RandomLevelSource::getChunk(const ChunkPos& pos)
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

	Biome** pBiomeBlock = m_pLevel->getBiomeSource()->getBiomeBlock(TilePos(pos, 0), 16, 16);
	prepareHeights(pos, pLevelData, nullptr, m_pLevel->getBiomeSource()->temperatures);
	buildSurfaces(pos, pLevelData, pBiomeBlock);
	m_largeCaveFeature.apply(this, m_pLevel, pos.x, pos.z, pLevelData, 0);
	pChunk->recalcHeightmap();

	return pChunk;
}

LevelChunk* RandomLevelSource::getChunkDontCreate(const ChunkPos& pos)
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

real* RandomLevelSource::getHeights(real* fptr, int a3, int a4, int a5, int a6, int a7, int a8)
{
	if (!fptr)
	{
		fptr = new real[a6 * a7 * a8];
	}

	real* bsf4 = m_pLevel->getBiomeSource()->temperatures;
	real* bsf8 = m_pLevel->getBiomeSource()->downfalls;

	constexpr real C_MAGIC_1 = 684.412;

	m_sr = m_perlinNoise6.getRegion(m_sr, a3, a5, a6, a8, 1.121, 1.121, 0.5);
	m_dr = m_perlinNoise7.getRegion(m_dr, a3, a5, a6, a8, 200.0, 200.0, 0.5);
	m_pnr = m_perlinNoise3.getRegion(m_pnr, real(a3), real(a4), real(a5), a6, a7, a8, C_MAGIC_1 / 80, C_MAGIC_1 / 160, C_MAGIC_1 / 80);
	m_ar = m_perlinNoise1.getRegion(m_ar, real(a3), real(a4), real(a5), a6, a7, a8, C_MAGIC_1, C_MAGIC_1, C_MAGIC_1);
	m_br = m_perlinNoise2.getRegion(m_br, real(a3), real(a4), real(a5), a6, a7, a8, C_MAGIC_1, C_MAGIC_1, C_MAGIC_1);


	int k1 = 0;
	int l1 = 0;
	int i2 = 16 / a6;
	for (int j2 = 0; j2 < a6; j2++)
	{
		int k2 = j2 * i2 + i2 / 2;
		for (int l2 = 0; l2 < a8; l2++)
		{
			int i3 = l2 * i2 + i2 / 2;
			real d2 = bsf4[k2 * 16 + i3];
			real d3 = bsf8[k2 * 16 + i3] * d2;
			real d4 = 1.0 - d3;
			d4 *= d4;
			d4 *= d4;
			d4 = 1.0 - d4;
			real d5 = (m_sr[l1] + 256.0) / 512.0;
			d5 *= d4;
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
				real d9 = (((real)j3 - d7) * 12.0) / d5;
				if (d9 < 0.0)
				{
					d9 *= 4.0;
				}
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

void RandomLevelSource::prepareHeights(const ChunkPos& pos, TileID* tiles, void* huh, real* noise)
{
	auto* heightMap = getHeights(field_7280, pos.x * 4, 0, pos.z * 4, 5, 17, 5);

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

							if (globalY < 64)
							{
								real temp = noise[(j * 4 + n) + (i * 4 + m) * 16];
								if (temp < 0.5 && globalY >= 63)
									tile = Tile::ice->m_ID;
								else
									tile = Tile::calmWater->m_ID;
							}

							if (density > 0.0)
								tile = Tile::stone->m_ID;

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


void RandomLevelSource::buildSurfaces(const ChunkPos& pos, TileID* tiles, Biome** biomes)
{
	m_perlinNoise4.getRegion(m_sandBuffer, real(pos.x) * 16.0, real(pos.z) * 16.0, 0.0, 16, 16, 1, 1.0 / 32.0, 1.0 / 32.0, 1.0);
	m_perlinNoise4.getRegion(m_gravelBuffer, real(pos.x) * 16.0, 109.0134, real(pos.z) * 16.0, 16, 1, 16, 1.0 / 32.0, 1.0, 1.0 / 32.0);
	m_perlinNoise5.getRegion(m_depthBuffer, real(pos.x) * 16.0, real(pos.z) * 16.0, 0.0f, 16, 16, 1, 1.0 / 16.0, 1.0 / 16.0, 1.0 / 16.0);

	// @NOTE: Again, extracted from Java Beta 1.6. Probably accurate
	constexpr int byte0 = 64;
	for (int k = 0; k < 16; k++)
	{
		for (int l = 0; l < 16; l++)
		{
			Biome* pBiome = biomes[k + l * 16];
			bool hasSurfaceNoise = m_sandBuffer[k + l * 16] + m_random.nextDouble() * 0.2 > 0.0;
			bool hasGravelNoise = m_gravelBuffer[k + l * 16] + m_random.nextDouble() * 0.2 > 3.0;
			int i1 = (int)(m_depthBuffer[k + l * 16] / 3.0 + 3.0 + m_random.nextDouble() * 0.25);
			int j1 = -1;

			TileID topBlock = pBiome->topBlock;
			TileID fillerBlock = pBiome->fillerBlock;

			for (int k1 = 127; k1 >= 0; k1--)
			{
				int l1 = (l * 16 + k) * 128 + k1;
				if (k1 <= int(0 + m_random.nextInt(5)))
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

				if (byte3 != Tile::stone->m_ID)
					continue;

				if (j1 == -1)
				{
					if (i1 <= 0)
					{
						topBlock = TILE_AIR;
						fillerBlock = Tile::stone->m_ID;
					}
					else if (k1 >= byte0 - 4 && k1 <= byte0 + 1)
					{
						topBlock = pBiome->topBlock;
						fillerBlock = pBiome->fillerBlock;
						if (hasGravelNoise) {
							topBlock = TILE_AIR;
							fillerBlock = Tile::gravel->m_ID;
						}
						if (hasSurfaceNoise) {
							topBlock = Tile::sand->m_ID;
							fillerBlock = Tile::sand->m_ID;
						}
					}

					if (k1 < byte0 && topBlock == TILE_AIR)
						topBlock = Tile::calmWater->m_ID;

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

				if (j1 == 0 && fillerBlock == Tile::sand->m_ID)
				{
					j1 = m_random.nextInt(4);
					fillerBlock = Tile::sandStone->m_ID;
				}
			}
		}
	}
}

void RandomLevelSource::postProcess(ChunkSource* src, const ChunkPos& pos)
{
	m_pLevel->m_bPostProcessing = true;
	SandTile::instaFall = true;
	
	TilePos tp = TilePos(pos, 0);

	//LOG_I("Post-Processing %d, %d", pos.x, pos.z);
	Biome* pBiome = m_pLevel->getBiomeSource()->getBiome(tp + 16);
	int64_t seed = m_pLevel->getSeed();

	m_random.setSeed(seed);
	int64_t x1 = 1 + 2 * (m_random.nextLong() / 2);
	int64_t x2 = 1 + 2 * (m_random.nextLong() / 2);
	m_random.setSeed(((int64_t(pos.x) * x1) + (int64_t(pos.z) * x2)) ^ seed);

	// @NOTE: I can't put the random calls _in_ the argument list - args are evaluated right to left I believe

	if (m_random.nextInt(4) == 0)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(128);
		int zo = m_random.nextInt(16);
		LakeFeature(Tile::calmWater->m_ID).place(m_pLevel, &m_random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}

	if (m_random.nextInt(8) == 0)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(m_random.nextInt(120) + 8);
		int zo = m_random.nextInt(16);
		if (yo < 64 || m_random.nextInt(10) == 0)
			LakeFeature(Tile::calmLava->m_ID).place(m_pLevel, &m_random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}

	
	for (int i = 0; i < 8; i++) 
	{
		int xo = m_random.nextInt(16) + 8;
		int yo = m_random.nextInt(128);
		int zo = m_random.nextInt(16) + 8;
		MonsterRoomFeature().place(m_pLevel, &m_random, TilePos(tp.x + xo, yo, tp.z + zo));
	}

	for (int i = 0; i < 10; i++)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(128);
		int zo = m_random.nextInt(16);
		ClayFeature(Tile::clay->m_ID, 32).place(m_pLevel, &m_random, TilePos(tp.x + xo, yo, tp.z + zo));
	}

	// Start of ore generation

	for (int i = 0; i < 20; i++)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(128);
		int zo = m_random.nextInt(16);
		OreFeature(Tile::dirt->m_ID, 32).place(m_pLevel, &m_random, TilePos(tp.x + xo, yo, tp.z + zo));
	}
	for (int i = 0; i < 10; i++)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(128);
		int zo = m_random.nextInt(16);
		OreFeature(Tile::gravel->m_ID, 32).place(m_pLevel, &m_random, TilePos(tp.x + xo, yo, tp.z + zo));
	}
	for (int i = 0; i < 20; i++)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(128);
		int zo = m_random.nextInt(16);
		OreFeature(Tile::coalOre->m_ID, 16).place(m_pLevel, &m_random, TilePos(tp.x + xo, yo, tp.z + zo));
	}
	for (int i = 0; i < 20; i++)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(64);
		int zo = m_random.nextInt(16);
		OreFeature(Tile::ironOre->m_ID, 8).place(m_pLevel, &m_random, TilePos(tp.x + xo, yo, tp.z + zo));
	}
	for (int i = 0; i < 2; i++)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(32);
		int zo = m_random.nextInt(16);
		OreFeature(Tile::goldOre->m_ID, 8).place(m_pLevel, &m_random, TilePos(tp.x + xo, yo, tp.z + zo));
	}
	for (int i = 0; i < 8; i++)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(16);
		int zo = m_random.nextInt(16);
		OreFeature(Tile::redstoneOre->m_ID, 7).place(m_pLevel, &m_random, TilePos(tp.x + xo, yo, tp.z + zo));
	}
	for (int i = 0; i < 1; i++)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(16);
		int zo = m_random.nextInt(16);
		OreFeature(Tile::diamondOre->m_ID, 7).place(m_pLevel, &m_random, TilePos(tp.x + xo, yo, tp.z + zo));
	}
	for (int i = 0; i < 1; i++)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(16) + m_random.nextInt(16);
		int zo = m_random.nextInt(16);
		OreFeature(Tile::lapisOre->m_ID, 6).place(m_pLevel, &m_random, TilePos(tp.x + xo, yo, tp.z + zo));
	}

	// End of ore generation
	// Start of tree generation

	real t1 = m_perlinNoise8.getValue(real(tp.x) * 0.5, real(tp.z) * 0.5);
	double t2 = m_random.nextDouble();
	int t3 = int((4.0 + t2 * 4.0 + t1 / 8) / 3.0);

	int treeCount = 0;
	if (m_random.nextInt(10) == 0)
		treeCount++;

	if (pBiome == Biome::forest)
		treeCount += t3 + 5;

	if (pBiome == Biome::rainForest)
		treeCount += t3 + 5;

	if (pBiome == Biome::seasonalForest)
		treeCount += t3 + 2;

	if (pBiome == Biome::taiga)
		treeCount += t3 + 5;

	if (pBiome == Biome::desert)
		treeCount -= 20;

	if (pBiome == Biome::tundra)
		treeCount -= 20;

	if (pBiome == Biome::plains)
		treeCount -= 20;

	for (int i = 0; i < treeCount; i++)
	{
		TilePos rng;
		rng.x = m_random.nextInt(16) + tp.x + 8;
		rng.z = m_random.nextInt(16) + tp.z + 8;
		rng.y = m_pLevel->getHeightmap(rng);

		Feature* pTreeFeature = pBiome->getTreeFeature(&m_random);
		if (pTreeFeature)
		{
			pTreeFeature->init(1.0, 1.0, 1.0);
			pTreeFeature->place(m_pLevel, &m_random, rng);
			delete pTreeFeature;
		}
	}

	int flowerCount = 0;

	if (pBiome == Biome::forest)
		flowerCount = 2;

	if (pBiome == Biome::seasonalForest)
		flowerCount = 4;

	if (pBiome == Biome::taiga)
		flowerCount = 2;

	if (pBiome == Biome::plains)
		flowerCount = 3;

	for (int i = 0; i < flowerCount; i++)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(128);
		int zo = m_random.nextInt(16);
		FlowerFeature(Tile::flower->m_ID).place(m_pLevel, &m_random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}

	int vegetationCount = 0;

	if (pBiome == Biome::forest)
		vegetationCount = 2;

	if (pBiome == Biome::rainForest)
		vegetationCount = 10;

	if (pBiome == Biome::seasonalForest)
		vegetationCount = 2;

	if (pBiome == Biome::taiga)
		vegetationCount = 1;

	if (pBiome == Biome::plains)
		vegetationCount = 10;

	for (int i = 0; i < vegetationCount; i++)
	{
		int data = 1;

		if (pBiome == Biome::rainForest && m_random.nextInt(3) != 0) {
			data = 2;
		}
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(128);
		int zo = m_random.nextInt(16);
		VegetationFeature(Tile::tallGrass->m_ID, data).place(m_pLevel, &m_random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}

	vegetationCount = 0;

	if (pBiome == Biome::desert)
		vegetationCount = 2;

	for (int i = 0; i < vegetationCount; i++)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(128);
		int zo = m_random.nextInt(16);
		VegetationFeature(Tile::deadBush->m_ID, 0, 4).place(m_pLevel, &m_random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}

	if (m_random.nextInt(2) == 0)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(128);
		int zo = m_random.nextInt(16);
		FlowerFeature(Tile::rose->m_ID).place(m_pLevel, &m_random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}

	if (m_random.nextInt(4) == 0)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(128);
		int zo = m_random.nextInt(16);
		FlowerFeature(Tile::mushroom1->m_ID).place(m_pLevel, &m_random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}

	if (m_random.nextInt(8) == 0)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(128);
		int zo = m_random.nextInt(16);
		FlowerFeature(Tile::mushroom2->m_ID).place(m_pLevel, &m_random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}

	for (int i = 0; i < 10; i++)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(128);
		int zo = m_random.nextInt(16);
		ReedsFeature().place(m_pLevel, &m_random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}

	if (m_random.nextInt(32) == 0)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(128);
		int zo = m_random.nextInt(16);
		PumpkinFeature().place(m_pLevel, &m_random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}

	vegetationCount = 0;

	if (pBiome == Biome::desert)
		vegetationCount += 10;

	for (int i = 0; i < vegetationCount; i++)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(128);
		int zo = m_random.nextInt(16);
		CactusFeature().place(m_pLevel, &m_random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}

	for (int i = 0; i < 50; i++)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(m_random.nextInt(120) + 8);
		int zo = m_random.nextInt(16);
		SpringFeature(Tile::water->m_ID).place(m_pLevel, &m_random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}

	for (int i = 0; i < 20; i++)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(m_random.nextInt(m_random.nextInt(112) + 8) + 8);
		int zo = m_random.nextInt(16);
		SpringFeature(Tile::lava->m_ID).place(m_pLevel, &m_random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}

	real* tempBlock = m_pLevel->getBiomeSource()->getTemperatureBlock(tp.x + 8, tp.z + 8, 16, 16);
	for (int j19 = tp.x + 8; j19 < tp.x + 8 + 16; j19++)
	{
		for (int j22 = tp.z + 8; j22 < tp.z + 8 + 16; j22++)
		{
			int i24 = j19 - (tp.x + 8);
			int j25 = j22 - (tp.z + 8);

			int tsb = m_pLevel->getTopSolidBlock(TilePos(j19, 0, j22));
			
			if (SNOW_CUTOFF >= (tempBlock[i24 * 16 + j25] - SNOW_SCALE * (real(tsb - 64) / 64.0)))
			{
				if (tsb > 0 && tsb < C_MAX_Y && m_pLevel->isEmptyTile(TilePos(j19, tsb, j22)))
				{
					TilePos below = TilePos(j19, tsb - 1, j22);
					auto t = m_pLevel->getTile(below);
					if (m_pLevel->getMaterial(below)->blocksMotion() && m_pLevel->getMaterial(below) != Material::ice)
					{
						m_pLevel->setTile(TilePos(j19, tsb, j22), Tile::topSnow->m_ID);
					}
				}
			}
		}
	}

	SandTile::instaFall = false;
	m_pLevel->m_bPostProcessing = false;
}

int RandomLevelSource::tick()
{
	return 0;
}

bool RandomLevelSource::shouldSave()
{
	return true;
}

bool RandomLevelSource::hasChunk(const ChunkPos& pos)
{
	return true;
}

LevelChunk* RandomLevelSource::create(const ChunkPos& pos)
{
	return getChunk(pos);
}

std::string RandomLevelSource::gatherStats()
{
	return "RandomLevelSource";
}
