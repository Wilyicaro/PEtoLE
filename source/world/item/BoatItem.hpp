#pragma once

#include "Item.hpp"

class BoatItem : public Item
{
public:
	BoatItem(int id);

	std::shared_ptr<ItemInstance> use(std::shared_ptr<ItemInstance>, Level*, Player*) override;
};
