#pragma once

#include "Slot.hpp"
#include "Container.hpp"

class ArmorSlot : public Slot {

public:
    int m_armorIndex;
    ArmorSlot(Container* container, int armorIndex, int slotIndex, int x, int y);

    bool mayPlace(std::shared_ptr<ItemInstance> item) override;
    int getMaxStackSize() override;
    int getNoItemIcon() override;
};
