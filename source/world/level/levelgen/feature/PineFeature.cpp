/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Feature.hpp"
#include "world/level/Level.hpp"

bool PineFeature::place(Level* level, Random* random, const TilePos& pos) {
    if (pos.y <= C_MIN_Y) return false;

    int height = random->nextInt(5) + 7;
    int bottomRadiusPoint = height - random->nextInt(2) - 3;
    int maxRadius = 1 + random->nextInt(height - bottomRadiusPoint + 1);
    int upperY = pos.y + height;

    // Check if space is available
    for (int y = pos.y; y <= upperY; y++) {
        int range = (y - pos.y < bottomRadiusPoint) ? 0 : maxRadius;

        for (int x = pos.x - range; x <= pos.x + range; x++) {
            for (int z = pos.z - range; z <= pos.z + range; z++) {
                if (y < C_MIN_Y || y >= C_MAX_Y ||
                    (level->getTile(TilePos(x, y, z)) != TILE_AIR &&
                        level->getTile(TilePos(x, y, z)) != Tile::leaves->m_ID)) {
                    return false;
                }
            }
        }
    }
    // Check soil block below
    TileID soil = level->getTile(pos.below());
    if (soil != Tile::grass->m_ID && soil != Tile::dirt->m_ID) {
        return false;
    }
    if (pos.y >= C_MAX_Y - 1 - height) {
        return false;
    }
    level->setTileNoUpdate(pos.below(), Tile::dirt->m_ID);

    // Generate leaves first (top to bottom) to match Java's order
    int range = 0;
    for (int y = upperY; y >= pos.y + bottomRadiusPoint; y--) {
        int b1 = y;
        for (int x = pos.x - range; x <= pos.x + range; x++) {
            int dx = x - pos.x;
            for (int z = pos.z - range; z <= pos.z + range; z++) {
                int dz = z - pos.z;

                if ((std::abs(dx) != range ||
                    std::abs(dz) != range ||
                    range <= 0) &&
                    !Tile::solid[level->getTile(TilePos(x, y, z))]) {
                    level->setTileAndDataNoUpdate(TilePos(x, y, z), Tile::leaves->m_ID, 1);
                }
            }
        }
        if (range >= 1 && y == pos.y + bottomRadiusPoint + 1) {
            range--;
        }
        else if (range < maxRadius) {
            range++;
        }
    }
    // Generate trunk afterwards
    for (int y = 0; y < height - 1; y++) {
        int cy = pos.y + y;
        TileID tile = level->getTile(TilePos(pos.x, cy, pos.z));
        if (tile == TILE_AIR ||
            tile == Tile::leaves->m_ID) {
            level->setTileAndDataNoUpdate(TilePos(pos.x, cy, pos.z), Tile::treeTrunk->m_ID, 1);
        }
    }
    return true;
}

