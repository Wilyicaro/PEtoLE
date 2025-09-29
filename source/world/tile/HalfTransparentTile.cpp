/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "HalfTransparentTile.hpp"
#include "world/level/Level.hpp"

HalfTransparentTile::HalfTransparentTile(int a, int b, Material* c, bool allowSame) : Tile(a, b, c), m_bAllowSame(allowSame)
{
}

bool HalfTransparentTile::isSolidRender() const
{
	return false;
}

bool HalfTransparentTile::shouldRenderFace(const LevelSource* level, const TilePos& pos, Facing::Name face) const
{
	return !m_bAllowSame && level->getTile(pos) == m_ID ? false : Tile::shouldRenderFace(level, pos, face);
}