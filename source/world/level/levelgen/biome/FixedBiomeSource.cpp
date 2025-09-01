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
}

real FixedBiomeSource::getTemperature(int x, int y) {
	return m_temperature;
}

const std::vector<Biome*>& FixedBiomeSource::getBiomeBlock(std::vector<Biome*>& pBiomes, const TilePos& pos, int c, int d)
{
	int size = c * d;
	if (pBiomes.size() < size)
		pBiomes.assign(size, m_pBiome);
	if (m_temperatures.size() < size)
		m_temperatures.assign(size, m_temperature);
	if (m_downfalls.size() < size)
		m_downfalls.assign(size, m_downfall);
	return pBiomes;
}

const std::vector<real>& FixedBiomeSource::getTemperatureBlock(int a, int b, int c, int d)
{
	int size = c * d;
	if (m_temperatures.size() < size)
		m_temperatures.assign(size, m_temperature);
	return m_temperatures;
}