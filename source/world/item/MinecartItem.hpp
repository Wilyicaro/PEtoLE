#pragma once

#include "Item.hpp"
#include <world/entity/Minecart.hpp>

class MinecartItem : public Item
{
public:
	MinecartItem(int id, Minecart::Type type);

	bool useOn(ItemInstance* instance, Player* player, Level* level, const TilePos& pos, Facing::Name face) override;

public:
	Minecart::Type m_type;
};
