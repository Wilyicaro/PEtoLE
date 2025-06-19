#include "FurnaceResultSlot.hpp"
#include "../item/Item.hpp"
#include "world/entity/Player.hpp"

FurnaceResultSlot::FurnaceResultSlot(std::shared_ptr<Player> player, Container* container, int slotIndex, int x, int y) : Slot(container, slotIndex, x, y), m_player(player) {}

bool FurnaceResultSlot::mayPlace(std::shared_ptr<ItemInstance>  item) {
    return false;
}

void FurnaceResultSlot::onTake(std::shared_ptr<ItemInstance> item) {
    item->onCrafting(m_player.get(), m_player->m_pLevel);
    //@TODO Stats
}
