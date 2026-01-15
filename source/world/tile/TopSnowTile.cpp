/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TopSnowTile.hpp"
#include "world/level/Level.hpp"

TopSnowTile::TopSnowTile(int a, int b, Material* c) : Tile(a, b, c)
{
	setShape(0, 0, 0, 1, 0.125f, 1);
	setTicking(true);
}

AABB* TopSnowTile::getAABB(const Level*, const TilePos& pos)
{
	return nullptr;
}

bool TopSnowTile::isCubeShaped() const
{
	return false;
}

bool TopSnowTile::isSolidRender() const
{
	return false;
}

int TopSnowTile::getResource(int x, Random* random) const
{
	return Item::snowBall->m_itemID;
}

int TopSnowTile::getResourceCount(Random* random) const
{
	return 0;
}

void TopSnowTile::playerDestroy(Level* level, Player* player, const TilePos& pos, int)
{
	if (level->m_bIsOnline) return;
	float var7 = 0.7F;
	Vec3 itemPos;
	itemPos.x = pos.x + (level->m_random.nextFloat() * var7) + (1.0F - var7) * 0.5;
	itemPos.y = pos.y + (level->m_random.nextFloat() * var7) + (1.0F - var7) * 0.5;
	itemPos.z = pos.z + (level->m_random.nextFloat() * var7) + (1.0F - var7) * 0.5;
	auto item = std::make_shared<ItemEntity>(level, itemPos, std::make_shared<ItemInstance>(Item::snowBall->m_itemID, 1, 0));
	item->m_throwTime = 10;
	level->addEntity(item);
	level->setTile(pos, TILE_AIR);
	player->awardStat(Stats::statMineBlock[m_ID]);
}

bool TopSnowTile::mayPlace(const Level* level, const TilePos& pos) const
{
	TileID tile = level->getTile(pos.below());

	if (!tile || !Tile::tiles[tile]->isSolidRender())
		return false;

	return level->getMaterial(pos.below())->blocksMotion();
}

bool TopSnowTile::checkCanSurvive(Level* level, const TilePos& pos)
{
	if (mayPlace(level, pos))
		return true;

	spawnResources(level, pos, level->getData(pos));
	level->setTile(pos, TILE_AIR);
	return false;
}

void TopSnowTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
	checkCanSurvive(level, pos);
}

bool TopSnowTile::shouldRenderFace(const LevelSource* level, const TilePos& pos, Facing::Name face) const
{
	if (face == Facing::UP)
		return true;

	Material* material = level->getMaterial(pos);

	return material != m_pMaterial && Tile::shouldRenderFace(level, pos, face);
}

void TopSnowTile::tick(Level* level, const TilePos& pos, Random* random)
{
	if (level->getBrightness(LightLayer::Block, pos) > 11)
	{
		spawnResources(level, pos, level->getData(pos));
		level->setTile(pos, TILE_AIR);
	}
}
