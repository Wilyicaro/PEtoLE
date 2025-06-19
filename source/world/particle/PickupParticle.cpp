/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Particle.hpp"
#include <client/renderer/entity/EntityRenderDispatcher.hpp>

PickupParticle::PickupParticle(Level* level, std::shared_ptr<Entity> throwed, std::shared_ptr<Entity> thrower, float vel) :
	Particle(level, throwed->m_pos, throwed->m_vel)
{
	this->throwed = throwed;
	this->thrower = thrower;
	this->vel = vel;
	field_EC = 3;
}

void PickupParticle::render(Tesselator& t, float f, float a, float b, float c, float d, float e)
{
	float var8 = (field_E8 + f) / field_EC;
	var8 *= var8;
	float var15 = Mth::Lerp(thrower->m_posPrev.x, thrower->m_pos.x, f);
	float var17 = Mth::Lerp(thrower->m_posPrev.y, thrower->m_pos.y, f) + vel;
	float var19 = Mth::Lerp(thrower->m_posPrev.z, thrower->m_pos.z, f);
	float var21 = m_pos.x + (var15 - m_pos.x) * var8;
	float var23 = m_pos.y + (var17 - m_pos.y) * var8;
	float var25 = m_pos.z + (var19 - m_pos.z) * var8;
	float var30 = m_pLevel->getBrightness(TilePos(var21, var23 + (m_heightOffset / 2.0F), var25));
	glColor4f(var30, var30, var30, 1.0F);
	EntityRenderDispatcher::getInstance()->render(throwed.get(), Vec3(var21 - xOff, var23 - yOff, var25 - zOff), throwed->m_rot.y, f);

}

void PickupParticle::tick()
{
	field_E8++;
	if (field_E8 >= field_EC)
		remove();

}

int PickupParticle::getParticleTexture()
{
	return PT_PARTICLES3;
}

