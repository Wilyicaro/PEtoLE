#pragma once

#include "Item.hpp"

class FlintAndSteelItem : public Item
{
public:
	FlintAndSteelItem(int id);

	virtual bool useOn(ItemInstance*, Player*, Level*, const TilePos& pos, Facing::Name face);
};
