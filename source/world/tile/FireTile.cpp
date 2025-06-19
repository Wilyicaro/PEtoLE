/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "FireTile.hpp"
#include "world/level/Level.hpp"

FireTile::FireTile(int ID, int texture) : Tile(ID, texture, Material::fire)
{
	// @BUG: Uninitialized memory can cause the odds of normally inflammable items to go
	// out of reasonable limits, therefore causing the bug demonstrated in the following video:
	// https://www.youtube.com/watch?v=3hrz7KK2EJs
#ifndef ORIGINAL_CODE
	memset(m_igniteOdds, 0, sizeof m_igniteOdds);
	memset(m_burnOdds, 0, sizeof m_burnOdds);
#endif

	m_igniteOdds[Tile::wood->m_ID]      = 5;
	m_burnOdds  [Tile::wood->m_ID]      = 20;
	m_igniteOdds[Tile::treeTrunk->m_ID] = 5;
	m_burnOdds  [Tile::treeTrunk->m_ID] = 5;
	m_igniteOdds[Tile::leaves->m_ID]    = 30;
	m_burnOdds  [Tile::leaves->m_ID]    = 60;
	m_igniteOdds[Tile::tnt->m_ID]       = 15;
	m_burnOdds  [Tile::tnt->m_ID]       = 100;
	m_igniteOdds[Tile::cloth->m_ID]     = 30;
	m_burnOdds  [Tile::cloth->m_ID]     = 60;
	// @NOTE: Not setting the other cloths' properties

	setTicking(true);
}

int FireTile::getRenderShape() const
{
	// @BUG: Since the shape is set to FIRE, but TileRenderer doesn't handle it,
	// fire is invisible in this version of Minecraft.
	return SHAPE_FIRE;
}

bool FireTile::isSolidRender() const
{
	return false;
}

bool FireTile::isCubeShaped() const
{
	return false;
}

AABB* FireTile::getAABB(const Level* level, const TilePos& pos)
{
	return nullptr;
}

int FireTile::getResourceCount(Random* random) const
{
	return 0;
}

int FireTile::getTickDelay() const
{
	return 10;
}

void FireTile::animateTick(Level* level, const TilePos& pos, Random* random)
{
	if (random->nextInt(24) == 0) {
		level->playSound(pos.center(), "fire.fire", 1.0F + random->nextFloat(), random->nextFloat() * 0.7F + 0.3F);
	}

	FireTile* pFireTile = (FireTile*)Tile::fire;
	if (level->isSolidTile(pos.below()) || pFireTile->canBurn(level, pos.below()))
	{
		for (int i = 0; i < 3; i++)
		{
			level->addParticle("largesmoke", Vec3(float(pos.x) + random->nextFloat(), float(pos.y) + random->nextFloat() * 0.5f + 0.5f, float(pos.z) + random->nextFloat()));
		}

		return;
	}

	if (pFireTile->canBurn(level, pos.west()))
		level->addParticle("largesmoke", Vec3(float(pos.x) + random->nextFloat() * 0.1f, float(pos.y) + random->nextFloat(), float(pos.z) + random->nextFloat()));
	if (pFireTile->canBurn(level, pos.east()))
		level->addParticle("largesmoke", Vec3(float(pos.x + 1) - random->nextFloat() * 0.1f, float(pos.y) + random->nextFloat(), float(pos.z) + random->nextFloat()));
	if (pFireTile->canBurn(level, pos.north()))
		level->addParticle("largesmoke", Vec3(float(pos.x) + random->nextFloat(), float(pos.y) + random->nextFloat(), float(pos.z) + random->nextFloat() * 0.1f));
	if (pFireTile->canBurn(level, pos.south()))
		level->addParticle("largesmoke", Vec3(float(pos.x) + random->nextFloat(), float(pos.y) + random->nextFloat(), float(pos.z + 1) - random->nextFloat() * 0.1f));
	if (pFireTile->canBurn(level, pos.above()))
		level->addParticle("largesmoke", Vec3(float(pos.x) + random->nextFloat(), float(pos.y + 1) - random->nextFloat() * 0.1f, float(pos.z) + random->nextFloat()));
}

