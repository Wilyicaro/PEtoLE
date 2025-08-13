#include "FoodItem.hpp"
#include "world/entity/Player.hpp"

FoodItem::FoodItem(int id, int nutrition, bool isWolfFood) : Item(id), m_bIsWolfFood(isWolfFood)
{
	m_maxStackSize = 1;
	m_nutrition = nutrition;
}

std::shared_ptr<ItemInstance> FoodItem::use(std::shared_ptr<ItemInstance> item, Level* level, Player* player)
{
    if (player->m_health < 20)
    {
        --item->m_count;
        player->heal(m_nutrition);
    }
    return item;
}

bool FoodItem::isFood() const
{
    return true;
}

bool FoodItem::isWolfFood() const
{
    return m_bIsWolfFood;
}

int FoodItem::getNutrition() const
{
    return m_nutrition;
}

