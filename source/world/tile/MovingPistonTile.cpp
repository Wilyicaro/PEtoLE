#include "MovingPistonTile.hpp"
#include "world/level/Level.hpp"
#include "entity/PistonMovingTileEntity.hpp"

MovingPistonTile::MovingPistonTile(int id) : Tile(id, Material::piston)
{
    setDestroyTime(-1.0f);
}

bool MovingPistonTile::hasTileEntity() const
{
    return true;
}

void MovingPistonTile::onPlace(Level*, const TilePos& pos)
{
}

void MovingPistonTile::onRemove(Level* level, const TilePos& pos)
{
    auto tileEntity = std::dynamic_pointer_cast<PistonMovingTileEntity>(level->getTileEntity(pos));
    if (tileEntity)
        tileEntity->finalTick();
    else
        Tile::onRemove(level, pos);
}

bool MovingPistonTile::mayPlace(const Level*, const TilePos& pos) const
{
    return false;
}

bool MovingPistonTile::mayPlace(const Level*, const TilePos& pos, Facing::Name face) const
{
    return false;
}

int MovingPistonTile::getRenderShape() const
{
    return SHAPE_NONE;
}

bool MovingPistonTile::isSolidRender() const
{
    return false;
}

bool MovingPistonTile::isCubeShaped() const
{
    return false;
}

bool MovingPistonTile::use(Level* level, const TilePos& pos, Player* player)
{
    if (level->m_bIsOnline && !level->getTileEntity(pos))
    {
        level->setTile(pos, TILE_AIR);
        return true;
    }
    else
        return false;
}

int MovingPistonTile::getResource(int, Random*)
{
    return 0;
}

void MovingPistonTile::spawnResources(Level* level, const TilePos& pos, int, float)
{
    if (!level->m_bIsOnline)
    {
        auto piston = std::dynamic_pointer_cast<PistonMovingTileEntity>(level->getTileEntity(pos));
        if (piston)
            Tile::tiles[piston->getTileId()]->spawnResources(level, pos, piston->getData());
    }
}

AABB* MovingPistonTile::getAABB(const Level* level, const TilePos& pos)
{
    auto piston = std::dynamic_pointer_cast<PistonMovingTileEntity>(level->getTileEntity(pos));
    if (piston)
    {
        float progress = piston->getProgress(0.0F);
        if (piston->isExtending())
            progress = 1.0F - progress;

        return moveByPositionAndProgress(level, pos, piston->getTileId(), progress, piston->getDirection());
    }
    return nullptr;
}

void MovingPistonTile::updateShape(const LevelSource* level, const TilePos& pos)
{
    auto piston = std::dynamic_pointer_cast<PistonMovingTileEntity>(level->getTileEntity(pos));
    if (piston) {
        Tile* tile = Tile::tiles[piston->getTileId()];
        if (!tile || tile == this)
            return;

        tile->updateShape(level, pos);
        float progress = piston->getProgress(0.0F);
        if (piston->isExtending())
            progress = 1.0F - progress;

        m_aabb.move(TilePos(Facing::NORMALS[piston->getDirection()]) * -progress);
    }
}

AABB* MovingPistonTile::moveByPositionAndProgress(const Level* level, const TilePos& pos, TileID id, float progress, Facing::Name dir)
{
    if (id && id != m_ID)
    {
        AABB* aabb = Tile::tiles[id]->getAABB(level, pos);
        if (!aabb)
            return nullptr;
        else
        {
            aabb->move(TilePos(Facing::NORMALS[dir]) * -progress);
            return aabb;
        }
    }
    else
        return nullptr;
}

std::shared_ptr<PistonMovingTileEntity> MovingPistonTile::newMovingTileEntity(TileID tile, int meta, Facing::Name direction, bool extending, bool isSourcePiston)
{
    return std::make_shared<PistonMovingTileEntity>(tile, meta, direction, extending, isSourcePiston);
}
