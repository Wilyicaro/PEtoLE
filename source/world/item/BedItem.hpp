#pragma once

#include "Item.hpp"

class BedItem : public Item
{
public:
	BedItem(int id);

	virtual bool useOn(ItemInstance*, Player*, Level*, const TilePos& pos, Facing::Name face);
};
