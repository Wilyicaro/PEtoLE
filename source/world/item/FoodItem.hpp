#pragma once

#include "Item.hpp"

class FoodItem : public Item
{
public:
	FoodItem(int id, int nutrition);
	virtual std::shared_ptr<ItemInstance> use(std::shared_ptr<ItemInstance>, Level*, Player*);
protected:
	int m_nutrition;
};
