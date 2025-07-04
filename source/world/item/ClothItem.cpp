/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ClothItem.hpp"
#include "world/level/Level.hpp"
#include "world/entity/Player.hpp"
#include "world/tile/Tile.hpp"
#include "DyeColor.hpp"
#include <world/tile/ClothTile.hpp>

ClothItem::ClothItem(int id) : TileItem(id)
{
	m_maxDamage = 0;
	m_bStackedByData = true;
}

std::string ClothItem::getDescriptionId(ItemInstance* item)
{
	return TileItem::getDescriptionId(item) + "." + DyeColor::IDS[ClothTile::getColorFromData(item->getAuxValue())];;
}

int ClothItem::getIcon(const ItemInstance* item) const
{
	return Tile::cloth->getTexture(Facing::NORTH, ClothTile::getColorFromData(item->getAuxValue()));
}

int ClothItem::getLevelDataForAuxValue(int x)
{
	return x;
}