void FireTile::checkBurn(Level* level, const TilePos& pos, int thing, Random* random)
{
	if (m_burnOdds[level->getTile(pos)] > int(random->nextInt(thing)))
	{
		TileID tid = level->getTile(pos);

		TileID newTile = m_ID;
		if (random->nextInt(2))
			newTile = TILE_AIR;

		level->setTile(pos, newTile);
		if (tid == Tile::tnt->m_ID)
			Tile::tnt->destroy(level, pos, 0);
	}
}

int FireTile::getFireOdds(const Level* level, const TilePos& pos)
{
	if (!level->isEmptyTile(pos))
		return 0;

	int odds = m_igniteOdds[level->getTile(pos.east())], o;
	if (odds < 0)
		odds = 0;

	o = m_igniteOdds[level->getTile(pos.west())];
	if (odds < o) odds = o;
	o = m_igniteOdds[level->getTile(pos.below())];
	if (odds < o) odds = o;
	o = m_igniteOdds[level->getTile(pos.above())];
	if (odds < o) odds = o;
	o = m_igniteOdds[level->getTile(pos.north())];
	if (odds < o) odds = o;
	o = m_igniteOdds[level->getTile(pos.south())];
	if (odds < o) odds = o;

	return odds;
}

bool FireTile::isValidFireLocation(const Level* level, const TilePos& pos) const
{
	if (canBurn(level, pos.east())) return true;
	if (canBurn(level, pos.west())) return true;
	if (canBurn(level, pos.below())) return true;
	if (canBurn(level, pos.above())) return true;
	if (canBurn(level, pos.north())) return true;
	if (canBurn(level, pos.south())) return true;
	return false;
}

bool FireTile::mayPick() const
{
	return false;
}

bool FireTile::mayPlace(const Level* level, const TilePos& pos) const
{
	// @NOTE: This is useless as you usually don't 'place' fire.
	if (level->isSolidTile(pos.below()))
		return true;

	return isValidFireLocation(level, pos);
}

void FireTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
	if (!level->isSolidTile(pos.below()) && !isValidFireLocation(level, pos))
		level->setTile(pos, TILE_AIR);
}

void FireTile::onPlace(Level* level, const TilePos& pos)
{
	// @NOTE: Unused return result
	level->getTile(pos.below());

	if (!level->isSolidTile(pos.below()) && !isValidFireLocation(level, pos))
	{
		level->setTile(pos, TILE_AIR);
		return;
	}

	level->addToTickNextTick(pos, m_ID, getTickDelay());
}

void FireTile::tick(Level* level, const TilePos& pos, Random* random)
{
	bool var6 = level->getTile(pos.below()) == Tile::netherrack->m_ID;
	int var7 = level->getData(pos);
	if (var7 < 15) {
		level->setData(pos, var7 + 1);
		level->addToTickNextTick(pos, m_ID, getTickDelay());
	}

	if (!var6 && !isValidFireLocation(level, pos)) {
		if (!level->isSolidTile(pos.below()) || var7 > 3) {
			level->setTile(pos, 0);
		}

	}
	else if (!var6 && !canBurn(level, pos.below()) && var7 == 15 && random->nextInt(4) == 0) {
		level->setTile(pos, 0);
	}
	else {
		if (var7 % 2 == 0 && var7 > 2) {
			checkBurn(level, pos.east(), 300, random);
			checkBurn(level, pos.west(), 300, random);
			checkBurn(level, pos.below(), 250, random);
			checkBurn(level, pos.above(), 250, random);
			checkBurn(level, pos.north(), 300, random);
			checkBurn(level, pos.south(), 300, random);

			for (int var8 = pos.x - 1; var8 <= pos.x + 1; ++var8) {
				for (int var9 = pos.z - 1; var9 <= pos.z + 1; ++var9) {
					for (int var10 = pos.y - 1; var10 <= pos.y + 4; ++var10) {
						if (var8 != pos.x || var10 != pos.y || var9 != pos.z) {
							int var11 = 100;
							if (var10 > pos.y + 1) {
								var11 += (var10 - (pos.y + 1)) * 100;
							}

							int var12 = getFireOdds(level, pos);
							if (var12 > 0 && random->nextInt(var11) <= var12) {
								level->setTile(pos, m_ID);
							}
						}
					}
				}
			}
		}

	}
}
