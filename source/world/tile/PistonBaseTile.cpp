#include "PistonBaseTile.hpp"
#include "MovingPistonTile.hpp"
#include "PistonHeadTile.hpp"
#include "world/level/Level.hpp"
#include "entity/PistonMovingTileEntity.hpp"

PistonBaseTile::PistonBaseTile(int id, int texture, bool sticky) : Tile(id, texture, Material::piston), m_bIsSticky(sticky), m_bUpdating(false)
{
	setSoundType(Tile::SOUND_STONE);
	setDestroyTime(0.5f);
}

int PistonBaseTile::getTexture(Facing::Name side, int meta) const
{
	return (side == Facing::UP || getFacing(meta) > 5) ? m_TextureFrame : side == Facing::DOWN ? 109 : 108;
}

int PistonBaseTile::getTexture(const LevelSource* level, const TilePos& pos, Facing::Name side) const
{
	int meta = level->getData(pos);
	int facing = getFacing(meta);
	return facing > 5 ? m_TextureFrame : (side == facing ? (!isExtended(meta) && m_aabb.min.x <= 0.0 && m_aabb.min.y <= 0.0 && m_aabb.min.z <= 0.0 && m_aabb.max.x >= 1.0 && m_aabb.max.y >= 1.0 && m_aabb.max.z >= 1.0 ? m_TextureFrame : 110) : (side == Facing::OPPOSITE[facing] ? 109 : 108));
}

int PistonBaseTile::getRenderShape() const
{
	return SHAPE_PISTON;
}

void PistonBaseTile::triggerEvent(Level* level, const TilePos& pos, int b0, int b1)
{
	m_bUpdating = true;
	if (b0 == 0)
	{
		if (moveBlocks(level, pos, (Facing::Name) b1))
		{
			LOG_I("EXTENDING");
			level->setData(pos, b1 | 8);
			level->playSound(pos.center(), "tile.piston.out", 0.5F, level->m_random.nextFloat() * 0.25F + 0.6F);
		}
	}
	else if (b0 == 1)
	{
		LOG_I("BACKING");
		TilePos relative = pos.relative((Facing::Name)b1);
		auto piston = std::dynamic_pointer_cast<PistonMovingTileEntity>(level->getTileEntity(relative));
		if (piston)
			piston->finalTick();

		level->setTileAndDataNoUpdate(pos, Tile::movingPiston->m_ID, b1);
		level->setTileEntity(pos, MovingPistonTile::newMovingTileEntity(m_ID, b1, (Facing::Name)b1, false, true));
		if (m_bIsSticky)
		{
			TilePos tp = pos.relative((Facing::Name)b1, 2);
			TileID tile = level->getTile(tp);
			int data = level->getData(tp);
			bool flag = false;
			if (tile == Tile::movingPiston->m_ID)
			{
				auto otherPiston = std::dynamic_pointer_cast<PistonMovingTileEntity>(level->getTileEntity(tp));
				if (otherPiston && otherPiston->getDirection() == b1 && otherPiston->isExtending())
				{
					otherPiston->finalTick();
					tile = otherPiston->getTileId();
					data = otherPiston->getData();
					flag = true;
				}
			}

			if (flag || tile <= 0 || !isPushable(tile, level, tp, false) || Tile::tiles[tile]->getPistonPushReaction() != 0 && tile != Tile::piston->m_ID && tile != Tile::stickyPiston->m_ID) {
				if (!flag)
				{
					m_bUpdating = false;
					level->setTile(relative, TILE_AIR);
					m_bUpdating = true;
				}
			}
			else
			{
				m_bUpdating = false;
				level->setTile(tp, TILE_AIR);
				m_bUpdating = true;
				level->setTileAndDataNoUpdate(relative, Tile::movingPiston->m_ID, data);
				level->setTileEntity(relative, MovingPistonTile::newMovingTileEntity(tile, data, (Facing::Name)b1, false, false));
			}
		}
		else
		{
			m_bUpdating = false;
			level->setTile(relative, TILE_AIR);
			m_bUpdating = true;
		}

		level->playSound(pos.center(), "tile.piston.in", 0.5F, level->m_random.nextFloat() * 0.15F + 0.6F);
	}

	m_bUpdating = false;
}

void PistonBaseTile::setPlacedBy(Level* level, const TilePos& pos, Mob* mob, Facing::Name face)
{
	level->setData(pos, getRotationData(level, pos, (Player*)mob));
	if (!level->m_bIsOnline)
		checkIfExtend(level, pos);
}

void PistonBaseTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
	if (!level->m_bIsOnline && !m_bUpdating)
		checkIfExtend(level, pos);
}

void PistonBaseTile::onPlace(Level* level, const TilePos& pos)
{
	if (!level->m_bIsOnline && !level->getTileEntity(pos))
		checkIfExtend(level, pos);
}

