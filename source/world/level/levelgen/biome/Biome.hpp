/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "common/Utils.hpp"
#include "world/level/levelgen/synth/PerlinNoise.hpp"
#include "world/level/levelgen/feature/Feature.hpp"
#include <world/entity/MobCategory.hpp>

class EntityType;

class Biome
{
public: // Static Functions
	static Biome* getBiome(float hum, float temp);
	static void initBiomes();

public: // Virtual functions
	virtual float adjustDepth(float depth);
	virtual float adjustScale(float scale);
	virtual int getSkyColor(float x);
	virtual Feature* getTreeFeature(Random*);

public: // Instance Functions
	Biome();
	virtual ~Biome();
	Biome* setColor(int color);
	Biome* setLeafColor(int color);
	Biome* setName(const std::string& name);
	Biome* setSnowCovered();
	Biome* setNoRain();
	bool canOnlyRain() const
	{
		return !hasSnow && hasRain;
	}

	void setMobs(const MobCategory* category, std::vector<EntityType*> vector);

	void addMobs(const MobCategory* category, std::vector<EntityType*> vector);

	const std::vector<EntityType*>& getMobs(const MobCategory* category);

public: // Static Variables
	static Biome
		* rainForest,
		* swampland,
		* seasonalForest,
		* forest,
		* savanna,
		* shrubland,
		* taiga,
		* desert,
		* plains,
		* iceDesert,
		* tundra;
	static Biome* map[4096];

private: // Private Functions
	static Biome* _getBiome(float hum, float temp);
	static void recalc();
	std::unordered_map<const MobCategory*, std::vector<EntityType*>> m_mobSpawns;

public: // Instance Variables
	std::string m_name;
	int m_Color;
	TileID topBlock;
	TileID fillerBlock;
	int m_LeafColor;
	bool hasSnow;
	bool hasRain = true;
};

class RainforestBiome : public Biome
{
public:
	Feature* getTreeFeature(Random*) override;
};

class SwampBiome : public Biome
{
};

class ForestBiome : public Biome
{
public:
	Feature* getTreeFeature(Random*) override;
};

class TaigaBiome : public Biome
{
public:
	Feature* getTreeFeature(Random*) override;
};

class FlatBiome : public Biome
{
};
