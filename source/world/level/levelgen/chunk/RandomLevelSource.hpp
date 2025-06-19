/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

// @NOTE: This class appears to reference a mythical "std::hashtable", but I couldn't find any traces of it.
// I doubt they used C++11 (since it came out in 2011), but this is weird...
// We'll use std::unordered_map instead.

#include <unordered_map>
#include "ChunkSource.hpp"
#include "common/Utils.hpp"
#include "world/level/levelgen/synth/PerlinNoise.hpp"
#include "world/level/levelgen/biome/BiomeSource.hpp"
#include "world/level/levelgen/feature/Feature.hpp"
#include "world/level/levelgen/feature/LargeCaveFeature.hpp"
#include <cstdint>

class RandomLevelSource : public ChunkSource
{
public:
	RandomLevelSource(Level*, int64_t seed, int);
	int tick() override;
	bool shouldSave() override;
	bool hasChunk(const ChunkPos& pos) override;
	LevelChunk* create(const ChunkPos& pos) override;
	LevelChunk* getChunk(const ChunkPos& pos) override;
	LevelChunk* getChunkDontCreate(const ChunkPos& pos) override;
	std::string gatherStats() override;
	void postProcess(ChunkSource*, const ChunkPos& pos) override;

	real* getHeights(real*, int, int, int, int, int, int);
	void prepareHeights(const ChunkPos& pos, TileID*, void*, real*);
	void buildSurfaces (const ChunkPos& pos, TileID*, Biome**);
	

public:
	bool field_4;
	LargeCaveFeature m_largeCaveFeature;
	std::unordered_map<uint64_t, LevelChunk*> m_chunks;
	real field_19F0;
	Random m_random;
	PerlinNoise m_perlinNoise1;
	PerlinNoise m_perlinNoise2;
	PerlinNoise m_perlinNoise3;
	PerlinNoise m_perlinNoise4;
	PerlinNoise m_perlinNoise5;
	PerlinNoise m_perlinNoise6;
	PerlinNoise m_perlinNoise7;
	PerlinNoise m_perlinNoise8;
	Level* m_pLevel;
	real* field_7280;
	real m_sandBuffer[256];
	real m_gravelBuffer[256];
	real m_depthBuffer[256];
	// @TODO
	real* m_pnr;
	real* m_ar;
	real* m_br;
	real* m_sr;
	real* m_dr;
	// @TODO


	static const real SNOW_CUTOFF;
	static const real SNOW_SCALE;
};

