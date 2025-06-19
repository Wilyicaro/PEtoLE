#include "FurnaceMenu.hpp"
#include "Slot.hpp"
#include "FurnaceResultSlot.hpp"

FurnaceMenu::FurnaceMenu(Inventory* inventory, std::shared_ptr<FurnaceTileEntity> furnace)
    : m_furnace(furnace)
{
    addSlot(std::make_shared<Slot>(m_furnace.get(), 0, 56, 17));
    addSlot(std::make_shared<Slot>(m_furnace.get(), 1, 56, 53));
    addSlot(std::make_shared<FurnaceResultSlot>(std::dynamic_pointer_cast<Player>(inventory->m_pPlayer->shared_from_this()), m_furnace.get(), 2, 116, 35));

    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 9; ++x) {
            addSlot(std::make_shared<Slot>(inventory, x + (y + 1) * 9, 8 + x * 18, 84 + y * 18));
        }
    }

    for (int i = 0; i < 9; ++i) {
        addSlot(std::make_shared<Slot>(inventory, i, 8 + i * 18, 142));
    }
}

bool FurnaceMenu::stillValid(Player* player) const {
    return m_furnace->stillValid(player);
}
