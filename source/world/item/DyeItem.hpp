/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include "Item.hpp"

class DyeItem : public Item
{
public:
	DyeItem(int id);
	std::string getDescriptionId(ItemInstance* item) override;
	int getIcon(const ItemInstance*) const override;
	void interactEnemy(ItemInstance* var1, Mob* var2) override;
	bool useOn(ItemInstance*, Player*, Level*, const TilePos& pos, Facing::Name face) override;
};
