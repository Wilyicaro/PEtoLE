#include "RedstoneTorchTile.hpp"
#include "world/level/Level.hpp"

RedstoneTorchTile::RedstoneTorchTile(int ID, int texture, Material* pMtl, bool on) : TorchTile(ID, texture, pMtl), m_bOn(on)
{
}

int RedstoneTorchTile::getTickDelay() const
{
    return 2;
}

int RedstoneTorchTile::getResource(int, Random*) const
{
	return Tile::redstoneTorchLit->m_ID;
}

int RedstoneTorchTile::getTexture(Facing::Name face, int data) const
{
    return data == 1 ? Tile::redstoneDust->getTexture(face, data) : TorchTile::getTexture(face, data);
}

bool RedstoneTorchTile::isToggledTooFrequently(Level* level, const TilePos& pos, bool addRecent) {
    if (addRecent) {
        m_recentToggles.push_back(Toggle(pos, level->getTime()));
    }

    int count = 0;

    for (auto& toggle : m_recentToggles) {
        if (toggle.pos == pos) {
            ++count;
            if (count >= 8) {
                return true;
            }
        }
    }

    return false;
}

void RedstoneTorchTile::animateTick(Level* level, const TilePos& pos, Random* random)
{
    if (m_bOn) {
        int var6 = level->getData(pos);
        real var7 = pos.x + 0.5F + (random->nextFloat() - 0.5F) * 0.2;
        real var9 = pos.y + 0.7F + (random->nextFloat() - 0.5F) * 0.2;
        real var11 = pos.z + 0.5F + (random->nextFloat() - 0.5F) * 0.2;
        constexpr real var13 = 0.22;
        constexpr real var15 = 0.27;
        if (var6 == 1) {
            level->addParticle("reddust", Vec3(var7 - var15, var9 + var13, var11));
        }
        else if (var6 == 2) {
            level->addParticle("reddust", Vec3(var7 + var15, var9 + var13, var11));
        }
        else if (var6 == 3) {
            level->addParticle("reddust", Vec3(var7, var9 + var13, var11 - var15));
        }
        else if (var6 == 4) {
            level->addParticle("reddust", Vec3(var7, var9 + var13, var11 + var15));
        }
        else {
            level->addParticle("reddust", Vec3(var7, var9, var11));
        }

    }
}

void RedstoneTorchTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
	TorchTile::neighborChanged(level, pos, tile);
	level->addToTickNextTick(pos, m_ID, getTickDelay());
}

void RedstoneTorchTile::onPlace(Level* level, const TilePos& pos)
{
	if (level->getData(pos) == 0) TorchTile::onPlace(level, pos);

    if (m_bOn) {
        for (Facing::Name face : Facing::ALL)
            level->updateNeighborsAt(pos.relative(face), m_ID);
    }
}

void RedstoneTorchTile::onRemove(Level* level, const TilePos& pos)
{
    if (m_bOn) {
        for (Facing::Name face : Facing::ALL)
            level->updateNeighborsAt(pos.relative(face), m_ID);
    }
}

int RedstoneTorchTile::getDirectSignal(const Level* level, const TilePos& pos, Facing::Name face) const
{
    return face == Facing::DOWN ? getSignal(level, pos, face) : false;
}

int RedstoneTorchTile::getSignal(const LevelSource* level, const TilePos& pos, Facing::Name face) const
{
    if (!m_bOn) {
        return false;
    }
    else {
        int var6 = level->getData(pos);
        if (var6 == 5 && face == Facing::UP) {
            return false;
        }
        else if (var6 == 3 && face == Facing::SOUTH) {
            return false;
        }
        else if (var6 == 4 && face == Facing::NORTH) {
            return false;
        }
        else if (var6 == 1 && face == Facing::EAST) {
            return false;
        }
        else {
            return var6 != 2 || face != Facing::EAST;
        }
    }
}

bool RedstoneTorchTile::isSignalSource() const
{
    return true;
}

bool RedstoneTorchTile::hasNeighborSignal(const Level* level, const TilePos& pos) {
    int var5 = level->getData(pos);
    if (var5 == 5 && level->getSignal(pos.below(), Facing::DOWN)) {
        return true;
    }
    else if (var5 == 3 && level->getSignal(pos.north(), Facing::DOWN)) {
        return true;
    }
    else if (var5 == 4 && level->getSignal(pos.south(), Facing::SOUTH)) {
        return true;
    }
    else if (var5 == 1 && level->getSignal(pos.below(), Facing::WEST)) {
        return true;
    }
    else {
        return var5 == 2 && level->getSignal(pos.below(), Facing::EAST);
    }
}

void RedstoneTorchTile::tick(Level* level, const TilePos& pos, Random* random)
{
    bool signal = hasNeighborSignal(level, pos);

    while (m_recentToggles.size() > 0 && (level->getTime() - m_recentToggles[0].when) > 100L) {
        m_recentToggles.erase(m_recentToggles.begin());
    }

    if (m_bOn) {
        if (signal) {
            level->setTileAndData(pos, Tile::redstoneTorch->m_ID, level->getData(pos));
            if (isToggledTooFrequently(level, pos, true)) {
                level->playSound(pos.center(), "random.fizz", 0.5F, 2.6F + (level->m_random.nextFloat() - level->m_random.nextFloat()) * 0.8F);

                for (int var7 = 0; var7 < 5; ++var7) {
                    real var8 = pos.x + random->nextDouble() * 0.6 + 0.2;
                    real var10 = pos.y + random->nextDouble() * 0.6 + 0.2;
                    real var12 = pos.z + random->nextDouble() * 0.6 + 0.2;
                    level->addParticle("smoke", Vec3(var8, var10, var12));
                }
            }
        }
    }
    else if (!signal && !isToggledTooFrequently(level, pos, false)) {
        level->setTileAndData(pos, Tile::redstoneTorchLit->m_ID, level->getData(pos));
    }
}
