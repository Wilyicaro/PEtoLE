#include "Feature.hpp"
#include "world/level/Level.hpp"

void FancyTreeFeature::generateBranchesAndTrunk()
{
    m_trunkHeight = Mth::min((int)(m_height * m_trunkHeightScale), m_height - 1);

    int branchCount = Mth::max((int)(1.382 + Mth::sqr(m_foliageDensity * m_height / 13.0)), 1);
    int maxBranches = branchCount * m_height;

    std::vector<std::array<int, 4>> foliageCoords;

    int topY = m_origin.y + m_height - m_foliageHeight;
    int trunkTopY = m_origin.y + m_trunkHeight;
    int offset = topY - m_origin.y;

    foliageCoords.push_back({ m_origin.x, topY, m_origin.z, trunkTopY });

    --topY;

    while (offset >= 0)
    {
        float spread = treeShape(offset);
        if (spread < 0.0f) {
            --topY;
            --offset;
            continue;
        }

        for (int i = 0; i < branchCount; ++i)
        {
            double length = m_widthScale * spread * (m_rnd.nextFloat() + 0.328);
            double angle = m_rnd.nextFloat() * 2.0 * M_PI;

            int dx = Mth::round(length * Mth::sin(angle) + m_origin.x);
            int dz = Mth::round(length * Mth::cos(angle) + m_origin.z);
            TilePos branchPos(dx, topY, dz);
            TilePos branchTop = branchPos.above(m_foliageHeight);

            if (checkLine(branchPos, branchTop) == -1)
            {
                double dist = Mth::sqrt(Mth::sqr(Mth::abs(m_origin.x - dx)) + Mth::sqr(Mth::abs(m_origin.z - dz)));
                double dy = dist * m_branchSlope;
                TilePos base(m_origin.x, (branchPos.y - dy > trunkTopY) ? trunkTopY : (int)(branchPos.y - dy), m_origin.z);

                if (checkLine(base, branchPos) == -1)
                {
                    foliageCoords.push_back({ dx, topY, dz, base.y });
                }
            }
        }

        --topY;
        --offset;
    }

    for (const auto& branch : foliageCoords)
    {
        foliageCluster(branch[0], branch[1], branch[2]);
    }

    makeTrunk();

    for (const auto& branch : foliageCoords)
    {
        TilePos base(m_origin);
        base.y = branch[3];
        if (trimBranches(base.y - m_origin.y))
        {
            limb(base, TilePos(branch[0], branch[1], branch[2]), Tile::treeTrunk->m_ID);
        }
    }

    foliageCoords.clear();
}

void FancyTreeFeature::crossection(int var1, int var2, int var3, float var4, uint8_t var5, int var6) {
    int var7 = (int)((double)var4 + 0.618);
    uint8_t var8 = axisConversionArray[var5];
    uint8_t var9 = axisConversionArray[var5 + 3];
    TilePos var10 = TilePos(var1, var2, var3);
    TilePos var11;
    int var12 = -var7;
    int var13 = -var7;

    for (var11[var5] = var10[var5]; var12 <= var7; ++var12) {
        var11[var8] = var10[var8] + var12;
        var13 = -var7;

        while (var13 <= var7) {
            double var14 = Mth::sqrt(Mth::sqr((double)Mth::abs(var12) + 0.5) + Mth::sqr((double)Mth::abs(var13) + 0.5));
            if (var14 > (double)var4) {
                ++var13;
            }
            else {
                var11[var9] = var10[var9] + var13;
                int var16 = m_pLevel->getTile(var11);
                if (var16 != TILE_AIR && var16 != TILE_LEAVES) {
                    ++var13;
                }
                else {
                    m_pLevel->setTileNoUpdate(var11, var6);
                    ++var13;
                }
            }
        }
    }

}

float FancyTreeFeature::treeShape(int var1) {
    if ((double)var1 < (double)((float)m_height) * 0.3) 
    {
        return -1.618F;
    }
    else 
    {
        float var2 = (float)m_height / 2.0;
        float var3 = (float)m_height / 2.0 - (float)var1;
        float var4;
        if (var3 == 0.0F) {
            var4 = var2;
        }
        else if (Mth::abs(var3) >= var2) {
            var4 = 0.0F;
        }
        else {
            var4 = (float)Mth::sqrt(Mth::sqr((double)Mth::abs(var2)) - Mth::sqr((double)Mth::abs(var3)));
        }
        return var4 / 2;
    }
}

float FancyTreeFeature::foliageShape(int var1) {
    return var1 >= 0 && var1 < m_foliageHeight ? (var1 != 0 && var1 != m_foliageHeight - 1 ? 3.0F : 2.0F) : -1.0F;
}

void FancyTreeFeature::foliageCluster(int var1, int var2, int var3) {
    int var4 = var2;

    for (int var5 = var2 + m_foliageHeight; var4 < var5; ++var4) {
        float var6 = foliageShape(var4 - var2);
        crossection(var1, var4, var3, var6, 1, Tile::leaves->m_ID);
    }

}

