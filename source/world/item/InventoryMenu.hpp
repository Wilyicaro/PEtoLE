#pragma once

#include "ContainerMenu.hpp"
#include "CraftingContainer.hpp"
#include "ResultContainer.hpp"

class InventoryMenu : public ContainerMenu {
public:
    std::shared_ptr<CraftingContainer> craftSlots;
    std::shared_ptr<ResultContainer> resultSlots;
    bool active;

    InventoryMenu(Inventory* inventory, bool active = true);

    void slotsChanged(Container* container) override;
    void removed(Player* player) override;
    bool stillValid(Player* player) const override;
};
