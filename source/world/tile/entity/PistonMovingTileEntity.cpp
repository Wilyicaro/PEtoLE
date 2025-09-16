#include "PistonMovingTileEntity.hpp"
#include "world/tile/MovingPistonTile.hpp"

PistonMovingTileEntity::PistonMovingTileEntity() : PistonMovingTileEntity(0, 0, Facing::DOWN, false, false)
{
}

PistonMovingTileEntity::PistonMovingTileEntity(TileID tile, int meta, Facing::Name direction, bool extending, bool isSourcePiston) : m_tileId(tile), m_meta(meta), m_direction(direction), m_bExtending(extending), m_bIsSourcePiston(isSourcePiston), m_progress(0), m_progressO(0)
{
	m_pType = TileEntityType::piston;
}

int PistonMovingTileEntity::getTileId() const
{
	return m_tileId;
}

int PistonMovingTileEntity::getData() const
{
	return m_meta;
}

bool PistonMovingTileEntity::isExtending() const
{
	return m_bExtending;
}

Facing::Name PistonMovingTileEntity::getDirection() const
{
	return m_direction;
}

bool PistonMovingTileEntity::isSourcePiston() const
{
	return m_bIsSourcePiston;
}

float PistonMovingTileEntity::getProgress(float partialTick) const
{
	return Mth::Lerp(m_progressO, m_progress, Mth::Min(1.0f, partialTick));
}

Vec3f PistonMovingTileEntity::getOff(float partialTick) const
{
	Vec3f normal = Vec3f(Facing::NORMALS[m_direction]);
	return m_bExtending ? normal * (getProgress(partialTick) - 1.0F) : normal * (1.0F - getProgress(partialTick));
}

void PistonMovingTileEntity::tick()
{
	m_progressO = m_progress;
	if (m_progressO >= 1.0F)
	{
		moveCollidedEntities(1.0F, 0.25F);
		m_pLevel->removeTileEntity(m_pos);
		setRemoved();
		if (m_pLevel->getTile(m_pos) == Tile::movingPiston->m_ID)
			m_pLevel->setTileAndData(m_pos, m_tileId, m_meta);
	}
	else
	{
		m_progress += 0.5F;
		if (m_progress >= 1.0F)
			m_progress = 1.0F;

		if (m_bExtending)
			moveCollidedEntities(m_progress, m_progress - m_progressO + 1.0F / 16.0F);
	}
}

void PistonMovingTileEntity::load(CompoundIO tag)
{
	TileEntity::load(tag);
	m_tileId = tag->getInt("blockId");
	m_meta = tag->getInt("blockData");
	m_direction = (Facing::Name) tag->getInt("facing");
	m_progressO = m_progress = tag->getFloat("progress");
	m_bExtending = tag->getBoolean("extending");
}

void PistonMovingTileEntity::save(CompoundIO tag)
{
	TileEntity::save(tag);
	tag->putInt("blockId", m_tileId);
	tag->putInt("blockData", m_meta);
	tag->putInt("facing", m_direction);
	tag->putFloat("progress", m_progressO);
	tag->putBoolean("extending", m_bExtending);
}

void PistonMovingTileEntity::moveCollidedEntities(float progress, float force)
{
	if (!m_bExtending)
		progress--;
	else
		progress = 1.0F - progress;

	AABB* aabb = Tile::movingPiston->moveByPositionAndProgress(m_pLevel, m_pos, m_tileId, progress, m_direction);
	if (aabb)
	{
		EntityVector vec = m_pLevel->getEntities(nullptr, *aabb);
		if (!vec.empty())
		{
			Vec3 move = Vec3(Facing::NORMALS[m_direction]) * force;
			for (auto& ent : vec)
				ent->move(move);
		}
	}
}

void PistonMovingTileEntity::finalTick()
{
	if (m_progressO < 1.0F)
	{
		m_progressO = m_progress = 1.0F;
		m_pLevel->removeTileEntity(m_pos);
		setRemoved();
		if (m_pLevel->getTile(m_pos) == Tile::movingPiston->m_ID)
			m_pLevel->setTileAndData(m_pos, m_tileId, m_meta);
	}
}
