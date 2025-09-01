#pragma once

#include "FoodItem.hpp"

class BowlFoodItem : public FoodItem
{
public:
	BowlFoodItem(int id, int nutrition, bool isWolfFood = false);

	virtual std::shared_ptr<ItemInstance> use(std::shared_ptr<ItemInstance>, Level*, Player*) override;
};
