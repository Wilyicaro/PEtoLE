/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Feature.hpp"
#include "world/level/Level.hpp"

bool HellFireFeature::place(Level* level, Random* random, const TilePos& pos)
{
	TilePos tp;

	for (int i = 0; i < 64; i++)
	{
		tp.x = pos.x + random->nextInt(8) - random->nextInt(8);
		tp.y = pos.y + random->nextInt(4) - random->nextInt(4);
		tp.z = pos.z + random->nextInt(8) - random->nextInt(8);

		if (level->isEmptyTile(tp) && level->getTile(tp.below()) == Tile::netherrack->m_ID)
			level->setTileNoUpdate(tp, Tile::fire->m_ID);
	}

	return true;
}
