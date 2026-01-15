/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ShearsItem.hpp"
#include <world/entity/Sheep.hpp>
#include <world/entity/Player.hpp>

ShearsItem::ShearsItem(int id) : Item(id)
{
	m_maxStackSize = 1;
	m_maxDamage = 238;
}

bool ShearsItem::mineBlock(ItemInstance* item, int tile, const TilePos& pos, Facing::Name face, Player* player)
{
	if (tile == Tile::leaves->m_ID || tile == Tile::web->m_ID)
	{
		item->hurtAndBreak(1, player);
	}

	return Item::mineBlock(item, tile, pos, face, player);
}

bool ShearsItem::canDestroySpecial(const Tile* tile)
{
	return tile->m_ID == Tile::web->m_ID;
}

float ShearsItem::getDestroySpeed(ItemInstance* item, const Tile* tile)
{
	return tile->m_ID != Tile::web->m_ID && tile->m_ID != Tile::leaves->m_ID ? (tile->m_ID == Tile::cloth->m_ID ? 5.0F : Item::getDestroySpeed(item, tile)) : 15.0F;
}
