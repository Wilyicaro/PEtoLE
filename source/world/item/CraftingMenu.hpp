#pragma once

#include "ContainerMenu.hpp"
#include "CraftingContainer.hpp"
#include "ResultContainer.hpp"

class CraftingMenu : public ContainerMenu {
public:
    std::shared_ptr<CraftingContainer> craftSlots;
    std::shared_ptr<ResultContainer> resultSlots;

    CraftingMenu(Inventory* inventory, const TilePos& tilePos, Level* level);

    void slotsChanged(Container* container) override;
    void removed(Player* player) override;
    bool stillValid(Player* player) const override;
    virtual std::shared_ptr<ItemInstance> quickMoveStack(int index) override;

private:
    const TilePos m_pos;
    Level* m_level;
};
