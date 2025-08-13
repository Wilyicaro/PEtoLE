#pragma once

#include "Item.hpp"

class SnowballItem : public Item
{
public:
	SnowballItem(int id);

	std::shared_ptr<ItemInstance> use(std::shared_ptr<ItemInstance>, Level*, Player*) override;

	virtual std::shared_ptr<Entity> createProjectile(Level*, std::shared_ptr<Mob>);
};