void PistonBaseTile::updateShape(const LevelSource* source, const TilePos& pos)
{
	int data = source->getData(pos);
	if (isExtended(data))
	{
		switch (getFacing(data))
		{
		case 0:
			setShape(0.0F, 0.25F, 0.0F, 1.0F, 1.0F, 1.0F);
			break;
		case 1:
			setShape(0.0F, 0.0F, 0.0F, 1.0F, 12.0F / 16.0F, 1.0F);
			break;
		case 2:
			setShape(0.0F, 0.0F, 0.25F, 1.0F, 1.0F, 1.0F);
			break;
		case 3:
			setShape(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 12.0F / 16.0F);
			break;
		case 4:
			setShape(0.25F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
			break;
		case 5:
			setShape(0.0F, 0.0F, 0.0F, 12.0F / 16.0F, 1.0F, 1.0F);
		}
	}
	else
		setShape(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
}

void PistonBaseTile::updateDefaultShape()
{
	setShape(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
}

void PistonBaseTile::addAABBs(const Level* level, const TilePos& pos, const AABB* aabb, std::vector<AABB>& out)
{
	setShape(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
	Tile::addAABBs(level, pos, aabb, out);
}

bool PistonBaseTile::isSolidRender() const
{
	return false;
}

bool PistonBaseTile::isCubeShaped() const
{
	return false;
}

int PistonBaseTile::getFaceTexture() const
{
	return m_bIsSticky ? 106 : 107;
}

void PistonBaseTile::checkIfExtend(Level* level, const TilePos& pos)
{
	int data = level->getData(pos);
	int face = getFacing(data);
	bool signal = getNeighborSignal(level, pos, (Facing::Name)face);
	if (data != 7)
	{
		if (signal && !isExtended(data))
		{
			if (canMoveBlocks(level, pos, (Facing::Name)face))
			{
				level->setDataNoUpdate(pos, face | 8);
				level->tileEvent(pos, 0, face);
			}
		}
		else if (!signal && isExtended(data))
		{
			level->setDataNoUpdate(pos, face);
			level->tileEvent(pos, 1, face);
		}

	}
}

bool PistonBaseTile::getNeighborSignal(Level* level, const TilePos& pos, Facing::Name facing)
{
	for (Facing::Name dir : Facing::ALL)
	{
		TilePos rel = pos.relative(dir);
		if ((facing != dir && level->getSignal(rel, dir)) || (Facing::isHorizontal(dir) && level->getSignal(rel.above(), dir)))
			return true;
	}

	return level->getSignal(pos, Facing::DOWN) || level->getSignal(pos.above(2), Facing::UP);
}

int PistonBaseTile::getRotationData(Level* level, const TilePos& pos, Player* player)
{
	if (Mth::abs(player->m_pos.x - pos.x) < 2.0F && Mth::abs(player->m_pos.z - pos.z) < 2.0F)
	{
		real var5 = player->m_pos.y + 1.82 - player->m_heightOffset;
		if (var5 - pos.y > 2.0)
			return 1;

		if (pos.y - var5 > 0.0)
			return 0;
	}

	int var7 = Mth::floor((player->m_rot.y * 4.0F / 360.0F) + 0.5) & 3;
	return var7 == 0 ? 2 : (var7 == 1 ? 5 : (var7 == 2 ? 3 : (var7 == 3 ? 4 : 0)));
}

bool PistonBaseTile::isPushable(TileID tile, Level* level, const TilePos& pos, bool destroy)
{
	if (tile == Tile::obsidian->m_ID)
		return false;
	else
	{
		if (tile != Tile::piston->m_ID && tile != Tile::stickyPiston->m_ID)
		{
			if (Tile::tiles[tile]->m_hardness == -1.0F)
				return false;

			if (Tile::tiles[tile]->getPistonPushReaction() == 2)
				return false;

			if (!destroy && Tile::tiles[tile]->getPistonPushReaction() == 1)
				return false;
		}
		else if (isExtended(level->getData(pos)))
			return false;

		return level->getTileEntity(pos) == nullptr;
	}
}

bool PistonBaseTile::canMoveBlocks(Level* level, const TilePos& pos, Facing::Name facing)
{
	TilePos tp = pos.relative(facing);
	int steps = 0;

	while (true)
	{
		if (steps < 13)
		{
			if (tp.y <= 0 || tp.y >= 127)
				return false;

			TileID tile = level->getTile(tp);
			if (tile)
			{
				if (!isPushable(tile, level, tp, true))
					return false;

				if (Tile::tiles[tile]->getPistonPushReaction() != 1)
				{
					if (steps == 12)
						return false;

					tp = tp.relative(facing);
					++steps;
					continue;
				}
			}
		}

		return true;
	}
}

bool PistonBaseTile::moveBlocks(Level* level, const TilePos& pos, Facing::Name facing)
{
	TilePos tp = pos.relative(facing);
	int steps = 0;

	while (true)
	{
		if (steps < 13)
		{
			if (tp.y <= 0 || tp.y >= 127)
				return false;

			TileID tile = level->getTile(tp);
			if (tile)
			{
				if (!isPushable(tile, level, tp, true))
					return false;

				if (Tile::tiles[tile]->getPistonPushReaction() != 1)
				{
					if (steps == 12)
						return false;

					tp = tp.relative(facing);
					++steps;
					continue;
				}

				Tile::tiles[tile]->spawnResources(level, tp, level->getData(tp));
				level->setTile(tp, TILE_AIR);
			}
		}

		while (tp.x != pos.x || tp.y != pos.y || tp.z != pos.z)
		{
			TilePos newTp = tp.relative(Facing::OPPOSITE[facing]);
			TileID tile = level->getTile(newTp);
			int data = level->getData(newTp);
			if (tile == m_ID && newTp.x == pos.x && newTp.y == pos.y && newTp.z == pos.z)
			{
				level->setTileAndDataNoUpdate(tp, Tile::movingPiston->m_ID, facing | (m_bIsSticky ? 8 : 0));
				level->setTileEntity(tp, MovingPistonTile::newMovingTileEntity(Tile::pistonHead->m_ID, facing | (m_bIsSticky ? 8 : 0), facing, true, false));
			}
			else
			{
				level->setTileAndDataNoUpdate(tp, Tile::movingPiston->m_ID, data);
				level->setTileEntity(tp, MovingPistonTile::newMovingTileEntity(tile, data, facing, true, false));
			}
			tp = newTp;
		}

		return true;
	}
}
