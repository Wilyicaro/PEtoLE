#pragma once

#include "Item.hpp"

class PaintingItem : public Item
{
public:
	PaintingItem(int id);

	bool useOn(ItemInstance* instance, Player* player, Level* level, const TilePos& pos, Facing::Name face) override;
};
