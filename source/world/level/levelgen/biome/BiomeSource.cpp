/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "BiomeSource.hpp"
#include "world/level/Level.hpp"

BiomeSource* BiomeSource::init()
{
	temperatures = nullptr;
	downfalls = nullptr;
	noises = nullptr;
	field_10 = 0;
	field_14 = 0;
	field_18 = 0;
	field_1C = 0;
	biomes = new Biome * [256];
	m_pPerlinNoise[0] = nullptr;
	m_pPerlinNoise[1] = nullptr;
	m_pPerlinNoise[2] = nullptr;

	return this;
}

BiomeSource::BiomeSource(Level* pLevel) :
	m_Random1(pLevel->getSeed() * 9871L),
	m_Random2(pLevel->getSeed() * 39811L),
	m_Random3(pLevel->getSeed() * 543321L)
{
	init();
	m_pPerlinNoise[0] = new PerlinNoise(&m_Random1, 4);
	m_pPerlinNoise[1] = new PerlinNoise(&m_Random2, 4);
	m_pPerlinNoise[2] = new PerlinNoise(&m_Random3, 2);
}

real BiomeSource::getTemperature(int x, int y) {
	temperatures = m_pPerlinNoise[0]->getBiomeRegion(temperatures, x, y, 1, 1, 0.025, 0.025, 0.5);
	return temperatures[0];
}

Biome* BiomeSource::getBiome(const ChunkPos& pos)
{
	return getBiome(TilePos(pos, 0));
}

Biome* BiomeSource::getBiome(const TilePos& pos)
{
	return *getBiomeBlock(pos, 1, 1);
}

Biome** BiomeSource::getBiomeBlock(const TilePos& pos, int c, int d)
{
	return getBiomeBlock(biomes, pos, c, d);
}

Biome** BiomeSource::getBiomeBlock(Biome** pBiomes, const TilePos& pos, int c, int d)
{
	temperatures = m_pPerlinNoise[0]->getBiomeRegion(temperatures, pos.x, pos.z, c, c, 0.025, 0.025, 0.25);
	downfalls = m_pPerlinNoise[1]->getBiomeRegion(downfalls, pos.x, pos.z, c, c, 0.05, 0.05, 1 / 3.0);
	noises = m_pPerlinNoise[2]->getBiomeRegion(noises, pos.x, pos.z, c, c, 0.25, 0.25, 0.5882352941176471);

	int index = 0;

	for (int i = 0; i < c; i++)
	{
		for (int j = 0; j < d; j++)
		{
			real d = noises[index] * 1.1 + 0.5;
			real d1 = 0.01f;
			real d2 = 1.0f - d1;
			real d3 = (temperatures[index] * 0.15 + 0.7) * d2 + d * d1;
			d1 = 0.002;
			d2 = 1.0 - d1;
			real d4 = (downfalls[index] * 0.15 + 0.5) * d2 + d * d1;
			d3 = 1.0 - (1.0 - d3) * (1.0 - d3);

			if (d3 < 0.0) d3 = 0.0;
			if (d4 < 0.0) d4 = 0.0;
			if (d3 > 1.0) d3 = 1.0;
			if (d4 > 1.0) d4 = 1.0;

			temperatures[index] = d3;
			downfalls[index] = d4;
			biomes[index++] = Biome::getBiome(d3, d4);
		}
	}

	return biomes;
}

real* BiomeSource::getTemperatureBlock(int a, int b, int c, int d)
{
	temperatures = m_pPerlinNoise[0]->getBiomeRegion(temperatures, a, b, c, d, 0.025, 0.025, 0.25);
	noises = m_pPerlinNoise[2]->getBiomeRegion(noises, a, b, c, d, 0.25, 0.25, 0.5882352941176471);

	int index = 0;

	for (int i = 0; i < c; i++)
	{
		for (int j = 0; j < d; j++)
		{
			real d = noises[index] * 1.1 + 0.5;
			real d1 = 0.01;
			real d2 = 1.0 - d1;
			real d3 = (temperatures[index] * 0.15 + 0.7) * d2 + d * d1;
			d3 = 1.0 - (1.0 - d3) * (1.0f - d3);
			if (d3 < 0.0)
				d3 = 0.0;
			if (d3 > 1.0)
				d3 = 1.0;

			temperatures[index] = d3;
			index++;
		}
	}

	return temperatures;
}

BiomeSource::~BiomeSource()
{
	for (int i = 0; i < 3; i++)
		if (m_pPerlinNoise[i])
			delete m_pPerlinNoise[i];

	if (temperatures)
		delete[] temperatures;

	if (downfalls)
		delete[] downfalls;

	if (noises)
		delete[] noises;

	if (biomes)
		delete[] biomes;
}
