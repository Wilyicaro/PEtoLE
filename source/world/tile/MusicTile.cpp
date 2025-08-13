#include "MusicTile.hpp"
#include "world/level/Level.hpp"
#include "entity/MusicTileEntity.hpp"

MusicTile::MusicTile(int id, int texture) : Tile(id, texture, Material::wood)
{
}

void MusicTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
    if (tile > 0 && Tile::tiles[tile]->isSignalSource()) {
        bool var6 = level->hasDirectSignal(pos);
        auto var7 = std::dynamic_pointer_cast<MusicTileEntity>(level->getTileEntity(pos));
        if (var7->m_bOn != var6) {
            if (var6) {
                var7->playNote(level, pos);
            }

            var7->m_bOn = var6;
        }
    }

}

bool MusicTile::use(Level* level, const TilePos& pos, Player* player)
{
    if (level->m_bIsOnline)
        return true;
    else
    {
        auto var6 = std::dynamic_pointer_cast<MusicTileEntity>(level->getTileEntity(pos));
        var6->tune();
        var6->playNote(level, pos);
        return true;
    }
}

void MusicTile::attack(Level* level, const TilePos& pos, Player* player)
{
    if (!level->m_bIsOnline)
    {
        auto var6 = std::dynamic_pointer_cast<MusicTileEntity>(level->getTileEntity(pos));
        var6->playNote(level, pos);
    }
}

bool MusicTile::hasTileEntity() const
{
	return true;
}

std::shared_ptr<TileEntity> MusicTile::newTileEntity()
{
	return std::make_shared<MusicTileEntity>();
}

void MusicTile::triggerEvent(Level* level, const TilePos& pos, int note, int info2)
{
    std::string soundType = "harp";
    if (note == 1) {
        soundType = "bd";
    }

    if (note == 2) {
        soundType = "snare";
    }

    if (note == 3) {
        soundType = "hat";
    }

    if (note == 4) {
        soundType = "bassattack";
    }

    level->playSound(pos.center(), "note." + soundType, 3.0F, std::pow(2.0, (info2 - 12) / 12.0));
    level->addParticle("note", Vec3(pos.x + 0.5, pos.y + 1.2, pos.z + 0.5), Vec3(info2 / 24.0, 0, 0));
}
