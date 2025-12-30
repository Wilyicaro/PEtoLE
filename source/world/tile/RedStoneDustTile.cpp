/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "RedStoneDustTile.hpp"
#include "world/level/Level.hpp"

RedStoneDustTile::RedStoneDustTile(int id, int texture) : Tile(id, texture, Material::decoration)
{
	setShape(0.0F, 0.0F, 0.0F, 1.0F, 1.0F / 16.0F, 1.0F);
}

AABB* RedStoneDustTile::getAABB(const Level*, const TilePos& pos)
{
	return nullptr;
}

bool RedStoneDustTile::isSolidRender() const
{
	return false;
}

bool RedStoneDustTile::isCubeShaped() const
{
	return false;
}

int RedStoneDustTile::getRenderShape() const
{
	return SHAPE_DUST;
}

int RedStoneDustTile::getColor(const LevelSource*, const TilePos& pos, Facing::Name facing, int texture) const
{
    return 0x800000;
}

bool RedStoneDustTile::mayPlace(const Level* level, const TilePos& pos) const
{
	return level->isNormalTile(pos.below());
}

int RedStoneDustTile::getResource(int, Random*) const
{
    return Item::redStone->m_itemID;
}

int RedStoneDustTile::getDirectSignal(const Level* level, const TilePos& pos, Facing::Name face) const
{
    return m_shouldSignal ? getSignal(level, pos, face) : 0;
}

int RedStoneDustTile::getSignal(const LevelSource* level, const TilePos& pos, Facing::Name face) const
{
    if (!m_shouldSignal) {
        return false;
    }
    else if (level->getData(pos) == 0) {
        return false;
    }
    else if (face == 1) {
        return true;
    }
    else {
        TilePos west = pos.west();
        TilePos east = pos.east();
        TilePos north = pos.north();
        TilePos south = pos.south();
        bool var6 = shouldConnectTo(level, west, 1) || !level->isNormalTile(west) && shouldConnectTo(level, west.below(), -1);
        bool var7 = shouldConnectTo(level, east, 3) || !level->isNormalTile(east) && shouldConnectTo(level, east.below(), -1);
        bool var8 = shouldConnectTo(level, north, 2) || !level->isNormalTile(north) && shouldConnectTo(level, north.below(), -1);
        bool var9 = shouldConnectTo(level, south, 0) || !level->isNormalTile(south) && shouldConnectTo(level, south.below(), -1);
        if (!level->isNormalTile(pos.above())) {
            if (level->isNormalTile(west) && shouldConnectTo(level, west.above(), -1)) {
                var6 = true;
            }

            if (level->isNormalTile(east) && shouldConnectTo(level, east.above(), -1)) {
                var7 = true;
            }

            if (level->isNormalTile(north) && shouldConnectTo(level, north.above(), -1)) {
                var8 = true;
            }

            if (level->isNormalTile(south) && shouldConnectTo(level, south.above(), -1)) {
                var9 = true;
            }
        }

        if (!var8 && !var7 && !var6 && !var9 && Facing::isVertical(face)) {
            return true;
        }
        else if (face == Facing::NORTH && var8 && !var6 && !var7) {
            return true;
        }
        else if (face == Facing::SOUTH && var9 && !var6 && !var7) {
            return true;
        }
        else if (face == Facing::WEST && var6 && !var8 && !var9) {
            return true;
        }
        else {
            return face == Facing::EAST && var7 && !var8 && !var9;
        }
    }
}

void RedStoneDustTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
    if (!level->m_bIsOnline) {
        int var6 = level->getData(pos);
        bool var7 = mayPlace(level, pos);
        if (!var7) {
            spawnResources(level, pos, var6);
            level->setTile(pos, 0);
        }
        else {
            updatePowerStrength(level, pos);
        }

        Tile::neighborChanged(level, pos, tile);
    }
}

bool RedStoneDustTile::isSignalSource() const
{
    return m_shouldSignal;
}

void RedStoneDustTile::animateTick(Level* level, const TilePos& pos, Random* random)
{
    if (level->getData(pos) > 0) {
        real var6 = pos.x + 0.5 + ((real)random->nextFloat() - 0.5) * 0.2;
        real var8 = pos.y + 0.0625F;
        real var10 = pos.z + 0.5 + ((real)random->nextFloat() - 0.5) * 0.2;
        level->addParticle("reddust", Vec3(var6, var8, var10));
    }
}

void RedStoneDustTile::updatePowerStrength(Level* level, const TilePos& pos) 
{
    updatePowerStrength(level, pos, pos);

    std::set<TilePos> copy = m_toUpdate;
    m_toUpdate.clear();

    for (auto& tp : copy)
    {
        level->updateNeighborsAt(tp, m_ID);
    }
}

