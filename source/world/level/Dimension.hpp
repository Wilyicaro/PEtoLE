/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "world/phys/Vec3T.hpp"
#include "world/level/levelgen/biome/BiomeSource.hpp"
#include "world/level/levelgen/chunk/ChunkSource.hpp"
#include "storage/ChunkStorage.hpp"
#include "Level.hpp"

class Level; // if included from Level.hpp

class Dimension
{
public:
	Dimension();
	virtual ~Dimension();
	static Dimension* getNew(int type);

	virtual Vec3f getFogColor(float, float);
	virtual void init();
	virtual bool mayRespawn();
	virtual bool isValidSpawn(const TilePos& pos);
	virtual float getCloudHeight();
	virtual float* getSunriseColor(float, float);
	virtual float getTimeOfDay(int64_t, float);
	virtual void init(Level* pLevel);
	virtual void updateLightRamp();


	virtual ChunkSource* createRandomLevelSource();
	virtual ChunkStorage* createStorage();

public:
	Level* m_pLevel;
	BiomeSource* m_pBiomeSource;
	bool m_bFoggy;
	bool m_bUltraWarm;
	bool m_bHasCeiling;
	float m_brightnessRamp[16];
	int m_ID;
	float m_sunriseColor[4];
};

