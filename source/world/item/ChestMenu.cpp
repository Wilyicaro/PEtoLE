#include "ChestMenu.hpp"
#include "Slot.hpp"

ChestMenu::ChestMenu(Container* inventory, Container* container)
    : m_container(container)
{
    int rows = m_container->getContainerSize() / 9;
    int verticalOffset = (rows - 4) * 18;

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < 9; ++col) {
            int index = col + row * 9;
            int x = 8 + col * 18;
            int y = 18 + row * 18;
            addSlot(std::make_shared<Slot>(m_container, index, x, y));
        }
    }

    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 9; ++col) {
            int index = col + row * 9 + 9;
            int x = 8 + col * 18;
            int y = 103 + row * 18 + verticalOffset;
            addSlot(std::make_shared<Slot>(inventory, index, x, y));
        }
    }

    for (int col = 0; col < 9; ++col) {
        int index = col;
        int x = 8 + col * 18;
        int y = 161 + verticalOffset;
        addSlot(std::make_shared<Slot>(inventory, index, x, y));
    }
}

bool ChestMenu::stillValid(Player* player) const {
    return m_container->stillValid(player);
}

std::shared_ptr<ItemInstance> ChestMenu::quickMoveStack(int index)
{
    std::shared_ptr<ItemInstance> item = nullptr;
    Slot* slot = getSlot(index);
    if (slot && slot->hasItem())
    {
        std::shared_ptr<ItemInstance> slotItem = slot->getItem();
        item = slotItem->copy();
        int rows = m_container->getContainerSize() / 9;
        if (index < rows * 9)
            moveItemStackTo(slotItem, rows * 9, slots.size(), true);
        else
            moveItemStackTo(slotItem, 0, rows * 9, false);

        if (slotItem->m_count == 0)
            slot->set(nullptr);
        else
            slot->setChanged();

        if (slotItem->m_count == item->m_count)
            return nullptr;
    }

    return item;
}
