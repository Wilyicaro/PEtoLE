/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "FixedBiomeSource.hpp"
#include "world/level/Level.hpp"

FixedBiomeSource::FixedBiomeSource(Biome* biome, real temperature, real downfall) : BiomeSource(), m_pBiome(biome), m_temperature(temperature), m_downfall(downfall)
{
	m_biomes.assign(256, biome);
	m_temperatures.assign(256, temperature);
	m_downfalls.assign(256, downfall);
}

real FixedBiomeSource::getTemperature(int x, int y) {
	return m_temperature;
}

const std::vector<Biome*>& FixedBiomeSource::getBiomeBlock(std::vector<Biome*>& pBiomes, const TilePos& pos, int c, int d)
{
	return m_biomes;
}

const std::vector<real>& FixedBiomeSource::getTemperatureBlock(int a, int b, int c, int d)
{

	return m_temperatures;
}