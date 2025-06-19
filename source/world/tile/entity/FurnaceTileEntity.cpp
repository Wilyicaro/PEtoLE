#include "FurnaceTileEntity.hpp"
#include "world/item/crafting/FurnaceRecipes.hpp"
#include <world/tile/FurnaceTile.hpp>

FurnaceTileEntity::FurnaceTileEntity() {
    m_items.fill(nullptr);
}

int FurnaceTileEntity::getContainerSize() {
    return m_items.size();
}

std::shared_ptr<ItemInstance> FurnaceTileEntity::getItem(int index) {
    return m_items[index];
}

std::shared_ptr<ItemInstance> FurnaceTileEntity::removeItem(int index, int count) 
{
    if (m_items[index] != nullptr) {
        std::shared_ptr<ItemInstance> result;
        if (m_items[index]->m_count <= count) {
            result = m_items[index];
            m_items[index] = nullptr;
            setChanged();
            return result;
        } else {
            result = m_items[index]->remove(count);
            if (m_items[index]->m_count == 0) {
                m_items[index] = nullptr;
            }
            setChanged();
            return result;
        }
    }
    return nullptr;
}

void FurnaceTileEntity::setItem(int index, std::shared_ptr<ItemInstance> item) 
{
    m_items[index] = item;
    if (item && item->m_count > getMaxStackSize()) {
        item->m_count = getMaxStackSize();
    }
    setChanged();
}

std::string FurnaceTileEntity::getName() 
{
    return "Furnace";
}

void FurnaceTileEntity::load(std::shared_ptr<CompoundTag> tag) 
{
    TileEntity::load(tag);
    auto list = tag->getList("Items");
    m_items.fill(nullptr);

    for (const auto& element : list->getValue()) {
        auto itemTag = std::dynamic_pointer_cast<CompoundTag>(element);
        if (itemTag) {
            int slot = itemTag->getByte("Slot") & 255;
            if (slot >= 0 && slot < static_cast<int>(m_items.size())) {
                m_items[slot] = std::make_shared<ItemInstance>(itemTag);
            }
        }
    }

    m_litTime = tag->getShort("BurnTime");
    m_tickCount = tag->getShort("CookTime");
    m_litDuration = FurnaceRecipes::getInstance().getBurnDuration(m_items[1]);
}

void FurnaceTileEntity::save(std::shared_ptr<CompoundTag> tag) 
{
    TileEntity::save(tag);
    tag->putShort("BurnTime", m_litTime);
    tag->putShort("CookTime", m_tickCount);

    auto list = std::make_shared<ListTag>();

    for (int i = 0; i < static_cast<int>(m_items.size()); ++i) {
        if (m_items[i]) {
            auto itemTag = std::make_shared<CompoundTag>();
            itemTag->putByte("Slot", static_cast<uint8_t>(i));
            m_items[i]->save(itemTag);
            list->add(itemTag);
        }
    }

    tag->put("Items", list);
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
            m_litDuration = m_litTime = FurnaceRecipes::getInstance().getBurnDuration(m_items[1]);
            if (m_litTime > 0) {
                var2 = true;
                if (m_items[1]) {
                    --m_items[1]->m_count;
                    if (!m_items[1]->m_count) {
                        m_items[1] = nullptr;
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
    if (!m_items[0]) {
        return false;
    }
    else {
        auto result = FurnaceRecipes::getInstance().getItemFor(this);
        if (!result) {
            return false;
        }
        else if (!m_items[2]) {
            return true;
        }
        else if (!m_items[2]->sameItem(result)) {
            return false;
        }
        else if (m_items[2]->m_count < getMaxStackSize() && m_items[2]->m_count < m_items[2]->getMaxStackSize()) {
            return true;
        }
        else {
            return m_items[2]->m_count < result->getMaxStackSize();
        }
    }
}

void FurnaceTileEntity::burn() {
    if (canBurn()) {
        auto result = FurnaceRecipes::getInstance().getItemFor(this);
        if (!m_items[2]) {
            m_items[2] = result;
        }
        else if (m_items[2]->sameItem(result)) {
            ++m_items[2]->m_count;
        }

        --m_items[0]->m_count;
        if (m_items[0]->m_count <= 0) {
            m_items[0] = nullptr;
        }

    }
}

int FurnaceTileEntity::getMaxStackSize()
{
    return 64;
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
