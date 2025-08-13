/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Feature.hpp"
#include "world/level/Level.hpp"

HellSpringFeature::HellSpringFeature(int id)
{
	m_ID = id;
}

bool HellSpringFeature::place(Level* level, Random* random, const TilePos& pos)
{
    if (level->getTile(pos.above()) != Tile::netherrack->m_ID)
        return false;

    if (level->getTile(pos) && level->getTile(pos) != Tile::netherrack->m_ID)
        return false;

    int nRockTiles = 0;
    int nEmptyTiles = 0;

    if (level->getTile(pos.west()) == Tile::netherrack->m_ID) nRockTiles++;
    if (level->getTile(pos.east()) == Tile::netherrack->m_ID) nRockTiles++;
    if (level->getTile(pos.north()) == Tile::netherrack->m_ID) nRockTiles++;
    if (level->getTile(pos.south()) == Tile::netherrack->m_ID) nRockTiles++;
    if (level->getTile(pos.below()) == Tile::netherrack->m_ID) nRockTiles++;

    if (level->isEmptyTile(pos.west())) nEmptyTiles++;
    if (level->isEmptyTile(pos.east())) nEmptyTiles++;
    if (level->isEmptyTile(pos.north())) nEmptyTiles++;
    if (level->isEmptyTile(pos.south())) nEmptyTiles++;
    if (level->isEmptyTile(pos.below())) nEmptyTiles++;

    if (nEmptyTiles != 1) return true;
    if (nRockTiles  != 4) return true;

    level->setTile(pos, m_ID);
    level->m_bInstantTicking = true;
    Tile::tiles[m_ID]->tick(level, pos, random);
    level->m_bInstantTicking = false;
    return true;
}
