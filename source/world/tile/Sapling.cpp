/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Sapling.hpp"
#include "world/level/Level.hpp"

Sapling::Sapling(int id, int texture) : Bush(id, texture)
{
}

int Sapling::getTexture(Facing::Name face, int data) const
{
	data &= 3;
	return data == 1 ? 63 : (data == 2 ? 79 : Bush::getTexture(face, data));
}

void Sapling::tick(Level* level, const TilePos& pos, Random* random)
{
	Bush::tick(level, pos, random);

	if (level->getRawBrightness(pos) > 8 && random->nextInt(7) == 0)
	{
		int data = level->getData(pos);

		if (data & 8)
			growTree(level, pos, random);
		else
			level->setDataNoUpdate(pos, data | 8);
	}
}

bool Sapling::maybeGrowTree(Level* level, const TilePos& pos, Random* random)
{
	Feature* pFeature;

	int data = level->getData(pos);
	switch (data)
	{
		default:
			if (random->nextInt() == 10)
				pFeature = new FancyTreeFeature;
			else pFeature = new TreeFeature;
			break;
		case 1:
			pFeature = new BirchFeature;
			break;
		case 2:
			pFeature = new SpruceFeature;
			break;
	}

	return pFeature->place(level, random, pos);
}

void Sapling::growTree(Level* level, const TilePos& pos, Random* random)
{
	level->setTileNoUpdate(pos, TILE_AIR);

	if (!maybeGrowTree(level, pos, random))
		level->setTileNoUpdate(pos, m_ID);
}

int Sapling::getSpawnResourcesAuxValue(int x) const
{
	return x & 3;
}
