#include "InventoryMenu.hpp"
#include "Inventory.hpp"
#include "ResultSlot.hpp"
#include "ArmorSlot.hpp"
#include "crafting/Recipes.hpp"

InventoryMenu::InventoryMenu(Inventory* inventory, bool active) : active(active) 
{
    craftSlots = std::make_shared<CraftingContainer>(this, 2, 2);
    resultSlots = std::make_shared<ResultContainer>();

    addSlot(std::make_shared<ResultSlot>(inventory->m_pPlayer, craftSlots.get(), resultSlots.get(), 0, 144, 36));

    for (int y = 0; y < 2; ++y) {
        for (int x = 0; x < 2; ++x) {
            addSlot(std::make_shared<Slot>(craftSlots.get(), x + y * 2, 88 + x * 18, 26 + y * 18));
        }
    }

    for (int i = 0; i < 4; ++i) {
        addSlot(std::make_shared<ArmorSlot>(inventory, i, inventory->getContainerSize() - 1 - i, 8, 8 + i * 18));
    }

    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 9; ++x) {
            addSlot(std::make_shared<Slot>(inventory, x + (y + 1) * 9, 8 + x * 18, 84 + y * 18));
        }
    }

    for (int i = 0; i < 9; ++i) {
        addSlot(std::make_shared<Slot>(inventory, i, 8 + i * 18, 142));
    }

    slotsChanged(craftSlots.get());
}

void InventoryMenu::slotsChanged(Container* container) 
{
    resultSlots->setItem(0, Recipes::getInstance().getItemFor(craftSlots.get()));
}

void InventoryMenu::removed(Player* player) 
{
    ContainerMenu::removed(player);
    for (int i = 0; i < 4; ++i) {
        std::shared_ptr<ItemInstance> item = craftSlots->getItem(i);
        if (item)
        {
            player->drop(item);
            craftSlots->setItem(i, nullptr);
        }
    }
}

bool InventoryMenu::stillValid(Player* player) const 
{
    return true;
}
