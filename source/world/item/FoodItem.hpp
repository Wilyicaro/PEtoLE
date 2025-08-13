#pragma once

#include "Item.hpp"

class FoodItem : public Item
{
public:
	FoodItem(int id, int nutrition, bool isWolfFood = false);

	virtual std::shared_ptr<ItemInstance> use(std::shared_ptr<ItemInstance>, Level*, Player*) override;
	virtual bool isFood() const override;
	virtual bool isWolfFood() const override;

	int getNutrition() const;

protected:
	int m_nutrition;
	bool m_bIsWolfFood;
};
