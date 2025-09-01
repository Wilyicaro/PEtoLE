#include "ResultSlot.hpp"
#include "../item/Item.hpp"
#include "world/entity/Player.hpp"

ResultSlot::ResultSlot(Player* player, Container* craftSlots, Container* resultSlots, int slotIndex, int x, int y)
    : Slot(resultSlots, slotIndex, x, y), m_player(player), craftSlots(craftSlots) {}

bool ResultSlot::mayPlace(std::shared_ptr<ItemInstance>  item) {
    return false;
}

void ResultSlot::onTake(std::shared_ptr<ItemInstance> item) {
    item->onCraftedBy(m_player, m_player->m_pLevel);
    for (int i = 0; i < craftSlots->getContainerSize(); ++i) {
        auto instance = craftSlots->getItem(i);
        if (instance != nullptr) {
            craftSlots->removeItem(i, 1);
            Item* item = instance->getItem();
            if (item->hasCraftingRemainingItem()) {
                craftSlots->setItem(i, std::make_shared<ItemInstance>(item->getCraftingRemainingItem()));
            }
        }
    }
}
