#pragma once

#include "SnowballItem.hpp"

class EggItem : public SnowballItem
{
public:
	EggItem(int id);

	std::shared_ptr<Entity> createProjectile(Level*, std::shared_ptr<Mob>) override;
};
