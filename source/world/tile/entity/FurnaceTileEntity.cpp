#include "FurnaceTileEntity.hpp"
#include "world/item/crafting/FurnaceRecipes.hpp"
#include <world/tile/FurnaceTile.hpp>

FurnaceTileEntity::FurnaceTileEntity() : SimpleContainer(3, "gui.furnace")
{
    m_pType = TileEntityType::furnace;
}

void FurnaceTileEntity::load(CompoundIO tag) 
{
    TileEntity::load(tag);
    SimpleContainer::load(tag);

    m_litTime = tag->getShort("BurnTime");
    m_tickCount = tag->getShort("CookTime");
    m_litDuration = FurnaceRecipes::getInstance().getBurnDuration(getItem(1));
}

void FurnaceTileEntity::save(CompoundIO tag) 
{
    TileEntity::save(tag);
    tag->putShort("BurnTime", m_litTime);
    tag->putShort("CookTime", m_tickCount);
    SimpleContainer::save(tag);
}

void FurnaceTileEntity::tick()
{
    bool var1 = m_litTime > 0;
    bool var2 = false;
    if (m_litTime > 0) {
        --m_litTime;
    }

    if (!m_pLevel->m_bIsOnline) {
        if (m_litTime == 0 && canBurn()) {
            m_litDuration = m_litTime = FurnaceRecipes::getInstance().getBurnDuration(getItem(1));
            if (m_litTime > 0) {
                var2 = true;
                if (getItem(1)) {
                    --getItem(1)->m_count;
                    if (!getItem(1)->m_count) {
                        setItem(1, nullptr);
                    }
                }
            }
        }

        if (isLit() && canBurn()) {
            ++m_tickCount;
            if (m_tickCount == 200) {
                m_tickCount = 0;
                burn();
                var2 = true;
            }
        }
        else {
            m_tickCount = 0;
        }

        if (var1 != m_litTime > 0) {
            var2 = true;
            FurnaceTile::setLit(m_litTime > 0, m_pLevel, m_pos);
        }
    }

    if (var2) {
        setChanged();
    }
}

bool FurnaceTileEntity::canBurn()
{
    if (!getItem(0)) {
        return false;
    }
    else {
        auto result = FurnaceRecipes::getInstance().getItemFor(this);
        if (!result) {
            return false;
        }
        else if (!getItem(2)) {
            return true;
        }
        else if (!getItem(2)->sameItem(result)) {
            return false;
        }
        else if (getItem(2)->m_count < getMaxStackSize() && getItem(2)->m_count < getItem(2)->getMaxStackSize()) {
            return true;
        }
        else {
            return getItem(2)->m_count < result->getMaxStackSize();
        }
    }
}

void FurnaceTileEntity::burn() {
    if (canBurn()) {
        auto result = FurnaceRecipes::getInstance().getItemFor(this);
        if (!getItem(2)) {
           setItem(2, result);
        }
        else if (getItem(2)->sameItem(result)) {
            ++getItem(2)->m_count;
        }

        --getItem(0)->m_count;
        if (getItem(0)->m_count <= 0) {
            setItem(0, nullptr);
        }

    }
}

int FurnaceTileEntity::getBurnProgress(int height) 
{
    return m_tickCount * height / 200;
}

int FurnaceTileEntity::getLitProgress(int height) {
    if (m_litDuration == 0) {
        m_litDuration = 200;
    }

    return m_litTime * height / m_litDuration;
}

bool FurnaceTileEntity::isLit() 
{
    return m_litTime > 0;
}

bool FurnaceTileEntity::stillValid(Player* player) 
{
    if (m_pLevel->getTileEntity(m_pos).get() != this) {
        return false;
    }
    return player->distanceToSqr(m_pos.center()) <= 64.0;
}

void FurnaceTileEntity::setChanged()
{
    TileEntity::setChanged();
}
