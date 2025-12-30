#include "RedstoneTorchTile.hpp"
#include "world/level/Level.hpp"

std::vector<RedstoneTorchTile::Toggle> RedstoneTorchTile::recentToggles;

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
    return face == Facing::UP ? Tile::redstoneDust->getTexture(face, data) : TorchTile::getTexture(face, data);
}

bool RedstoneTorchTile::isToggledTooFrequently(Level* level, const TilePos& pos, bool addRecent) {
    if (addRecent)
        recentToggles.push_back(Toggle(pos, level->getTime()));

    int count = 0;

    for (auto& toggle : recentToggles)
    {
        if (toggle.pos == pos)
        {
            ++count;
            if (count >= 8)
                return true;
        }
    }

    return false;
}

void RedstoneTorchTile::animateTick(Level* level, const TilePos& pos, Random* random)
{
    if (m_bOn) {
        int data = level->getData(pos);
        real x = pos.x + 0.5F + (random->nextFloat() - 0.5F) * 0.2;
        real y = pos.y + 0.7F + (random->nextFloat() - 0.5F) * 0.2;
        real z = pos.z + 0.5F + (random->nextFloat() - 0.5F) * 0.2;
        constexpr real yd = 0.22;
        constexpr real hd = 0.27;
        if (data == 1) {
            level->addParticle("reddust", Vec3(x - hd, y + yd, z));
        }
        else if (data == 2) {
            level->addParticle("reddust", Vec3(x + hd, y + yd, z));
        }
        else if (data == 3) {
            level->addParticle("reddust", Vec3(x, y + yd, z - hd));
        }
        else if (data == 4) {
            level->addParticle("reddust", Vec3(x, y + yd, z + hd));
        }
        else {
            level->addParticle("reddust", Vec3(x, y, z));
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

    if (m_bOn)
    {
        for (Facing::Name face : Facing::ALL)
            level->updateNeighborsAt(pos.relative(face), m_ID);
    }
}

void RedstoneTorchTile::onRemove(Level* level, const TilePos& pos)
{
    if (m_bOn)
    {
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
    if (!m_bOn)
        return false;
    else
    {
        int data = level->getData(pos);
        if (data == 5 && face == Facing::UP)
            return false;
        else if (data == 3 && face == Facing::SOUTH)
            return false;
        else if (data == 4 && face == Facing::NORTH)
            return false;
        else if (data == 1 && face == Facing::EAST)
            return false;
        else
            return data != 2 || face != Facing::EAST;
    }
}

bool RedstoneTorchTile::isSignalSource() const
{
    return true;
}

bool RedstoneTorchTile::hasNeighborSignal(const Level* level, const TilePos& pos) {
    int data = level->getData(pos);
    if (data == 5 && level->getSignal(pos.below(), Facing::DOWN))
        return true;
    else if (data == 3 && level->getSignal(pos.north(), Facing::NORTH))
        return true;
    else if (data == 4 && level->getSignal(pos.south(), Facing::SOUTH))
        return true;
    else if (data == 1 && level->getSignal(pos.west(), Facing::WEST))
        return true;
    else
        return data == 2 && level->getSignal(pos.east(), Facing::EAST);
}

void RedstoneTorchTile::tick(Level* level, const TilePos& pos, Random* random)
{
    bool signal = hasNeighborSignal(level, pos);

    while (recentToggles.size() > 0 && (level->getTime() - recentToggles[0].when) > 100L) {
        recentToggles.erase(recentToggles.begin());
    }

    if (m_bOn) {
        if (signal) {
            level->setTileAndData(pos, Tile::redstoneTorch->m_ID, level->getData(pos));
            if (isToggledTooFrequently(level, pos, true)) {
                level->playSound(pos.center(), "random.fizz", 0.5F, 2.6F + (level->m_random.nextFloat() - level->m_random.nextFloat()) * 0.8F);

                for (int i = 0; i < 5; ++i) {
                    real x = pos.x + random->nextDouble() * 0.6 + 0.2;
                    real y = pos.y + random->nextDouble() * 0.6 + 0.2;
                    real z = pos.z + random->nextDouble() * 0.6 + 0.2;
                    level->addParticle("smoke", Vec3(x, y, z));
                }
            }
        }
    }
    else if (!signal && !isToggledTooFrequently(level, pos, false)) {
        level->setTileAndData(pos, Tile::redstoneTorchLit->m_ID, level->getData(pos));
    }
}
