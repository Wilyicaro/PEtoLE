#include "ChestTileEntity.hpp"

ChestTileEntity::ChestTileEntity() {
    m_items.fill(nullptr);
}

int ChestTileEntity::getContainerSize() {
    return 27;
}

std::shared_ptr<ItemInstance> ChestTileEntity::getItem(int index) {
    return m_items[index];
}

std::shared_ptr<ItemInstance> ChestTileEntity::removeItem(int index, int count) {
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

void ChestTileEntity::setItem(int index, std::shared_ptr<ItemInstance> item) {
    m_items[index] = item;
    if (item && item->m_count > getMaxStackSize()) {
        item->m_count = getMaxStackSize();
    }
    setChanged();
}

std::string ChestTileEntity::getName() {
    return "Chest";
}

void ChestTileEntity::load(std::shared_ptr<CompoundTag> tag) {
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
}

void ChestTileEntity::save(std::shared_ptr<CompoundTag> tag) {
    TileEntity::save(tag);
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

int ChestTileEntity::getMaxStackSize() {
    return 64;
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
