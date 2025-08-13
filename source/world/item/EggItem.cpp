#include "EggItem.hpp"
#include <world/entity/projectile/ThrownEgg.hpp>

EggItem::EggItem(int id) : SnowballItem(id)
{
}

std::shared_ptr<Entity> EggItem::createProjectile(Level* level, std::shared_ptr<Mob> mob)
{
	return std::make_shared<ThrownEgg>(level, mob);
}
