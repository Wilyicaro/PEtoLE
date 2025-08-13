#include "MusicTileEntity.hpp"

MusicTileEntity::MusicTileEntity() : m_bOn(false), m_note(0)
{
    m_pType = TileEntityType::music;
}

void MusicTileEntity::load(std::shared_ptr<CompoundTag> tag)
{
    TileEntity::load(tag);
    tag->putByte("note", m_note);
}

void MusicTileEntity::save(std::shared_ptr<CompoundTag> tag)
{
    TileEntity::save(tag);
    m_note = Mth::clamp(tag->getByte("note"), 0, 24);
}

void MusicTileEntity::tune()
{
    m_note = (m_note + 1) % 25;
    setChanged();
}

void MusicTileEntity::playNote(Level* level, const TilePos& pos)
{
    if (level->getMaterial(pos.above()) == Material::air)
    {
        Material* material = level->getMaterial(pos.below());
        uint8_t sound = 0;
        if (material == Material::stone) {
            sound = 1;
        }

        if (material == Material::sand) {
            sound = 2;
        }

        if (material == Material::glass) {
            sound = 3;
        }

        if (material == Material::wood) {
            sound = 4;
        }

        level->tileEvent(pos, sound, m_note);
    }
}