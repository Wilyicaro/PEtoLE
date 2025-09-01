#include "BowlFoodItem.hpp"

BowlFoodItem::BowlFoodItem(int id, int nutrition, bool isWolfFood) : FoodItem(id, nutrition, isWolfFood)
{
}

std::shared_ptr<ItemInstance> BowlFoodItem::use(std::shared_ptr<ItemInstance> item, Level* level, Player* player)
{
    FoodItem::use(item, level, player);
    return std::make_shared<ItemInstance>(Item::bowl);
}