void RedStoneDustTile::updatePowerStrength(Level* level, const TilePos& from, const TilePos& to)
{
    int var8 = level->getData(from);
    int var9 = 0;
    m_shouldSignal = false;
    bool var10 = level->hasNeighborSignal(from);
    m_shouldSignal = true;
    int var11;
   
    if (var10) {
        var9 = 15;
    }
    else {
        for (var11 = 0; var11 < 4; ++var11) {
            TilePos tp(from);
            if (var11 == 0) {
                --tp.x;
            }

            if (var11 == 1) {
                ++tp.x;
            }

            if (var11 == 2) {
                --tp.z;
            }

            if (var11 == 3) {
                ++tp.z;
            }

            if (tp != to) {
                var9 = checkTarget(level, tp, var9);
            }

            if (level->isNormalTile(tp) && !level->isNormalTile(from.above())) {
                TilePos above = tp.above();
                if (above != to) {
                    var9 = checkTarget(level, above, var9);
                }
            }
            else if (!level->isNormalTile(tp) && (tp.x != to.x || from.y - 1 != to.y || tp.z != to.z)) {
                var9 = checkTarget(level, tp.below(), var9);
            }
        }

        if (var9 > 0) {
            --var9;
        }
        else {
            var9 = 0;
        }
    }

    if (var8 != var9) {
        level->m_noNeighborUpdate = true;
        level->setData(from, var9);
        level->setTilesDirty(from, from);
        level->m_noNeighborUpdate = false;

        for (var11 = 0; var11 < 4; ++var11)
        {
            TilePos tp(from);
            int var14 = from.y - 1;
            if (var11 == 0) {
                tp.x = from.x - 1;
            }

            if (var11 == 1) {
                ++tp.x;
            }

            if (var11 == 2) {
                tp.z = from.z - 1;
            }

            if (var11 == 3) {
                ++tp.z;
            }

            if (level->isNormalTile(tp)) {
                var14 += 2;
            }

    
            int var16 = checkTarget(level, tp, -1);
            var9 = level->getData(from);
            if (var9 > 0) {
                --var9;
            }

            if (var16 >= 0 && var16 != var9) {
               updatePowerStrength(level, tp, from);
            }

            TilePos fTp = TilePos(tp.x, var14, tp.z);
            var16 = checkTarget(level, fTp, -1);
            var9 = level->getData(from);
            if (var9 > 0) {
                --var9;
            }

            if (var16 >= 0 && var16 != var9) {
              updatePowerStrength(level, fTp, from);
            }
        }

        if (var8 == 0 || var9 == 0) {
            m_toUpdate.insert(from);
            for (Facing::Name face : Facing::ALL)
            {
                m_toUpdate.insert(from.relative(face));
            }
        }
    }

}

int RedStoneDustTile::checkTarget(Level* level, const TilePos& pos, int data)
{
    if (level->getTile(pos) != m_ID) {
        return data;
    }
    else {
        int var6 = level->getData(pos);
        return var6 > data ? var6 : data;
    }
}

void RedStoneDustTile::checkCornerChangeAt(Level* level, const TilePos& pos) 
{
    if (level->getTile(pos) == m_ID) {
        level->updateNeighborsAt(pos, m_ID);
        for (Facing::Name face : Facing::ALL)
            level->updateNeighborsAt(pos.relative(face), m_ID);
    }
}

bool RedStoneDustTile::shouldConnectTo(const LevelSource* level, const TilePos& pos, int side)
{
    int var4 = level->getTile(pos);
    if (var4 == Tile::redstoneDust->m_ID) {
        return true;
    }
    else if (var4 == 0) {
        return false;
    }
    else if (Tile::tiles[var4]->isSignalSource())
        return true;
    else if (var4 != Tile::repeater->m_ID && var4 != Tile::repeaterLit->m_ID) {
        return false;
    }
    else {
        return side == repeaterConnect[level->getData(pos) & 3];
    }
}

void RedStoneDustTile::checkSignalUpdates(Level* level, const TilePos& pos)
{
    if (!level->m_bIsOnline) {
        updatePowerStrength(level, pos);
        TilePos above = pos.above();
        TilePos below = pos.below();
        level->updateNeighborsAt(above, m_ID);
        level->updateNeighborsAt(below, m_ID);

        for (Facing::Name face : Facing::HORIZONTAL)
        {
            TilePos rel = pos.relative(face);
            checkCornerChangeAt(level, rel);
            if (level->isNormalTile(rel)) {
                checkCornerChangeAt(level, rel.above());
            }
            else {
                checkCornerChangeAt(level, rel.below());
            }
        }

    }
}

void RedStoneDustTile::onPlace(Level* level, const TilePos& pos)
{
    Tile::onPlace(level, pos);
    checkSignalUpdates(level, pos);
    
}

void RedStoneDustTile::onRemove(Level* level, const TilePos& pos)
{
    Tile::onRemove(level, pos);
    checkSignalUpdates(level, pos);
}
