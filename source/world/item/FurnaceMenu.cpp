#include "FurnaceMenu.hpp"
#include "Slot.hpp"
#include "FurnaceResultSlot.hpp"

FurnaceMenu::FurnaceMenu(Inventory* inventory, std::shared_ptr<FurnaceTileEntity> furnace)
    : m_furnace(furnace)
{
    addSlot(std::make_shared<Slot>(m_furnace.get(), 0, 56, 17));
    addSlot(std::make_shared<Slot>(m_furnace.get(), 1, 56, 53));
    addSlot(std::make_shared<FurnaceResultSlot>(inventory->m_pPlayer, m_furnace.get(), 2, 116, 35));

    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 9; ++x) {
            addSlot(std::make_shared<Slot>(inventory, x + (y + 1) * 9, 8 + x * 18, 84 + y * 18));
        }
    }

    for (int i = 0; i < 9; ++i) {
        addSlot(std::make_shared<Slot>(inventory, i, 8 + i * 18, 142));
    }
}

bool FurnaceMenu::stillValid(Player* player) const
{
    return m_furnace->stillValid(player);
}

void FurnaceMenu::addSlotListener(std::shared_ptr<ContainerListener> listener)
{
    ContainerMenu::addSlotListener(listener);
    listener->setContainerData(this, 0, m_furnace->m_tickCount);
    listener->setContainerData(this, 1, m_furnace->m_litTime);
    listener->setContainerData(this, 2, m_furnace->m_litDuration);
}

void FurnaceMenu::broadcastChanges()
{
    ContainerMenu::broadcastChanges();

    for (const std::shared_ptr<ContainerListener> listener : listeners)
    {
        if (m_lastCookTime != m_furnace->m_tickCount)
            listener->setContainerData(this, 0, m_furnace->m_tickCount);

        if (m_lastBurnTime != m_furnace->m_litTime)
            listener->setContainerData(this, 1, m_furnace->m_litTime);

        if (m_lastLitDuration != m_furnace->m_litDuration)
            listener->setContainerData(this, 2, m_furnace->m_litDuration);
    }

    m_lastCookTime = m_furnace->m_tickCount;
    m_lastBurnTime = m_furnace->m_litTime;
    m_lastLitDuration = m_furnace->m_litDuration;
}

void FurnaceMenu::setData(int index, int value)
{
    if (index == 0)
        m_furnace->m_tickCount = value;
    else if (index == 1)
        m_furnace->m_litTime = value;
    else if (index == 2)
        m_furnace->m_litDuration = value;
}

std::shared_ptr<ItemInstance> FurnaceMenu::quickMoveStack(int index)
{
    std::shared_ptr<ItemInstance> item = nullptr;
    Slot* slot = getSlot(index);
    if (slot && slot->hasItem())
    {
        std::shared_ptr<ItemInstance> slotItem = slot->getItem();
        item = slotItem->copy();
        if (index == 2)
            moveItemStackTo(slotItem, 3, 39, true);
        else if (index >= 3 && index < 30)
            moveItemStackTo(slotItem, 30, 39, false);
        else if (index >= 30 && index < 39)
            moveItemStackTo(slotItem, 3, 30, false);
        else
            moveItemStackTo(slotItem, 3, 39, false);

        if (slotItem->m_count == 0)
            slot->set(nullptr);
        else
            slot->setChanged();

        if (slotItem->m_count == item->m_count)
            return nullptr;

        slot->onTake(slotItem);
    }

    return item;
}
