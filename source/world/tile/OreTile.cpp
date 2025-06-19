/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "OreTile.hpp"
#include "world/level/Level.hpp"

OreTile::OreTile(int id, int texture) : Tile(id, texture, Material::stone)
{

}

int OreTile::getResource(int x, Random* random) const
{
	if (m_ID == Tile::coalOre->m_ID)
	{
		return Item::coal->m_itemID;
	}
	else if (m_ID == Tile::diamondOre->m_ID)
	{
		return Item::diamond->m_itemID;
	}
	else
	{
		return m_ID == Tile::lapisOre->m_ID ? Item::dyePowder->m_itemID : m_ID;
	}
}

int OreTile::getResourceCount(Random* random) const
{
	return m_ID == Tile::lapisOre->m_ID ? 4 + random->nextInt(5) : 1;
}

int OreTile::getSpawnResourcesAuxValue(int x) const
{
	return m_ID == Tile::lapisOre->m_ID ? 4 : 0;
}
