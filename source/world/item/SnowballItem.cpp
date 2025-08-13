#include "SnowballItem.hpp"
#include "world/level/Level.hpp"
#include <world/entity/projectile/Snowball.hpp>

SnowballItem::SnowballItem(int id) : Item(id)
{
	m_maxStackSize = 16;
}

std::shared_ptr<ItemInstance> SnowballItem::use(std::shared_ptr<ItemInstance> item, Level* level, Player* player)
{
    --item->m_count;
    level->playSound(player, "random.bow", 0.5F, 0.4F / (itemRand.nextFloat() * 0.4F + 0.8F));
    if (!level->m_bIsOnline)
        level->addEntity(createProjectile(level, std::dynamic_pointer_cast<Mob>(player->shared_from_this())));

    return item;
}

std::shared_ptr<Entity> SnowballItem::createProjectile(Level* level, std::shared_ptr<Mob> mob)
{
	return std::make_shared<Snowball>(level, mob);
}
