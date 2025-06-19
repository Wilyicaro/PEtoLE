#include "MobSpawnerTile.hpp"
#include "world/level/Level.hpp"
#include "world/item/CompoundContainer.hpp"
#include "entity/MobSpawnerTileEntity.hpp"

MobSpawnerTile::MobSpawnerTile(int id, int texture) : Tile(id, texture, Material::stone)
{
}

bool MobSpawnerTile::hasTileEntity() const
{
    return true;
}

std::shared_ptr<TileEntity> MobSpawnerTile::newTileEntity()
{
    return std::make_shared<MobSpawnerTileEntity>();
}

int MobSpawnerTile::getResource(int, Random*) const 
{
    return 0;
}

int MobSpawnerTile::getResourceCount(Random*) const
{
     return 0;
}

bool MobSpawnerTile::isSolidRender() const
{
    return false;
}