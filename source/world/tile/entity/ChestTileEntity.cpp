#include "ChestTileEntity.hpp"

ChestTileEntity::ChestTileEntity() : SimpleContainer(27, "gui.chest")
{
    m_pType = TileEntityType::chest;
}

void ChestTileEntity::load(CompoundIO tag) {
    TileEntity::load(tag);
    SimpleContainer::load(tag);
}

void ChestTileEntity::save(CompoundIO tag) {
    TileEntity::save(tag);
    SimpleContainer::save(tag);
}

bool ChestTileEntity::stillValid(Player* player) {
    if (m_pLevel->getTileEntity(m_pos).get() != this) {
        return false;
    }
    return player->distanceToSqr(m_pos.center()) <= 64.0;
}

void ChestTileEntity::setChanged()
{
    TileEntity::setChanged();
}
