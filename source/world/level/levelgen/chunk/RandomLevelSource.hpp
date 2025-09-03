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
	RandomLevelSource(Level*, int64_t seed);
	int tick() override;
	bool shouldSave() override;
	bool hasChunk(const ChunkPos& pos) override;
	LevelChunk* create(const ChunkPos& pos) override;
	LevelChunk* getChunk(const ChunkPos& pos) override;
	LevelChunk* getChunkDontCreate(const ChunkPos& pos) override;
	std::string gatherStats() override;
	void postProcess(ChunkSource*, const ChunkPos& pos) override;

	//@NOTE: Adapted LCE methods, thanks to GRAnimated for allowing to use them
	float getHeightFalloff(int chunkX, int chunkZ, int& distance, const DimensionLimit&);
	int distanceToEdge(float a, int x, int z, const DimensionLimit&);

	const std::vector<real>& getHeights(std::vector<real>&, int, int, int, int, int, int);
	void prepareHeights(const ChunkPos& pos, TileID*, const std::vector<real>&);
	void buildSurfaces(const ChunkPos& pos, TileID*, const std::vector<Biome*>);
	

public:
	LargeCaveFeature m_largeCaveFeature;
	std::unordered_map<uint64_t, LevelChunk*> m_chunks;
	Random m_random;
	PerlinNoise m_lperlinNoise1;
	PerlinNoise m_lperlinNoise2;
	PerlinNoise m_perlinNoise1;
	PerlinNoise m_perlinNoise2;
	PerlinNoise m_perlinNoise3;
	PerlinNoise m_scaleNoise;
	PerlinNoise m_depthNoise;
	PerlinNoise m_forestNoise;
	Level* m_pLevel;
	std::vector<real> m_buffer;
	std::vector<real> m_sandBuffer;
	std::vector<real> m_gravelBuffer;
	std::vector<real> m_depthBuffer;
	std::vector<real> m_pnr;
	std::vector<real> m_ar;
	std::vector<real> m_br;
	std::vector<real> m_sr;
	std::vector<real> m_dr;


	static const real SNOW_CUTOFF;
	static const real SNOW_SCALE;
};

