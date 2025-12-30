/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TntTile.hpp"
#include "world/level/Level.hpp"
#include "world/entity/PrimedTnt.hpp"

TntTile::TntTile(int id, int texture) : Tile(id, texture, Material::explosive)
{

}

int TntTile::getResourceCount(Random* random) const
{
	return 0;
}

int TntTile::getTexture(Facing::Name face) const
{
	switch (face)
	{
	case Facing::DOWN:
		return m_TextureFrame + 2;
	case Facing::UP:
		return m_TextureFrame + 1;
	}

	return m_TextureFrame;
}

void TntTile::destroy(Level* level, const TilePos& pos, int data)
{
	// prevent players from using this in multiplayer, to prevent a desync of player IDs
	if (level->m_bIsOnline) return;

	if ((data & 1) == 0)
		spawnResources(level, pos, std::make_shared<ItemInstance>(Tile::tnt->m_ID, 1, 0));
	else
	{
		auto tnt = std::make_shared<PrimedTnt>(level, Vec3(pos.x + 0.5, pos.y, pos.z + 0.5));
		level->addEntity(tnt);
		level->playSound(tnt.get(), "random.fuse", 1.0F, 1.0F);
	}
}

void TntTile::onPlace(Level* level, const TilePos& pos)
{
	Tile::onPlace(level, pos);
	if (level->hasNeighborSignal(pos))
	{
		destroy(level, pos, 1);
		level->setTile(pos, 0);
	}
}

void TntTile::wasExploded(Level* level, const TilePos& pos)
{
	auto tnt = std::make_shared<PrimedTnt>(level, Vec3(pos.x + 0.5, pos.y, pos.z + 0.5));
	tnt->m_fuseTimer = level->m_random.nextInt(tnt->m_fuseTimer / 4) + tnt->m_fuseTimer / 8;
	level->addEntity(tnt);
}

void TntTile::attack(Level* level, const TilePos& pos, Player* player)
{
	if (player->getSelectedItem() && player->getSelectedItem()->m_itemID == Item::flintAndSteel->m_itemID)
		level->setDataNoUpdate(pos, 1);

	Tile::attack(level, pos, player);
}

void TntTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
	if (tile > 0 && Tile::tiles[tile]->isSignalSource() && level->hasNeighborSignal(pos))
	{
		destroy(level, pos, 1);
		level->setTile(pos, TILE_AIR);
	}
}
