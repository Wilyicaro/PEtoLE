#pragma once

#include "Slot.hpp"
#include "Container.hpp"

class ResultSlot : public Slot {
private:
    Container* craftSlots;
    Player* m_pPlayer;

public:
    ResultSlot(Player* player, Container* craftSlots, Container* resultSlots, int slotIndex, int x, int y);

    bool mayPlace(std::shared_ptr<ItemInstance> item) override;
    bool canSync() override;
    void onTake(std::shared_ptr<ItemInstance>) override;
};
