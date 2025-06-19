/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LiquidTileStatic.hpp"
#include "world/level/Level.hpp"

LiquidTileStatic::LiquidTileStatic(int id, Material* pMtl) : LiquidTile(id, pMtl)
{
	setTicking(false);
	if (pMtl == Material::lava)
		setTicking(true);
}

bool LiquidTileStatic::isFlammable(Level* level, const TilePos& pos)
{
	return level->getMaterial(pos)->isFlammable();
}

void LiquidTileStatic::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
	updateLiquid(level, pos);
	if (level->getTile(pos) == m_ID)
		setDynamic(level, pos);
}

void LiquidTileStatic::setDynamic(Level* level, const TilePos& pos)
{
	level->m_noNeighborUpdate = true;
	level->setTileAndDataNoUpdate(pos, m_ID - 1, level->getData(pos));
	level->setTilesDirty(pos, pos);
	level->addToTickNextTick(pos, m_ID - 1, getTickDelay());
	level->m_noNeighborUpdate = false;
}

void LiquidTileStatic::tick(Level* level, const TilePos& pos, Random* random)
{
	if (m_pMaterial != Material::lava)
		return;

	int y2 = random->nextInt(3);
	TilePos tp(pos);
	for (int i = 0; i < y2; ++i)
	{
		tp.x += random->nextInt(3) - 1;
		tp.y++;
		tp.z += random->nextInt(3) - 1;

		TileID tile = level->getTile(tp.above());
		if (tile)
		{
			if (tiles[tile]->m_pMaterial->blocksMotion())
				return;
		}
		else if (
			isFlammable(level, TilePos(tp.x - 1, tp.y + 1, tp.z)) ||
			isFlammable(level, TilePos(tp.x + 1, tp.y + 1, tp.z)) ||
			isFlammable(level, TilePos(tp.x, tp.y + 1, tp.z - 1)) ||
			isFlammable(level, TilePos(tp.x, tp.y + 1, tp.z + 1)) ||
			isFlammable(level, tp) ||
			isFlammable(level, TilePos(tp.x, tp.y + 2, tp.z)))
		{
			level->setTile(tp.above(), Tile::fire->m_ID);
			return;
		}
	}
}
