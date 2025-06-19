#pragma once

#include "DiggerItem.hpp"

class PickaxeItem : public DiggerItem
{
public:
	PickaxeItem(int id, Tier& tier);

	bool canDestroySpecial(const Tile*) override;
};
