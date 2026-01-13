#include "RecordPlayerTile.hpp"
#include "world/level/Level.hpp"
#include "entity/RecordPlayerTileEntity.hpp"

RecordPlayerTile::RecordPlayerTile(int id, int texture) : Tile(id, texture, Material::wood)
{
}

int RecordPlayerTile::getTexture(Facing::Name face) const
{
    return m_TextureFrame + (face == Facing::UP ? 1 : 0);
}

bool RecordPlayerTile::use(Level* level, const TilePos& pos, Player* player)
{
    if (level->m_bIsOnline || !level->getData(pos))
        return false;
    else
    {
        ejectRecord(level, pos);
        return true;
    }
}

void RecordPlayerTile::spawnResources(Level* level, const TilePos& pos, int data, float chance)
{
    if (!level->m_bIsOnline)
        Tile::spawnResources(level, pos, data, chance);
}

void RecordPlayerTile::onRemove(Level* level, const TilePos& pos)
{
    ejectRecord(level, pos);
    Tile::onRemove(level, pos);
}

void RecordPlayerTile::playRecord(Level* level, const TilePos& pos, int record)
{
    if (!level->m_bIsOnline) {
        auto player = std::dynamic_pointer_cast<RecordPlayerTileEntity>(level->getTileEntity(pos));
        player->m_record = record;
        player->setChanged();
        level->setData(pos, 1);
    }
}

void RecordPlayerTile::ejectRecord(Level* level, const TilePos& pos)
{
    if (!level->m_bIsOnline) {
        auto player = std::dynamic_pointer_cast<RecordPlayerTileEntity>(level->getTileEntity(pos));
        int record = player->m_record;
        if (record != 0) {
            level->levelEvent(LevelEvent::SOUND_PLAY_RECORD, pos, 0);
            level->playStreamingMusic(Util::EMPTY_STRING, pos);
            player->m_record = 0;
            player->setChanged();
            level->setData(pos, 0);
            float var8 = 0.7F;
            real var9 = (level->m_random.nextFloat() * var8) + (1.0F - var8) * 0.5;
            real var11 = (level->m_random.nextFloat() * var8) + (1.0F - var8) * 0.2 + 0.6;
            real var13 = (level->m_random.nextFloat() * var8) + (1.0F - var8) * 0.5;
            auto var15 = std::make_shared<ItemEntity>(level, Vec3(pos.x + var9, pos.y + var11, pos.z + var13), std::make_shared<ItemInstance>(record, 1, 0));
            var15->m_throwTime = 10;
            level->addEntity(var15);
        }
    }
}

bool RecordPlayerTile::hasTileEntity() const
{
	return true;
}

std::shared_ptr<TileEntity> RecordPlayerTile::newTileEntity()
{
	return std::make_shared<RecordPlayerTileEntity>();
}