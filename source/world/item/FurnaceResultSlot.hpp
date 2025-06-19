#pragma once

#include "Slot.hpp"
#include "Container.hpp"

class FurnaceResultSlot : public Slot {

public:
    FurnaceResultSlot(std::shared_ptr<Player> player, Container* container, int slotIndex, int x, int y);

    bool mayPlace(std::shared_ptr<ItemInstance> item) override;
    void onTake(std::shared_ptr<ItemInstance>) override;

private:
    std::shared_ptr<Player> m_player;
};
