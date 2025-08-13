/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "BiomeSource.hpp"

class FixedBiomeSource : public BiomeSource
{
public:
	FixedBiomeSource(Biome* biome, real temperature, real downfall);
	real getTemperature(int x, int y);

	virtual const std::vector<Biome*>& getBiomeBlock(std::vector<Biome*>&, const TilePos& pos, int, int) override;
	virtual const std::vector<real>& getTemperatureBlock(int, int, int, int) override;

public:
	Biome* m_pBiome;
	real m_temperature;
	real m_downfall;
	
};

