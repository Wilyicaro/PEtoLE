/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Particle.hpp"
#include "world/level/Level.hpp"
#include <client/renderer/entity/EntityRenderDispatcher.hpp>

PickupParticle::PickupParticle(Level* level, std::shared_ptr<Entity> throwed, std::shared_ptr<Entity> thrower, float vel) :
	Particle(level, throwed->m_pos, throwed->m_vel)
{
	this->throwed = throwed;
	this->thrower = thrower;
	this->vel = vel;
	m_lifetime = 3;
}

void PickupParticle::render(Tesselator& t, float f, float a, float b, float c, float d, float e)
{
	float var8 = (m_age + f) / m_lifetime;
	var8 *= var8;
	Vec3 tPos(
		Mth::lerp(thrower->m_posPrev.x, thrower->m_pos.x, f),
		Mth::lerp(thrower->m_posPrev.y, thrower->m_pos.y, f) + vel + thrower->m_heightOffset,
		Mth::lerp(thrower->m_posPrev.z, thrower->m_pos.z, f)
	);
	Vec3 pos = m_pos + (tPos - m_pos) * var8;
	float var30 = m_pLevel->getBrightness(TilePos(pos.x, pos.y + (m_heightOffset / 2.0F), pos.z));
	glColor4f(var30, var30, var30, 1.0F);
	EntityRenderDispatcher::getInstance()->render(throwed.get(), pos - off, throwed->m_rot.y, f);

}

void PickupParticle::tick()
{
	m_age++;
	if (m_age >= m_lifetime)
		remove();

}

int PickupParticle::getParticleTexture()
{
	return PT_PARTICLES3;
}

