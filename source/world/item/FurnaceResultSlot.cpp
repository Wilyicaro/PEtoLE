#include "FurnaceResultSlot.hpp"
#include "../item/Item.hpp"
#include "world/entity/Player.hpp"
#include "stats/Achievement.hpp"

FurnaceResultSlot::FurnaceResultSlot(Player* player, Container* container, int slotIndex, int x, int y) : Slot(container, slotIndex, x, y), m_pPlayer(player) {}

bool FurnaceResultSlot::mayPlace(std::shared_ptr<ItemInstance>  item) {
    return false;
}

void FurnaceResultSlot::onTake(std::shared_ptr<ItemInstance> item) {
    item->onCraftedBy(m_pPlayer, m_pPlayer->m_pLevel);
	if (item->m_itemID == Item::ironIngot->m_itemID)
		m_pPlayer->awardStat(Achievements::acquireIron);
	else if (item->m_itemID == Item::cookedFish->m_itemID)
		m_pPlayer->awardStat(Achievements::cookFish);

	Slot::onTake(item);
}
