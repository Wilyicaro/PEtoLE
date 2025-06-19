#include "FoodItem.hpp"
#include "world/entity/Player.hpp"

FoodItem::FoodItem(int id, int nutrition) : Item(id)
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

