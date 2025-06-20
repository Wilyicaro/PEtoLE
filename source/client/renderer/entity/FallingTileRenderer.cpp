/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "GameMods.hpp"
#if defined(ENH_ALLOW_SAND_GRAVITY)
#include "FallingTileRenderer.hpp"
#include "world/entity/FallingTile.hpp"

FallingTileRenderer::FallingTileRenderer()
{
	m_shadowRadius = 0.5f;
}

void FallingTileRenderer::render(Entity* entity, float x, float y, float z, float a6, float a7)
{
	FallingTile* fallingTile = (FallingTile*)entity;

	glPushMatrix();
	glTranslatef(x, y, z);

	bindTexture(C_TERRAIN_NAME);
	// Render the base
#ifdef ENH_SHADE_HELD_TILES
#define ARGPATCH , getBrightness(entity, 0.0f)
#else
#define ARGPATCH
#endif
	
	m_tileRenderer.renderTile(Tile::tiles[fallingTile->m_id], 0 ARGPATCH);

	glPopMatrix();

#ifdef ARGPATCH
#undef ARGPATCH
#endif
}

#endif
