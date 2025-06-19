#pragma once

#include "DiggerItem.hpp"

class ShovelItem : public DiggerItem
{
public:
	ShovelItem(int id, Tier& tier);

	bool canDestroySpecial(const Tile*) override;
};
