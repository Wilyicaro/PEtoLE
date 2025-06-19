/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Feature.hpp"
#include "world/level/Level.hpp"

LakeFeature::LakeFeature(int id)
{
    m_ID = id;
}

bool LakeFeature::place(Level* level, Random* random, const TilePos& pos)
{
	TilePos actual(pos);
	actual.x -= 8;

	for (actual.z -= 8; actual.y > 0 && level->isEmptyTile(actual); --actual.y) {
	}

	actual.y -= 4;
	bool* var6 = new bool[2048];
	std::fill(var6, var6 + 2048, false);
	int var7 = random->nextInt(4) + 4;

	int var8;
	for (var8 = 0; var8 < var7; ++var8) {
		double var9 = random->nextDouble() * 6.0 + 3.0;
		double var11 = random->nextDouble() * 4.0 + 2.0;
		double var13 = random->nextDouble() * 6.0 + 3.0;
		double var15 = random->nextDouble() * (16.0 - var9 - 2.0) + 1.0 + var9 / 2.0;
		double var17 = random->nextDouble() * (8.0 - var11 - 4.0) + 2.0 + var11 / 2.0;
		double var19 = random->nextDouble() * (16.0 - var13 - 2.0) + 1.0 + var13 / 2.0;

		for (int var21 = 1; var21 < 15; ++var21) {
			for (int var22 = 1; var22 < 15; ++var22) {
				for (int var23 = 1; var23 < 7; ++var23) {
					double var24 = ((double)var21 - var15) / (var9 / 2.0);
					double var26 = ((double)var23 - var17) / (var11 / 2.0);
					double var28 = ((double)var22 - var19) / (var13 / 2.0);
					double var30 = var24 * var24 + var26 * var26 + var28 * var28;
					if (var30 < 1.0) {
						var6[(var21 * 16 + var22) * 8 + var23] = true;
					}
				}
			}
		}
	}

	int var10;
	int var32;
	bool var33;
	for (var8 = 0; var8 < 16; ++var8) {
		for (var32 = 0; var32 < 16; ++var32) {
			for (var10 = 0; var10 < 8; ++var10) {
				var33 = !var6[(var8 * 16 + var32) * 8 + var10] && (var8 < 15 && var6[((var8 + 1) * 16 + var32) * 8 + var10] || var8 > 0 && var6[((var8 - 1) * 16 + var32) * 8 + var10] || var32 < 15 && var6[(var8 * 16 + var32 + 1) * 8 + var10] || var32 > 0 && var6[(var8 * 16 + (var32 - 1)) * 8 + var10] || var10 < 7 && var6[(var8 * 16 + var32) * 8 + var10 + 1] || var10 > 0 && var6[(var8 * 16 + var32) * 8 + (var10 - 1)]);
				if (var33) {
					TilePos cPos = actual.offset(var8, var10, var32);
					Material* var12 = level->getMaterial(cPos);
					if (var10 >= 4 && var12->isLiquid()) {
						return false;
					}

					if (var10 < 4 && !var12->isSolid() && level->getTile(cPos) != m_ID) {
						return false;
					}
				}
			}
		}
	}

	for (var8 = 0; var8 < 16; ++var8) {
		for (var32 = 0; var32 < 16; ++var32) {
			for (var10 = 0; var10 < 8; ++var10) {
				if (var6[(var8 * 16 + var32) * 8 + var10]) {
					level->setTileNoUpdate(actual.offset(var8, var10, var32), var10 >= 4 ? 0 : m_ID);
				}
			}
		}
	}

	for (var8 = 0; var8 < 16; ++var8) {
		for (var32 = 0; var32 < 16; ++var32) {
			for (var10 = 4; var10 < 8; ++var10) {
				TilePos cPos = actual.offset(var8, var10, var32);
				if (var6[(var8 * 16 + var32) * 8 + var10] && level->getTile(cPos) == Tile::dirt->m_ID && level->getBrightness(LightLayer::Sky, cPos) > 0) {
					level->setTileNoUpdate(cPos, Tile::grass->m_ID);
				}
			}
		}
	}

	if (Tile::tiles[m_ID]->m_pMaterial == Material::lava) {
		for (var8 = 0; var8 < 16; ++var8) {
			for (var32 = 0; var32 < 16; ++var32) {
				for (var10 = 0; var10 < 8; ++var10) {
					var33 = !var6[(var8 * 16 + var32) * 8 + var10] && (var8 < 15 && var6[((var8 + 1) * 16 + var32) * 8 + var10] || var8 > 0 && var6[((var8 - 1) * 16 + var32) * 8 + var10] || var32 < 15 && var6[(var8 * 16 + var32 + 1) * 8 + var10] || var32 > 0 && var6[(var8 * 16 + (var32 - 1)) * 8 + var10] || var10 < 7 && var6[(var8 * 16 + var32) * 8 + var10 + 1] || var10 > 0 && var6[(var8 * 16 + var32) * 8 + (var10 - 1)]);
					TilePos cPos = actual.offset(var8, var10, var32);
					if (var33 && (var10 < 4 || random->nextInt(2) != 0) && level->getMaterial(cPos)->isSolid()) {
						level->setTileNoUpdate(cPos, Tile::stone->m_ID);
					}
				}
			}
		}
	}

	delete[] var6;

	return true;
}
