/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Feature.hpp"
#include "world/level/Level.hpp"

ClayFeature::ClayFeature(int id, int count)
{
    m_ID = id;
    m_count = count;
}

bool ClayFeature::place(Level* level, Random* random, const TilePos& pos)
{
    if (level->getMaterial(pos) != Material::water) {
        return false;
    }
    else {
        float var6 = random->nextFloat() * M_PI;
        real var7 = (real)((float)(pos.x + 8) + Mth::sin(var6) * (float)m_count / 8.0F);
        real var9 = (real)((float)(pos.x + 8) - Mth::sin(var6) * (float)m_count / 8.0F);
        real var11 = (real)((float)(pos.z + 8) + Mth::cos(var6) * (float)m_count / 8.0F);
        real var13 = (real)((float)(pos.z + 8) - Mth::cos(var6) * (float)m_count / 8.0F);
        real var15 = (real)(pos.y + random->nextInt(3) + 2);
        real var17 = (real)(pos.y + random->nextInt(3) + 2);

        for (int var19 = 0; var19 <= m_count; ++var19) {
            real var20 = var7 + (var9 - var7) * (real)var19 / (real)m_count;
            real var22 = var15 + (var17 - var15) * (real)var19 / (real)m_count;
            real var24 = var11 + (var13 - var11) * (real)var19 / (real)m_count;
            real var26 = random->nextDouble() * (real)m_count / 16.0;
            real var28 = (real)(Mth::sin((float)var19 * M_PI / (float)m_count) + 1.0F) * var26 + 1.0;
            real var30 = (real)(Mth::sin((float)var19 * M_PI / (float)m_count) + 1.0F) * var26 + 1.0;

            TilePos tp;
            for (tp.x = Mth::floor(var20 - var28 / 2.0); tp.x <= Mth::floor(var20 + var28 / 2.0); ++tp.x) {
                for (tp.y = Mth::floor(var22 - var30 / 2.0); tp.y <= Mth::floor(var22 + var30 / 2.0); ++tp.y) {
                    for (tp.z = Mth::floor(var24 - var28 / 2.0); tp.z <= Mth::floor(var24 + var28 / 2.0); ++tp.z) {
                        real var35 = ((real)tp.x + 0.5 - var20) / (var28 / 2.0);
                        real var37 = ((real)tp.y + 0.5 - var22) / (var30 / 2.0);
                        real var39 = ((real)tp.z + 0.5 - var24) / (var28 / 2.0);
                        if (var35 * var35 + var37 * var37 + var39 * var39 < 1.0) {
                            int var41 = level->getTile(tp);
                            if (var41 == Tile::sand->m_ID) {
                                level->setTileNoUpdate(tp, m_ID);
                            }
                        }
                    }
                }
            }
        }

        return true;
    }
}
