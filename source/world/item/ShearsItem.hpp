#pragma once

#include "Item.hpp"

class ShearsItem : public Item
{
public:
	ShearsItem(int id);

	virtual void mineBlock(ItemInstance* instance, int tile, const TilePos& pos, Facing::Name face, Player*) override;
	virtual bool canDestroySpecial(const Tile*) override;
	virtual float getDestroySpeed(ItemInstance* instance, const Tile* tile) override;
};
