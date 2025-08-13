#pragma once

#include "Item.hpp"

class MinecartItem : public Item
{
public:
	MinecartItem(int id, int type);

	bool useOn(ItemInstance* instance, Player* player, Level* level, const TilePos& pos, Facing::Name face) override;

public:
	int m_type;
};
