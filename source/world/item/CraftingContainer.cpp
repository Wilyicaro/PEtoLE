#include "CraftingContainer.hpp"
#include "ItemInstance.hpp"

CraftingContainer::CraftingContainer(ContainerMenu* menu, int width, int height)
    : items(width * height, nullptr), width(width), menu(menu) {}

int CraftingContainer::getContainerSize() {
    return items.size();
}

std::shared_ptr<ItemInstance> CraftingContainer::getItem(int index) {
    if (index < 0 || index >= getContainerSize()) return nullptr;
    return items[index];
}

std::shared_ptr<ItemInstance> CraftingContainer::getItem(int x, int y) {
    if (x >= 0 && x < width) {
        int index = x + y * width;
        return getItem(index);
    }
    return nullptr;
}

std::string CraftingContainer::getName() {
    return "Crafting";
}

std::shared_ptr<ItemInstance> CraftingContainer::removeItem(int index, int count) {
    if (index < 0 || index >= getContainerSize()) return nullptr;

    auto item = items[index];
    if (item) {
        std::shared_ptr<ItemInstance> removed;

        if (item->m_count <= count) {
            removed = item;
            items[index] = nullptr;
        } else {
            removed = item->remove(count);
            if (item->m_count == 0) {
                items[index] = nullptr;
            }
        }

        menu->slotsChanged(this);
        return removed;
    }

    return nullptr;
}

void CraftingContainer::setItem(int index, std::shared_ptr<ItemInstance> item) {
    if (index >= 0 && index < getContainerSize()) {
        items[index] = item;
        menu->slotsChanged(this);
    }
}

int CraftingContainer::getMaxStackSize() {
    return 64;
}

void CraftingContainer::setChanged() {
    // No-op
}

bool CraftingContainer::stillValid(Player* player) {
    return true;
}
