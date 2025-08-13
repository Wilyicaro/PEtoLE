#include "CraftingMenu.hpp"
#include "Inventory.hpp"
#include "ResultSlot.hpp"
#include "crafting/Recipes.hpp"
#include "world/level/Level.hpp"

CraftingMenu::CraftingMenu(Inventory* inventory, const TilePos& tilePos, Level* level) : m_pos(tilePos), m_level(level)
{
    craftSlots = std::make_shared<CraftingContainer>(this, 3, 3);
    resultSlots = std::make_shared<ResultContainer>();

    addSlot(std::make_shared<ResultSlot>(craftSlots.get(), resultSlots.get(), 0, 124, 35));

    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 3; ++x) {
            addSlot(std::make_shared<Slot>(craftSlots.get(), x + y * 3, 30 + x * 18, 17 + y * 18));
        }
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

void CraftingMenu::slotsChanged(Container* container) 
{
    resultSlots->setItem(0, Recipes::getInstance().getItemFor(craftSlots.get()));
}

void CraftingMenu::removed(Player* player) 
{
    ContainerMenu::removed(player);
    for (int i = 0; i < 9; ++i) {
        std::shared_ptr<ItemInstance> item = craftSlots->getItem(i);
        if (item != nullptr) {
            player->drop(item);
            craftSlots->setItem(i, nullptr);
        }
    }
}

bool CraftingMenu::stillValid(Player* player) const 
{
    if (m_level->getTile(m_pos) != Tile::craftingTable->m_ID) {
        return false;
    }
    else {
        return !(player->distanceToSqr(m_pos.center()) > 64.0);
    }
}
