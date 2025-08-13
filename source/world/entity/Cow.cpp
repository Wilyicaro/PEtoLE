/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "Cow.hpp"
#include "common/Utils.hpp"
#include "world/entity/Player.hpp"

Cow::Cow(Level* pLevel) : Animal(pLevel)
{
	m_pType = EntityType::cow;
	m_texture = "mob/cow.png";
	setSize(0.9f, 1.3f);
}

bool Cow::interact(Player* player)
{
	auto var2 = player->m_pInventory->getSelected();
	if (var2 && var2->m_itemID == Item::emptyBucket->m_itemID) {
		player->m_pInventory->setSelectedItem(std::make_shared<ItemInstance>(Item::milk));
		return true;
	}
	else {
		return false;
	}
}
