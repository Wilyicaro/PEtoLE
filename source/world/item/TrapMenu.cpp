#include "TrapMenu.hpp"
#include "Slot.hpp"

TrapMenu::TrapMenu(Container* inventory, std::shared_ptr<DispenserTileEntity> trap)
    : m_trap(trap)
{
    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 3; ++x) {
            addSlot(std::make_shared<Slot>(trap.get(), x + y * 3, 61 + x * 18, 17 + y * 18));
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
}

bool TrapMenu::stillValid(Player* player) const {
    return m_trap->stillValid(player);
}
