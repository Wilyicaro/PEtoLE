#include "ResultSlot.hpp"
#include "../item/Item.hpp"
#include "world/entity/Player.hpp"
#include "stats/Achievement.hpp"

ResultSlot::ResultSlot(Player* player, Container* craftSlots, Container* resultSlots, int slotIndex, int x, int y)
    : Slot(resultSlots, slotIndex, x, y), m_pPlayer(player), craftSlots(craftSlots) {}

bool ResultSlot::mayPlace(std::shared_ptr<ItemInstance>  item)
{
    return false;
}

bool ResultSlot::canSync()
{
    return false;
}

void ResultSlot::onTake(std::shared_ptr<ItemInstance> item)
{
    item->onCraftedBy(m_pPlayer, m_pPlayer->m_pLevel);

	if (item->m_itemID == Tile::craftingTable->m_ID)
		m_pPlayer->awardStat(Achievements::buildWorkBench);
	else if (item->m_itemID == Item::woodPickaxe->m_itemID)
		m_pPlayer->awardStat(Achievements::buildPickaxe);
	else if (item->m_itemID == Tile::furnace->m_ID)
		m_pPlayer->awardStat(Achievements::buildFurnace);
	else if (item->m_itemID == Item::woodHoe->m_itemID)
		m_pPlayer->awardStat(Achievements::buildHoe);
	else if (item->m_itemID == Item::bread->m_itemID)
		m_pPlayer->awardStat(Achievements::makeBread);
	else if (item->m_itemID == Item::cake->m_itemID)
		m_pPlayer->awardStat(Achievements::bakeCake);
	else if (item->m_itemID == Item::stonePickaxe->m_itemID)
		m_pPlayer->awardStat(Achievements::buildBetterPickaxe);
	else if (item->m_itemID == Item::woodSword->m_itemID)
		m_pPlayer->awardStat(Achievements::buildSword);

    for (int i = 0; i < craftSlots->getContainerSize(); ++i)
	{
        auto instance = craftSlots->getItem(i);
        if (instance)
		{
            craftSlots->removeItem(i, 1);
            Item* item = instance->getItem();
            if (item->hasCraftingRemainingItem())
                craftSlots->setItem(i, std::make_shared<ItemInstance>(item->getCraftingRemainingItem()));
        }
    }
}
