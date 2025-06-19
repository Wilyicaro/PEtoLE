/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Feature.hpp"
#include "world/level/Level.hpp"

bool SpruceFeature::place(Level* level, Random* random, const TilePos& pos) {
    if (pos.y <= C_MIN_Y) return false;

    int height = random->nextInt(4) + 6;
    int base = 1 + random->nextInt(2);
    int leafStart = height - base;
    int leafRadius = 2 + random->nextInt(2);
    bool canPlace = true;

    // Validate space first
    for (int y = pos.y; y <= pos.y + height && canPlace; y++) {
        int range = (y - pos.y < base) ? 0 : leafRadius;
        for (int x = pos.x - range; x <= pos.x + range && canPlace; x++) {
            for (int z = pos.z - range; z <= pos.z + range && canPlace; z++) {
                if (y < 0 || y >= C_MAX_Y ||
                    (level->getTile(TilePos(x, y, z)) != TILE_AIR &&
                        level->getTile(TilePos(x, y, z)) != Tile::leaves->m_ID)) {
                    canPlace = false;
                }
            }
        }
    }
    if (!canPlace) return false;

    // Validate soil
    TileID soil = level->getTile(pos.below());
    if (soil != Tile::grass->m_ID && soil != Tile::dirt->m_ID) return false;

    if (pos.y >= C_MAX_Y - height) return false;

    // Prepare soil
    level->setTileNoUpdate(pos.below(), Tile::dirt->m_ID);

    // Leaves first
    int radius = random->nextInt(2);
    int rStep = 1;
    int rReset = 0;

    for (int y = 0; y <= leafStart; y++) {
        int cy = pos.y + height - y;
        for (int x = pos.x - radius; x <= pos.x + radius; x++) {
            for (int z = pos.z - radius; z <= pos.z + radius; z++) {
                if ((std::abs(x - pos.x) != radius ||
                    std::abs(z - pos.z) != radius ||
                    radius <= 0) &&
                    !Tile::solid[level->getTile(TilePos(x, cy, z))]) {
                    level->setTileAndDataNoUpdate(TilePos(x, cy, z), Tile::leaves->m_ID, 1);
                }
            }
        }
        if (radius >= rStep) {
            radius = rReset;
            rReset = 1;
            rStep++;
            if (rStep > leafRadius) rStep = leafRadius;
        }
        else {
            radius++;
        }
    }

    // Then trunk
    int trunkHeight = height - random->nextInt(3);
    for (int y = 0; y < trunkHeight; y++) {
        int cy = pos.y + y;
        TileID tile = level->getTile(TilePos(pos.x, cy, pos.z));
        if (tile == TILE_AIR ||
            tile == Tile::leaves->m_ID) {
            level->setTileAndDataNoUpdate(TilePos(pos.x, cy, pos.z), Tile::treeTrunk->m_ID, 1);
        }
    }
    return true;
}
