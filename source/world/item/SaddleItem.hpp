#pragma once

#include "Item.hpp"

class SaddleItem : public Item
{
public:
	SaddleItem(int id);
	virtual void interactEnemy(ItemInstance* instance, Mob* mob);
	virtual bool hurtEnemy(ItemInstance* instance, Mob* mob, Player* player);
};
