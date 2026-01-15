/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "Pig.hpp"
#include "PigZombie.hpp"
#include "world/level/Level.hpp"
#include "common/Utils.hpp"
#include "stats/Achievement.hpp"

Pig::Pig(Level* pLevel) : Animal(pLevel)
{
	m_pType = EntityType::pig;
	m_texture = "mob/pig.png";
	setSize(0.9f, 0.9f);
}

void Pig::defineSynchedData()
{
	m_entityData.define(16, (int8_t)0);
}

int Pig::getDeathLoot() const
{
	if (isOnFire())
		return Item::cookedPorkchop->m_itemID;
	else
		return Item::rawPorkchop->m_itemID;
}

bool Pig::hasSaddle() const
{
	return m_entityData.get<int8_t>(16) & 1;
}

void Pig::setSaddle(bool b)
{
	m_entityData.set(16, (int8_t) b);
}

bool Pig::interact(Player* player) {
	if (!hasSaddle() || m_pLevel->m_bIsOnline || m_pRider && m_pRider.get() != player)
		return false;
	else 
	{
		player->ride(shared_from_this());
		return true;
	}
}

void Pig::addAdditionalSaveData(CompoundIO tag)
{
	Animal::addAdditionalSaveData(tag);
	tag->putBoolean("Saddle", hasSaddle());
}

void Pig::readAdditionalSaveData(CompoundIO tag)
{
	Animal::readAdditionalSaveData(tag);
	setSaddle(tag->getBoolean("Saddle"));
}

void Pig::thunderHit(LightningBolt* lighting)
{
	if (!m_pLevel->m_bIsOnline)
	{
		std::shared_ptr<PigZombie> pigZombie = std::make_shared<PigZombie>(m_pLevel);
		pigZombie->moveTo(m_pos, m_rot);
		m_pLevel->addEntity(pigZombie);
		remove();
	}
}

void Pig::causeFallDamage(float fallDistance)
{
	Animal::causeFallDamage(fallDistance);
	if (fallDistance > 5.0F && m_pRider && m_pRider->isPlayer())
		((Player*)m_pRider.get())->awardStat(Achievements::flyPig);
}
