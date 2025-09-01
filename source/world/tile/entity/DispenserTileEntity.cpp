#include "DispenserTileEntity.hpp"

DispenserTileEntity::DispenserTileEntity() : SimpleContainer(9, "gui.dispenser")
{
    m_pType = TileEntityType::trap;
}

void DispenserTileEntity::load(CompoundIO tag) {
    TileEntity::load(tag);
    SimpleContainer::load(tag);
}

void DispenserTileEntity::save(CompoundIO tag) {
    TileEntity::save(tag);
    SimpleContainer::save(tag);
}

bool DispenserTileEntity::stillValid(Player* player) {
    if (m_pLevel->getTileEntity(m_pos).get() != this) {
        return false;
    }
    return player->distanceToSqr(m_pos.center()) <= 64.0;
}

void DispenserTileEntity::setChanged()
{
    TileEntity::setChanged();
}

bool DispenserTileEntity::removeProjectile(int id)
{
    for (int i = 0; i < getContainerSize(); ++i)
    {
        if (getItem(i) && getItem(i)->m_itemID == id)
            return removeItem(i, 1) != nullptr;
    }
    return false;
}

std::shared_ptr<ItemInstance> DispenserTileEntity::removeRandomItem() {
    int id = -1;
    int h = 1;

    for (int i = 0; i < getContainerSize(); ++i)
    {
        if (getItem(i) && m_random.nextInt(h) == 0)
        {
            id = i;
            ++h;
        }
    }

    if (id >= 0)
    {
        return removeItem(id, 1);
    }
    else return nullptr;
}