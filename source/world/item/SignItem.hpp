#pragma once

#include "Item.hpp"

class SignItem : public Item
{
public:
	SignItem(int id);

	virtual bool useOn(ItemInstance*, Player*, Level*, const TilePos& pos, Facing::Name face);
};
