#include "SaddleItem.hpp"
#include "world/entity/Mob.hpp"
#include <world/entity/Pig.hpp>

SaddleItem::SaddleItem(int id) : Item(id)
{
	m_maxStackSize = 1;
	m_maxDamage = 64;
}

void SaddleItem::interactEnemy(ItemInstance* instance, Mob* mob)
{
    if (mob->getType() == EntityType::pig) {
        Pig* pig = (Pig*)mob;
        if (!pig->hasSaddle()) {
            pig->setSaddle(true);
            --instance->m_count;
        }
    }

}

void SaddleItem::hurtEnemy(ItemInstance* instance, Mob* mob)
{
	interactEnemy(instance, mob);
}

