#pragma once

#include "Item.hpp"

class BowItem : public Item
{
public:
	BowItem(int id);

	virtual std::shared_ptr<ItemInstance> use(std::shared_ptr<ItemInstance>, Level*, Player*) override;
};
