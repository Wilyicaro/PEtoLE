/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "HellDimension.hpp"
#include "world/level/levelgen/chunk/HellRandomLevelSource.hpp"
#include "world/level/levelgen/biome/FixedBiomeSource.hpp"

Vec3f HellDimension::getFogColor(float a, float b)
{
	return Vec3f(0.2, 0.03, 0.03);
}

float HellDimension::getTimeOfDay(int64_t l, float f)
{
	return 0.5f;
}

void HellDimension::updateLightRamp()
{
	for (int i = 0; i < 16; i++)
	{
		float f1 = 1.0f - float(i) / 15.0f;
		m_brightnessRamp[i] = ((1.0f - f1) / (f1 * 3.0f + 1.0f)) * (1.0f - 0.1f) + 0.1f;
	}
}

void HellDimension::init()
{
	m_pBiomeSource = new FixedBiomeSource(Biome::hell, 1.0, 0.0);
	m_bFoggy = true;
	m_bUltraWarm = true;
	m_bHasCeiling = true;
	m_ID = -1;
}

void HellDimension::init(Level* pLevel)
{
	m_pLevel = pLevel;
	init();
	updateLightRamp();
}

bool HellDimension::mayRespawn()
{
	return false;
}

ChunkSource* HellDimension::createRandomLevelSource()
{
	return new HellRandomLevelSource(m_pLevel, m_pLevel->getSeed());
}

bool HellDimension::isValidSpawn(const TilePos& pos)
{
	int var3 = m_pLevel->getTopTile(pos);
	if (var3 == Tile::bedrock->m_ID)
		return false;
	else if (var3 == 0)
		return false;
	else
		return Tile::solid[var3];
}