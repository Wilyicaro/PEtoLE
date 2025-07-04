/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "FenceTile.hpp"
#include "world/level/Level.hpp"

FenceTile::FenceTile(int a, int b) : Tile(a, b, Material::wood)
{
}

AABB* FenceTile::getAABB(const Level* pLevel, const TilePos& pos)
{
	AABB* rAABB = Tile::getAABB(pLevel, pos);
	rAABB->max.y += 0.5f;
	return rAABB;
}

bool FenceTile::isSolidRender() const
{
	return false;
}

bool FenceTile::isCubeShaped() const
{
	return false;
}

int FenceTile::getRenderShape() const
{
	return SHAPE_FENCE;
}