void FancyTreeFeature::limb(const TilePos& start, const TilePos& end, int blockId)
{
    TilePos var4;
    int8_t var5 = 0;

    int8_t var6;
    for (var6 = 0; var5 < 3; ++var5) {
        var4[var5] = end[var5] - start[var5];
        if (Mth::abs(var4[var5]) > Mth::abs(var4[var6])) {
            var6 = var5;
        }
    }

    if (var4[var6] != 0) {
        int8_t var7 = axisConversionArray[var6];
        int8_t var8 = axisConversionArray[var6 + 3];
        int8_t var9;
        if (var4[var6] > 0) {
            var9 = 1;
        }
        else {
            var9 = -1;
        }

        double var10 = (double)var4[var7] / (double)var4[var6];
        double var12 = (double)var4[var8] / (double)var4[var6];
        TilePos var14;
        int var15 = 0;

        for (int var16 = var4[var6] + var9; var15 != var16; var15 += var9) {
            var14[var6] = Mth::round((double)(start[var6] + var15));
            var14[var7] = Mth::round((double)start[var7] + (double)var15 * var10);
            var14[var8] = Mth::round((double)start[var8] + (double)var15 * var12);
            m_pLevel->setTileNoUpdate(var14, blockId);
        }

    }
}


bool FancyTreeFeature::trimBranches(int var1) {
    return (double)var1 >= (double)m_height * 0.2;
}

void FancyTreeFeature::makeTrunk() {
    TilePos top = m_origin.above(m_trunkHeight);
    limb(m_origin, top, Tile::treeTrunk->m_ID);

    if (m_trunkWidth == 2)
    {
        TilePos x1 = m_origin.offset(1, 0, 0);
        TilePos x2 = m_origin.offset(1, 0, 1);
        TilePos x3 = m_origin.offset(0, 0, 1);
        limb(x1, x1.above(m_trunkHeight), Tile::treeTrunk->m_ID);
        limb(x2, x2.above(m_trunkHeight), Tile::treeTrunk->m_ID);
        limb(x3, x3.above(m_trunkHeight), Tile::treeTrunk->m_ID);
    }
}

int FancyTreeFeature::checkLine(TilePos& var1, TilePos& var2) {
    TilePos var3;

    int8_t var5 = 0;
    for (int8_t var4 = 0; var4 < 3; ++var4) {
        var3[var4] = var2[var4] - var1[var4];
        if (Mth::abs(var3[var4]) > Mth::abs(var3[var5])) {
            var5 = var4;
        }
    }

    if (var3[var5] == 0) {
        return -1;
    }
    else {
        uint8_t var6 = axisConversionArray[var5];
        uint8_t var7 = axisConversionArray[var5 + 3];
        int var8;
        if (var3[var5] > 0) {
            var8 = 1;
        }
        else {
            var8 = -1;
        }

        double var9 = (double)var3[var6] / (double)var3[var5];
        double var11 = (double)var3[var7] / (double)var3[var5];
        TilePos var13;
        int var14 = 0;

        int var15;
        for (var15 = var3[var5] + var8; var14 != var15; var14 += var8) {
            var13[var5] = var1[var5] + var14;
            var13[var6] = Mth::floor((double)var1[var6] + (double)var14 * var9);
            var13[var7] = Mth::floor((double)var1[var7] + (double)var14 * var11);
            int var16 = m_pLevel->getTile(var13);
            if (var16 != TILE_AIR && var16 != TILE_LEAVES) {
                break;
            }
        }

        return var14 == var15 ? -1 : Mth::abs(var14);
    }
}

bool FancyTreeFeature::checkLocation() {
    TilePos var1(m_origin);
    TilePos var2 = m_origin.above(m_height - 1);
    int var3 = m_pLevel->getTile(m_origin.below());
    if (var3 != 2 && var3 != 3) {
        return false;
    }
    else {
        int var4 = checkLine(var1, var2);
        if (var4 == -1) {
            return true;
        }
        else if (var4 < 6) {
            return false;
        }
        else {
            m_height = var4;
            return true;
        }
    }
}

void FancyTreeFeature::init(real a, real b, real c) {
    m_heightVariance = (int)(a * 12.0);
    if (a > 0.5) {
        m_foliageHeight = 5;
    }

    m_widthScale = b;
    m_foliageDensity = c;
}

bool FancyTreeFeature::place(Level* level, Random* random, const TilePos& pos)
{
    m_pLevel = level;
    m_rnd.setSeed(random->nextLong());
    m_origin = pos;
    if (!m_height) {
        m_height = 5 + m_rnd.nextInt(m_heightVariance);
    }

    if (!checkLocation()) {
        return false;
    }
    else {
        generateBranchesAndTrunk();
        return true;
    }
}
