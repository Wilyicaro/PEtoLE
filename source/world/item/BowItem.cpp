#include "BowItem.hpp"
#include "world/entity/Player.hpp"
#include "world/level/Level.hpp"
#include <world/entity/projectile/Arrow.hpp>

BowItem::BowItem(int id) : Item(id)
{
	m_maxStackSize = 1;
}

std::shared_ptr<ItemInstance> BowItem::use(std::shared_ptr<ItemInstance> item, Level* level, Player* player)
{
    if (player->isCreative() || player->m_pInventory->removeResource(Item::arrow->m_itemID)) {
        level->playSound(player, "random.bow", 1.0F, 1.0F / (itemRand.nextFloat() * 0.4F + 0.8F));
        if (!level->m_bIsOnline) {
            level->addEntity(std::make_shared<Arrow>(level, std::dynamic_pointer_cast<Mob>(player->shared_from_this())));
        }
    }

    return item;
}