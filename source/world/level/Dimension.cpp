/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Dimension.hpp"
#include "world/level/levelgen/chunk/TestChunkSource.hpp"
#include "world/level/levelgen/chunk/RandomLevelSource.hpp"
#include "world/level/levelgen/chunk/ChunkCache.hpp"

Dimension* Dimension::getNew(int type)
{
	switch (type)
	{
	case 0:
		return new Dimension;
	/*case -1:
		return new HellDimension;*/
	default: // type not supported
		return nullptr;
	}
}

Vec3f Dimension::getFogColor(float a, float b)
{
	float var3 = Mth::cos(a * (float)M_PI * 2.0F) * 2.0F + 0.5F;
	if (var3 < 0.0F) {
		var3 = 0.0F;
	}

	if (var3 > 1.0F) {
		var3 = 1.0F;
	}

	Vec3f fog(192.0F / 255.0F, 216.0F / 255.0F, 1.0f);

	fog.x *= var3 * 0.94F + 0.06F;
	fog.y *= var3 * 0.94F + 0.06F;
	fog.z *= var3 * 0.91F + 0.09F;

	return fog;
}

float* Dimension::getSunriseColor(float a, float b)
{
	float radial = 0.4f;
	float dot = Mth::cos(a * M_PI * 2.0f);
	float center = -0.0f;

	if (dot >= center - radial && dot <= center + radial)
	{
		float norm = (dot - center) / radial * 0.5f + 0.5f;
		float alpha = 1.0f - (1.0f - Mth::sin(norm * M_PI)) * 0.99f;
		
		m_sunriseColor[0] = norm * 0.3f + 0.7f;
		m_sunriseColor[1] = norm * norm * 0.7f + 0.2f;
		m_sunriseColor[2] = 0.2f;
		m_sunriseColor[3] = alpha * alpha;
		return m_sunriseColor;
	}
	return nullptr;
}

float Dimension::getTimeOfDay(int32_t l, float f)
{
	int i = int(l % 24000);
	float f1 = (float(i) + f) / 24000.0f - 0.25f;
	if (f1 < 0.0f)
		f1 += 1.0f;
	if (f1 > 1.0f)
		f1 -= 1.0f;

	// @NOTE: At this point, if the day/night cycle isn't running,
	// f1's value should be -0.25

	float f2 = f1;
	f1 = 1.0f - (Mth::cos(float(M_PI) * f1) + 1.0f) * 0.5f;
	f1 = f2 + (f1 - f2) / 3.0f;
	return f1;
}

void Dimension::updateLightRamp()
{
	for (int i = 0; i < 16; i++)
	{
		float f1 = 1.0f - float(i) / 15.0f;
		m_brightnessRamp[i] = ((1.0f - f1) / (f1 * 3.0f + 1.0f)) * (1.0f - 0.05f) + 0.05f;
	}
}

void Dimension::init()
{
	m_pBiomeSource = new BiomeSource(m_pLevel);
}

void Dimension::init(Level* pLevel)
{
	m_pLevel = pLevel;
	init();
	updateLightRamp();
}

Dimension::Dimension()
{
	m_pLevel = nullptr;
	m_pBiomeSource = nullptr;
	m_bFoggy = false;
	m_bUltraWarm = false;
	m_bHasCeiling = false;
	m_ID = 0;
}

Dimension::~Dimension()
{
	if (m_pBiomeSource)
		delete m_pBiomeSource;
}

bool Dimension::mayRespawn()
{
	return true;
}

ChunkSource* Dimension::createRandomLevelSource()
{
#ifdef MOD_USE_FLAT_WORLD
	return new TestChunkSource(m_pLevel);
#else
	return new RandomLevelSource(m_pLevel, m_pLevel->getSeed(), m_pLevel->getLevelData()->getGeneratorVersion());
#endif
}

bool Dimension::isValidSpawn(const TilePos& pos)
{
	TileID tile = m_pLevel->getTopTile(pos);
	if (tile == 0)
		return false;

	return Tile::tiles[tile]->isSolidRender();
}

float Dimension::getCloudHeight()
{
	return 108.0f;
}
