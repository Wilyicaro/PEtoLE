/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "DyeItem.hpp"
#include "DyeColor.hpp"
#include "world/level/Level.hpp"
#include "world/entity/Player.hpp"
#include "world/tile/Tile.hpp"
#include <world/tile/Sapling.hpp>
#include <world/tile/CropsTile.hpp>
#include <world/entity/Sheep.hpp>
#include <world/tile/ClothTile.hpp>


DyeItem::DyeItem(int id) : Item(id)
{
	m_bStackedByData = true;
	m_maxDamage = 0;
}

std::string DyeItem::getDescriptionId(ItemInstance* item)
{
	return Item::getDescriptionId(item) + "." + DyeColor::IDS[item->getAuxValue()];
}

int DyeItem::getIcon(const ItemInstance* item) const
{
	return m_icon + item->getAuxValue() % 8 * 16 + item->getAuxValue() / 8;
}

bool DyeItem::useOn(ItemInstance* item, Player* player, Level* level, const TilePos& pos, Facing::Name face)
{
	if (item->getAuxValue() == 15)
	{
		TileID var8 = level->getTile(pos);
		if (var8 == Tile::sapling->m_ID)
		{
			if (!level->m_bIsOnline)
			{
				Tile::sapling->growTree(level, pos, &level->m_random);
				--item->m_count;
			}

			return true;
		}

		if (var8 == Tile::crops->m_ID)
		{
			if (!level->m_bIsOnline)
			{
				((CropsTile*)Tile::crops)->fertilize(level, pos);
				--item->m_count;
			}
		
			return true;
		}

		if (var8 == Tile::grass->m_ID)
		{
			if (!level->m_bIsOnline)
			{
				--item->m_count;


				for (int var9 = 0; var9 < 128; ++var9)
				{
					TilePos tp = pos.above();

					for (int var13 = 0; var13 < var9 / 16; ++var13)
					{
						tp.x += itemRand.nextInt(3) - 1;
						tp.y += (itemRand.nextInt(3) - 1) * itemRand.nextInt(3) / 2;
						tp.z += itemRand.nextInt(3) - 1;
						if (level->getTile(tp.below()) != Tile::grass->m_ID || level->isSolidTile(tp)) {
							goto label53;
						}
					}

					if (level->getTile(tp) == TILE_AIR)
					{
						if (itemRand.nextInt(10) != 0)
							level->setTileAndData(tp, Tile::tallGrass->m_ID, 1);
						else if (itemRand.nextInt(3) != 0)
							level->setTile(tp, Tile::flower->m_ID);
						else
							level->setTile(tp, Tile::rose->m_ID);
					}
				label53:;
				}
			}

			return true;
		}
	}
	return false;
}

void DyeItem::interactEnemy(ItemInstance* var1, Mob* var2)
{
	if (var2->getType() == EntityType::sheep)
	{
		Sheep* sheep = (Sheep*)var2;
		int var4 = ClothTile::getColorFromData(var1->getAuxValue());
		if (!sheep->isSheared() && sheep->getColor() != var4)
		{
			sheep->setColor(var4);
			--var1->m_count;
		}
	}

}
