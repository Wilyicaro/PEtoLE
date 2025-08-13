/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Biome.hpp"
#include "world/tile/Tile.hpp"

Biome
* Biome::rainForest,
* Biome::swampland,
* Biome::seasonalForest,
* Biome::forest,
* Biome::savanna,
* Biome::shrubland,
* Biome::taiga,
* Biome::desert,
* Biome::plains,
* Biome::iceDesert,
* Biome::tundra,
* Biome::hell;

Biome* Biome::map[4096];

Biome* Biome::_getBiome(float temp, float hum)
{
	float ht = hum * temp;
	return temp < 0.1F ? tundra : (ht < 0.2F ? (temp < 0.5F ? tundra : (temp < 0.95F ? savanna : desert)) : (ht > 0.5F && temp < 0.7F ? swampland : (temp < 0.5F ? taiga : (temp < 0.97F ? (ht < 0.35F ? shrubland : forest) : (ht < 0.45F ? plains : (ht < 0.9F ? seasonalForest : rainForest))))));
}

float Biome::adjustDepth(float f)
{
	return f;
}

float Biome::adjustScale(float f)
{
	return f;
}

int Biome::getSkyColor(float x)
{
	x /= 3.0F;
	if (x < -1.0F) {
		x = -1.0F;
	}

	if (x > 1.0F) {
		x = 1.0F;
	}


	return Mth::HSBtoRGB(0.62222224F - x * 0.05F, 0.5F + x * 0.1F, 1.0F);
}

Biome* Biome::getBiome(float hum, float temp)
{
	int i = int(63.0f * hum);
	int j = int(63.0f * temp);

	return map[i + j * 64];
}

void Biome::recalc()
{
	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			map[i + j * 64] = _getBiome(float(i) / 63, float(j) / 63);
		}
	}

	desert->topBlock = desert->fillerBlock = Tile::sand->m_ID;
	iceDesert->topBlock = iceDesert->fillerBlock = Tile::sand->m_ID;
}

Biome::Biome() : m_bHasRain(true), m_bHasSnow(false)
{
	m_name = "";
	m_Color = 0;
	m_LeafColor = 0;
	topBlock = Tile::grass->m_ID;
	fillerBlock = Tile::dirt->m_ID;
	setMobs(MobCategory::monster, { { EntityType::spider, 10 }, { EntityType::zombie, 10 }, { EntityType::skeleton, 10 }, { EntityType::creeper, 10 }, { EntityType::slime, 10 } });
	setMobs(MobCategory::creature, { { EntityType::sheep, 12 }, { EntityType::pig, 10 }, { EntityType::chicken, 10 }, { EntityType::cow, 8 } });
	setMobs(MobCategory::waterCreature, { { EntityType::squid, 10 } });
}

Biome::~Biome()
{
}

Feature* Biome::getTreeFeature(Random* pRandom)
{
	if (pRandom->nextInt(10) == 0) return new FancyTreeFeature;

	return new TreeFeature;
}

Biome* Biome::setSnowCovered()
{
	m_bHasSnow = true;
	return this;
}

Biome* Biome::setNoRain()
{
	m_bHasRain = false;
	return this;
}

void Biome::setMobs(const MobCategory* category, std::unordered_map<EntityType*, int> spawns)
{
	m_mobSpawns[category] = spawns;
}

void Biome::addMobs(const MobCategory* category, std::unordered_map<EntityType*, int> spawns)
{
	auto match = m_mobSpawns.find(category);
	if (match != m_mobSpawns.end())
	{
		for (auto& type : spawns)
			match->second.insert(type);
	}
	else
	{
		setMobs(category, spawns);
	}
}

const std::unordered_map<EntityType*, int>& Biome::getMobs(const MobCategory* category)
{
	return m_mobSpawns[category];
}

Biome* Biome::setColor(int color)
{
	m_Color = color;
	return this;
}

Biome* Biome::setLeafColor(int color)
{
	m_LeafColor = color;
	return this;
}

Biome* Biome::setName(const std::string & name)
{
	m_name = name;
	return this;
}

void Biome::initBiomes()
{
	rainForest = (new RainforestBiome)
		->setColor(0x08FA36)
		->setName("Rainforest")
		->setLeafColor(0x1FF458);

	swampland = (new SwampBiome)
		->setColor(0x07F9B2)
		->setName("Swampland")
		->setLeafColor(0x8BAF48);

	seasonalForest = (new Biome)
		->setColor(0x9BE023)
		->setName("Seasonal Forest");

	forest = (new ForestBiome)
		->setColor(0x056621)
		->setName("Forest")
		->setLeafColor(0x4EBA31);

	savanna = (new FlatBiome)
		->setColor(0xD9E023)
		->setName("Savanna");

	shrubland = (new Biome)
		->setColor(0xA1AD20)
		->setName("Shrubland");

	taiga = (new TaigaBiome)
		->setColor(0x2EB153)
		->setName("Taiga")
		->setSnowCovered()
		->setLeafColor(0x7BB731);

	desert = (new FlatBiome)
		->setColor(0xFA9418)
		->setName("Desert")
		->setNoRain();

	plains = (new FlatBiome)
		->setColor(0xFFD910)
		->setName("Plains");

	iceDesert = (new FlatBiome)
		->setColor(0xFFED93)
		->setName("Ice Desert")
		->setSnowCovered()
		->setNoRain()
		->setLeafColor(0xC4D339);

	tundra = (new Biome)
		->setColor(0x57EBF9)
		->setName("Tundra")
		->setSnowCovered()
		->setLeafColor(0xC4D339);

	hell = (new HellBiome)
		->setColor(0xFF0000)
		->setName("Hell")
		->setNoRain();

	recalc();
}

Feature* RainforestBiome::getTreeFeature(Random* pRandom)
{
	if (pRandom->nextInt(3) == 0) return new FancyTreeFeature;
	return new TreeFeature;
}

Feature* ForestBiome::getTreeFeature(Random* pRandom)
{
	if (pRandom->nextInt(5) == 0)
	{
		return new BirchFeature;
	}

	if (pRandom->nextInt(3) == 0) return new FancyTreeFeature;
	return new TreeFeature;
}

TaigaBiome::TaigaBiome() : Biome()
{
	addMobs(MobCategory::creature, { { EntityType::wolf, 2 } });
}

Feature* TaigaBiome::getTreeFeature(Random* pRandom)
{
	if (pRandom->nextInt(3) == 0)
		return new PineFeature;
	
	return new SpruceFeature;
}

HellBiome::HellBiome() : Biome()
{
	setMobs(MobCategory::monster, { { EntityType::ghast, 10 }, { EntityType::pigZombie, 10} });
	setMobs(MobCategory::creature, {});
}
