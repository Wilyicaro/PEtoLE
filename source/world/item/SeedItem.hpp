#pragma once

#include "Item.hpp"

class SeedItem : public Item
{
public:
	SeedItem(int id, int place);

	virtual bool useOn(ItemInstance*, Player*, Level*, const TilePos& pos, Facing::Name face);

public:
	int m_tile;
};
