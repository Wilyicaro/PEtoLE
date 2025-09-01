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

class FlatLevelSource : public ChunkSource
{
public:
	FlatLevelSource(Level*, int64_t seed);
	int tick() override;
	bool shouldSave() override;
	bool hasChunk(const ChunkPos& pos) override;
	LevelChunk* create(const ChunkPos& pos) override;
	LevelChunk* getChunk(const ChunkPos& pos) override;
	LevelChunk* getChunkDontCreate(const ChunkPos& pos) override;
	std::string gatherStats() override;
	void postProcess(ChunkSource*, const ChunkPos& pos) override;

	void buildSurfaces(const ChunkPos& pos, TileID*);
	

public:
	Random m_random;
	Level* m_pLevel;
};

