/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "FallingTile.hpp"
#include "world/level/Level.hpp"

FallingTile::FallingTile(Level* level) : Entity(level), field_E0(0)
{
	m_pType = EntityType::fallingTile;
}

FallingTile::FallingTile(Level* level, const Vec3& pos, int id) : FallingTile(level)
{
	m_id = id;
    m_bBlocksBuilding = false;
	setSize(0.98f, 0.98f);
	m_heightOffset = m_bbHeight * 0.5f;
	setPos(pos);
	m_oPos = pos;
	m_bMakeStepSound = false;
	m_vel = Vec3::ZERO;
}

float FallingTile::getShadowHeightOffs() const
{
	return 0.0f;
}

bool FallingTile::isPickable() const
{
	return !m_bRemoved;
}

void FallingTile::tick()
{
	if (!m_id)
		remove();

	m_oPos = m_pos;
	field_E0++;

	m_vel.y -= 0.04f;
	move(m_vel);

	m_vel *= 0.98f;

	TilePos tilePos(m_pos);

	// if we're inside one of our own tiles, clear it.
	// Assumes we started there
	if (m_pLevel->getTile(tilePos) == m_id)
		m_pLevel->setTile(tilePos, TILE_AIR);

	if (!m_bOnGround)
	{
		if (field_E0 > 100 && !m_pLevel->m_bIsOnline)
			remove();

		return;
	}

	m_vel.x *= 0.7f;
	m_vel.z *= 0.7f;
	m_vel.y *= -0.5f;
	remove();
	if (m_pLevel->mayPlace(m_id, tilePos, true, Facing::UP))
	{
		m_pLevel->setTile(tilePos, m_id);
	}
	else
	{
		this->spawnAtLocation(m_id, 1);
	}
}

Level* FallingTile::getLevel()
{
	return m_pLevel;
}


void FallingTile::addAdditionalSaveData(CompoundIO tag) {
	tag->putByte("Tile", m_id);
}

void FallingTile::readAdditionalSaveData(CompoundIO tag) {
	m_id = tag->getByte("Tile") & 255;
}