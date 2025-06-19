#pragma once

#include "Item.hpp"

class HoeItem : public Item
{
public:
	HoeItem(int id, Tier& tier);

	virtual bool useOn(ItemInstance*, Player*, Level*, const TilePos& pos, Facing::Name face) override;
	virtual bool isHandEquipped() override;
};
