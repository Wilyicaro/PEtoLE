#include "DispenserTile.hpp"
#include "world/level/Level.hpp"
#include "entity/DispenserTileEntity.hpp"
#include <world/entity/projectile/Arrow.hpp>
#include "FurnaceTile.hpp"

DispenserTile::DispenserTile(int id, int texture) : Tile(id, texture, Material::stone)
{
}

int DispenserTile::getTickDelay() const
{
    return 4;
}

int DispenserTile::getTexture(const LevelSource* level, const TilePos& pos, Facing::Name face) const
{
    switch (face) {
    case Facing::UP: case Facing::DOWN: return m_TextureFrame + 17;
    default:
        int meta = level->getData(pos);
        return face != meta ? m_TextureFrame : m_TextureFrame + 1;
    }
}

int DispenserTile::getTexture(Facing::Name face) const
{
	switch (face) {
	case Facing::UP: case Facing::DOWN: return m_TextureFrame + 17;
	case Facing::SOUTH: return m_TextureFrame + 1;
	default: return m_TextureFrame;
	}
}

void DispenserTile::onPlace(Level* level, const TilePos& pos)
{
    Tile::onPlace(level, pos);
    FurnaceTile::recalcLockDir(level, pos);
}

void DispenserTile::onRemove(Level* level, const TilePos& pos){
    auto var5 = std::dynamic_pointer_cast<DispenserTileEntity>(level->getTileEntity(pos));

    for (int var6 = 0; var6 < var5->getContainerSize(); ++var6) {
        auto var7 = var5->getItem(var6);
        if (var7) {
            float var8 = m_dispenserRandom.nextFloat() * 0.8F + 0.1F;
            float var9 = m_dispenserRandom.nextFloat() * 0.8F + 0.1F;
            float var10 = m_dispenserRandom.nextFloat() * 0.8F + 0.1F;

            while (var7->m_count) {
                int var11 = m_dispenserRandom.nextInt(21) + 10;
                if (var11 > var7->m_count) {
                    var11 = var7->m_count;
                }

                var7->m_count -= var11;
                auto var12 = std::make_shared<ItemEntity>(level, pos.offset(var8, var9, var10), std::make_shared<ItemInstance>(var7->m_itemID, var11, var7->getAuxValue()));
                float var13 = 0.05F;
                var12->m_vel.x = (double)((float)m_dispenserRandom.nextGaussian() * var13);
                var12->m_vel.y = (double)((float)m_dispenserRandom.nextGaussian() * var13 + 0.2F);
                var12->m_vel.z = (double)((float)m_dispenserRandom.nextGaussian() * var13);
                level->addEntity(var12);
            }
        }
    }

    Tile::onRemove(level, pos);
}

bool DispenserTile::use(Level* level, const TilePos& pos, Player* player) 
{
    if (level->m_bIsOnline)
        return true;

    player->openTrap(std::dynamic_pointer_cast<DispenserTileEntity>(level->getTileEntity(pos)));
    return true;
}

void DispenserTile::fireArrow(Level* level, const TilePos& pos, Random* random) {
    int var6 = level->getData(pos);
    float xDir = 0.0F;
    float zDir = 0.0F;
    if (var6 == 3) {
        zDir = 1.0F;
    }
    else if (var6 == 2) {
        zDir = -1.0F;
    }
    else if (var6 == 5) {
        xDir = 1.0F;
    }
    else {
        xDir = -1.0F;
    }

    auto var11 = std::dynamic_pointer_cast<DispenserTileEntity>(level->getTileEntity(pos));
    auto randomItem = var11->removeRandomItem();
    Vec3 vel(pos.x + xDir * 0.5 + 0.5, pos.y + 0.5, pos.z + zDir * 0.5 + 0.5);
    if (!randomItem) {
        level->playSound(Vec3(pos), "random.click", 1.0F, 1.2F);
    }
    else {
        real var20;
        if (randomItem->m_itemID == Item::arrow->m_itemID)
        {
            auto arrow = std::make_shared<Arrow>(level, vel);
            arrow->shoot(Vec3(xDir, 0.1, zDir), 1.1F, 6.0F);
            level->addEntity(arrow);
            level->playSound(Vec3(pos), "random.bow", 1.0F, 1.2F);
        }
        //else if (randomItem->m_itemID == Item::egg->m_itemID)
        //{
        //    auto egg = std::make_shared<ThrownEgg>(level, vel);
        //     egg->shoot(Vec3(xDir, 0.1, zDir), 1.1F, 6.0F);
        //     level->addEntity(egg);
        //     level->playSound(Vec3(pos), "random.bow", 1.0F, 1.2F);
        //}
        //else if (randomItem->m_itemID == Item::snowBall->m_itemID)
        //{
        //    auto snowball = std::make_shared<Snowball>(level, vel);
        //    snowball.shoot((double)xDir, 0.1, (double)zDir, 1.1F, 6.0F);
        //    level->addEntity(snowball);
        //    level->playSound(Vec3(pos), "random.bow", 1.0F, 1.2F);
        //}
        else {
            auto itemEnt = std::make_shared<ItemEntity>(level, vel.add(0, -0.3, 0), randomItem);
            var20 = random->nextDouble() * 0.1 + 0.2;
            itemEnt->m_vel.x = xDir * var20;
            itemEnt->m_vel.y = 0.2;
            itemEnt->m_vel.z = zDir * var20;
            itemEnt->m_vel.x += random->nextGaussian() * 0.0075 * 6.0;
            itemEnt->m_vel.y += random->nextGaussian() * 0.0075 * 6.0;
            itemEnt->m_vel.z += random->nextGaussian() * 0.0075 * 6.0;
            level->addEntity(itemEnt);
            level->playSound(Vec3(pos), "random.click", 1.0F, 1.0F);
        }

        for (int var37 = 0; var37 < 10; ++var37) {
            var20 = random->nextDouble() * 0.2 + 0.01;
            real var22 = vel.x + xDir * 0.01 + (random->nextDouble() - 0.5) * zDir * 0.5;
            real var24 = vel.y + (random->nextDouble() - 0.5) * 0.5;
            real var26 = vel.z + zDir * 0.01 + (random->nextDouble() - 0.5) * xDir * 0.5;
            real var28 = xDir * var20 + random->nextGaussian() * 0.01;
            real var30 = -0.03 + random->nextGaussian() * 0.01;
            real var32 = zDir * var20 + random->nextGaussian() * 0.01;
            level->addParticle("smoke", Vec3(var22, var24, var26), Vec3(var28, var30, var32));
        }
    }

}

void DispenserTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
    if (tile && Tile::tiles[tile]->isSignalSource() && (level->hasNeighborSignal(pos) || level->hasNeighborSignal(pos.above())))
        level->addToTickNextTick(pos, m_ID, getTickDelay());
}

void DispenserTile::tick(Level* level, const TilePos& pos, Random* random)
{
    if (level->hasNeighborSignal(pos) || level->hasNeighborSignal(pos.above()))
        fireArrow(level, pos, random);
}

void DispenserTile::setPlacedBy(Level* level, const TilePos& pos, Mob* mob, Facing::Name face)
{
    int rot = Mth::floor(0.5f + (mob->m_rot.y * 4.0f / 360.0f)) & 3;

    int data = 4;

    switch (rot)
    {
    case 0: data = 2; break;
    case 1: data = 5; break;
    case 2: data = 3; break;
    }

    level->setData(pos, data);
}

bool DispenserTile::hasTileEntity() const
{
    return true;
}

std::shared_ptr<TileEntity> DispenserTile::newTileEntity()
{
    return std::make_shared<DispenserTileEntity>();
}
