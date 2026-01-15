/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LeafTile.hpp"
#include "Sapling.hpp"
#include "world/level/Level.hpp"
#include "client/renderer/PatchManager.hpp"
#include <client/renderer/FoliageColor.hpp>

LeafTile::LeafTile(int id) : TransparentTile(id, TEXTURE_LEAVES_TRANSPARENT, Material::leaves, false)
{
	m_checkBuffer = nullptr;

	m_oTex = m_TextureFrame;

	setTicking(true);
}

LeafTile::~LeafTile()
{
	if (m_checkBuffer)
		delete[] m_checkBuffer;
}

void LeafTile::die(Level* level, const TilePos& pos)
{
	spawnResources(level, pos, level->getData(pos));
	level->setTile(pos, TILE_AIR);
}

int LeafTile::getColor(const LevelSource* levelSource, const TilePos& pos, Facing::Name facing, int texture) const
{
	int data = levelSource->getData(pos);

	if ((data & 1) == 1)
	{
		return FoliageColor::getEvergreenColor();
	}
	if ((data & 2) == 2)
	{
		return FoliageColor::getBirchColor();
	}

	levelSource->getBiomeSource()->getBiome(pos);
	return FoliageColor::get(levelSource->getBiomeSource()->m_temperatures[0], levelSource->getBiomeSource()->m_downfalls[0]);
}

int LeafTile::getColor(int data, Facing::Name facing, int texture) const
{
	if ((data & 1) == 1)
	{
		return FoliageColor::getEvergreenColor();
	}
	if ((data & 2) == 2)
	{
		return FoliageColor::getBirchColor();
	}
	return FoliageColor::getDefaultColor();
}

int LeafTile::getTexture(Facing::Name face, int data) const
{
	if ((data & 3) == 1)
		return m_TextureFrame + 80;

	return m_TextureFrame;
}

bool LeafTile::isSolidRender() const
{
	return !m_bTransparent;
}

void LeafTile::stepOn(Level* level, const TilePos& pos, Entity* entity)
{
}

void LeafTile::onRemove(Level* level, const TilePos& pos)
{
	if (!level->hasChunksAt(pos - 2, pos + 2))
		return;

	TilePos o(-1, -1, -1);
	for (o.x = -1; o.x < 2; o.x++)
	{
		for (o.y = -1; o.y < 2; o.y++)
		{
			for (o.z = -1; o.z < 2; o.z++)
			{
				TileID tile = level->getTile(pos + o);
				if (tile != Tile::leaves->m_ID) continue;

				level->setDataNoUpdate(pos + o, level->getData(pos + o) | 8);
			}
		}
	}
}

void LeafTile::tick(Level* level, const TilePos& pos, Random* random)
{
	if (level->m_bIsOnline)
		return;

	int data = level->getData(pos);
	if ((data & 8) == 0)
		return;

	constexpr int C_RANGE = 32;
	constexpr int C_RANGE_SMALL = 4;
	constexpr int k1 = C_RANGE / 2;
	constexpr int j1 = C_RANGE * C_RANGE;

	if (!m_checkBuffer)
		m_checkBuffer = new int[C_RANGE * C_RANGE * C_RANGE];

	if (level->hasChunksAt(pos - 5, pos + 5))
	{
		TilePos curr(pos);
		for (int i2 = -C_RANGE_SMALL; i2 <= C_RANGE_SMALL; i2++)
		{
			curr.x = pos.x + i2;
			for (int j = -C_RANGE_SMALL; j <= C_RANGE_SMALL; j++)
			{
				curr.y = pos.y + j;
				for (int k = -C_RANGE_SMALL; k <= C_RANGE_SMALL; k++)
				{
					curr.z = pos.z + k;
					TileID tile = level->getTile(curr);
					m_checkBuffer[(i2 + k1) * j1 + (j + k1) * C_RANGE + k + k1] = tile == Tile::treeTrunk->m_ID ? 0 : tile == Tile::leaves->m_ID ? -2 : -1;
				}
			}
		}

		for (int i2 = 1; i2 <= C_RANGE_SMALL; i2++)
		{
			for (int l2 = -C_RANGE_SMALL; l2 <= C_RANGE_SMALL; l2++)
			{
				for (int j3 = -C_RANGE_SMALL; j3 <= C_RANGE_SMALL; j3++)
				{
					for (int l3 = -C_RANGE_SMALL; l3 <= C_RANGE_SMALL; l3++)
					{
						if (m_checkBuffer[(l2 + k1) * j1 + (j3 + k1) * C_RANGE + (l3 + k1)] != i2 - 1)
							continue;

						if (m_checkBuffer[((l2 + k1) - 1) * j1 + (j3 + k1) * C_RANGE + (l3 + k1)] == -2)
							m_checkBuffer[((l2 + k1) - 1) * j1 + (j3 + k1) * C_RANGE + (l3 + k1)] = i2;

						if (m_checkBuffer[(l2 + k1 + 1) * j1 + (j3 + k1) * C_RANGE + (l3 + k1)] == -2)
							m_checkBuffer[(l2 + k1 + 1) * j1 + (j3 + k1) * C_RANGE + (l3 + k1)] = i2;

						if (m_checkBuffer[(l2 + k1) * j1 + ((j3 + k1) - 1) * C_RANGE + (l3 + k1)] == -2)
							m_checkBuffer[(l2 + k1) * j1 + ((j3 + k1) - 1) * C_RANGE + (l3 + k1)] = i2;

						if (m_checkBuffer[(l2 + k1) * j1 + (j3 + k1 + 1) * C_RANGE + (l3 + k1)] == -2)
							m_checkBuffer[(l2 + k1) * j1 + (j3 + k1 + 1) * C_RANGE + (l3 + k1)] = i2;

						if (m_checkBuffer[(l2 + k1) * j1 + (j3 + k1) * C_RANGE + ((l3 + k1) - 1)] == -2)
							m_checkBuffer[(l2 + k1) * j1 + (j3 + k1) * C_RANGE + ((l3 + k1) - 1)] = i2;

						if (m_checkBuffer[(l2 + k1) * j1 + (j3 + k1) * C_RANGE + (l3 + k1 + 1)] == -2)
							m_checkBuffer[(l2 + k1) * j1 + (j3 + k1) * C_RANGE + (l3 + k1 + 1)] = i2;
					}
				}
			}
		}
	}

	if (m_checkBuffer[k1 * j1 + k1 * C_RANGE + k1] < 0)
		die(level, pos);
	else
		level->setDataNoUpdate(pos, data & -9);
}

#ifdef ENH_b1_7
void LeafTile::playerDestroy(Level* level, Player* player, const TilePos& pos, int meta)
{
	if (!level->m_bIsOnline && player->getSelectedItem() && player->getSelectedItem()->m_itemID == Item::shears->m_itemID)
	{
		player->awardStat(Stats::statMineBlock[m_ID]);
		spawnResources(level, pos, std::make_shared<ItemInstance>(Tile::leaves->m_ID, 1, meta & 3));
	}
	else
		TransparentTile::playerDestroy(level, player, pos, meta);
}
#endif

int LeafTile::getResource(int x, Random* random) const
{
	return random->nextInt(20) == 0 ? Tile::sapling->m_ID : 0;
}

int LeafTile::getSpawnResourcesAuxValue(int x) const
{
	return x & 3;
}
