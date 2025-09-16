/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "Wolf.hpp"
#include "world/level/Level.hpp"
#include "common/Utils.hpp"
#include "projectile/Arrow.hpp"
#include "world/item/FoodItem.hpp"

Wolf::Wolf(Level* pLevel) : Animal(pLevel)
{
	m_pType = EntityType::wolf;
	m_texture = "mob/wolf.png";
	setSize(0.8f, 0.8f);
	m_health = 8;
}

void Wolf::defineSynchedData()
{
	m_entityData.define<int8_t>(16, 0);
	m_entityData.define<std::string>(17, "");
	m_entityData.define<int>(18, m_health);
}

int Wolf::getDeathLoot() const
{
	return -1;
}

float Wolf::getWetShade(float f) const
{
	return Mth::Min(12.0F / 16.0F + Mth::Lerp(m_shakeAnimO, m_shakeAnim, f) / 2.0f * 0.25f, 1.0f);
}

float Wolf::getBodyRollAngle(float f, float g) const
{
	float ang = Mth::clamp((Mth::Lerp(m_shakeAnimO, m_shakeAnim, f) + g) / 1.8f, 0.0f, 1.0f);

	return Mth::sin(ang * M_PI) * Mth::sin(ang * M_PI * 11.0f) * 0.15f * M_PI;
}

float Wolf::getHeadRollAngle(float f) const
{
	return Mth::Lerp(m_interestedAngleO, m_interestedAngle, f) * 0.15F * M_PI;
}

std::string Wolf::getOwner()
{
	return m_entityData.get<std::string>(17);
}

void Wolf::setOwner(std::string owner)
{
	m_entityData.set<std::string>(17, owner);
}

bool Wolf::isAngry() const
{
	return m_entityData.get<int8_t>(16) & 2;
}

void Wolf::setAngry(bool b)
{
	int8_t data = m_entityData.get<int8_t>(16);
	m_entityData.set<int8_t>(16, b ? data | 2 : data & -3);
}

bool Wolf::isTamed() const
{
	return m_entityData.get<int8_t>(16) & 4;
}

void Wolf::setTamed(bool b)
{
	int8_t data = m_entityData.get<int8_t>(16);
	m_entityData.set<int8_t>(16, b ? data | 4 : data & -5);
}

bool Wolf::isOrderedToSit() const
{
	return m_entityData.get<int8_t>(16) & 1;
}

void Wolf::setOrderedToSit(bool b)
{
	int8_t data = m_entityData.get<int8_t>(16);
	m_entityData.set<int8_t>(16, b ? data | 1 : data & -2);
}

void Wolf::updateAttackTarget(Entity* ent, float dist)
{
	if (!m_pLevel->findPath(&m_path, this, ent, 16.0F) && dist > 12.0F)
	{
		TilePos base(ent->m_pos.x - 2, ent->m_hitbox.min.y, ent->m_pos.z - 2);

		TilePos tp(base);
		for (int dx = 0; dx <= 4; ++dx) {
			tp.x = base.x + dx;
			for (int dz = 0; dz <= 4; ++dz) {
				tp.z = base.z + dz;
				if ((dx < 1 || dz < 1 || dx > 3 || dz > 3) && m_pLevel->isNormalTile(tp.below()) && !m_pLevel->isNormalTile(tp) && !m_pLevel->isNormalTile(tp.above())) {
					moveTo(Vec3(tp.x + 0.5, tp.y, tp.z + 0.5), m_rot);
					return;
				}
			}
		}
	}
}

void Wolf::addTamingParticles(bool heart)
{
	const std::string particle = heart ? "heart" : "smoke";
	
	Vec3 pos;
	Vec3 vel;
	for (int var3 = 0; var3 < 7; ++var3)
	{
		pos.x = m_pos.x + (m_random.nextFloat() * m_bbWidth * 2.0F) - m_bbWidth;
		pos.y = m_pos.y + (m_random.nextFloat() * m_bbHeight * 2.0F) - m_bbHeight;
		pos.z = m_pos.z + (m_random.nextFloat() * m_bbWidth * 2.0F) - m_bbWidth;
		vel.x = m_random.nextGaussian() * 0.02;
		vel.y = m_random.nextGaussian() * 0.02;
		vel.z = m_random.nextGaussian() * 0.02;
		m_pLevel->addParticle(particle, pos, vel);
	}
}

void Wolf::addAdditionalSaveData(CompoundIO tag)
{
	Animal::addAdditionalSaveData(tag);
	tag->putBoolean("Angry", isAngry());
	tag->putBoolean("Sitting", isOrderedToSit());
	tag->putString("Owner", getOwner());
}

void Wolf::readAdditionalSaveData(CompoundIO tag)
{
	Animal::readAdditionalSaveData(tag);
	setAngry(tag->getBoolean("Angry"));
	setOrderedToSit(tag->getBoolean("Sitting"));
	std::string owner = tag->getString("Owner");
	if (!owner.empty())
	{
		setTamed(true);
		setOwner(owner);
	}
}

bool Wolf::canDespawn() const
{
	return !isTamed();
}

void Wolf::updateAi()
{
	Animal::updateAi();

	if (!m_bHoldGround && !isPathFinding() && isTamed() && !m_pRiding)
	{
		auto player = m_pLevel->getPlayer(getOwner());
		if (player)
		{
			float var2 = player->distanceToSqr(this);
			if (var2 > 5.0F)
				updateAttackTarget(player.get(), var2);
		}
		else if (!wasInWater())
			setOrderedToSit(true);
	}
	else if (!m_pAttackTarget && !isPathFinding() && !isTamed() && m_random.nextInt(100) == 0)
	{
		AABB hit = AABB(m_pos, m_pos + 1);
		hit.grow(16.0, 4.0, 16.0);
		auto var1 = m_pLevel->getEntitiesOfType(EntityType::sheep, hit);
		if (!var1.empty())
			setAttackTarget(var1[m_pLevel->m_random.nextInt(var1.size())]);
	}

	if (wasInWater())
		setOrderedToSit(false);

	if (!m_pLevel->m_bIsOnline)
		m_entityData.set<int>(18, m_health);
}

void Wolf::aiStep()
{
	Animal::aiStep();
	m_bInterested = false;
	if (m_pEntLookedAt && !isPathFinding() && !isAngry())
	{
		if (m_pEntLookedAt->isPlayer())
		{
			Player* player = (Player*)m_pEntLookedAt.get();
			auto selected = player->getSelectedItem();
			if (selected)
			{
				if (!isTamed() && selected->m_itemID == Item::bone->m_itemID)
					m_bInterested = true;
				else if (isTamed() && selected->getItem()->isFood())
					m_bInterested = selected->getItem()->isWolfFood();
			}
		}
	}

	if (!m_bInterpolateOnly && m_bIsShaking && !m_bIsWet && !isPathFinding() && m_bOnGround)
	{
		m_bIsWet = true;
		m_shakeAnim = 0.0F;
		m_shakeAnimO = 0.0F;
		m_pLevel->entityEvent(this, 8);
	}
}

void Wolf::tick()
{
	Animal::tick();

	m_interestedAngleO = m_interestedAngle;
	if (m_bInterested)
		m_interestedAngle += (1.0F - m_interestedAngle) * 0.4F;
	else
		m_interestedAngle += (0.0F - m_interestedAngle) * 0.4F;

	if (m_bInterested)
		m_lookTime = 10;

	if (isWet()) {
		m_bIsShaking = true;
		m_bIsWet = false;
		m_shakeAnim = 0.0F;
		m_shakeAnimO = 0.0F;
	}
	else if (m_bIsWet)
	{
		if (m_shakeAnim == 0.0F)
			m_pLevel->playSound(this, "mob.wolf.shake", getSoundVolume(), (m_random.nextFloat() - m_random.nextFloat()) * 0.2F + 1.0F);

		m_shakeAnimO = m_shakeAnim;
		m_shakeAnim += 0.05F;
		if (m_shakeAnim >= 2.0F)
		{
			m_bIsShaking = false;
			m_bIsWet = false;
			m_shakeAnim = 0.0F;
			m_shakeAnimO = 0.0F;
		}

		if (m_shakeAnim > 0.4F)
		{
			float var1 = m_hitbox.min.y;
			int var2 = (int)(Mth::sin((m_shakeAnim - 0.4F) * M_PI) * 7.0F);

			for (int var3 = 0; var3 < var2; ++var3) {
				float var4 = (m_random.nextFloat() * 2.0F - 1.0F) * m_bbWidth * 0.5F;
				float var5 = (m_random.nextFloat() * 2.0F - 1.0F) * m_bbWidth * 0.5F;
				m_pLevel->addParticle("splash", Vec3(m_pos.x + var4, (var1 + 0.8F), m_pos.z + var5), m_vel);
			}
		}
	}
}

float Wolf::getHeadHeight() const
{
	return m_bbHeight * 0.8f;
}

int Wolf::getMaxHeadXRot() const
{
	return isOrderedToSit() ? 40 : Animal::getMaxHeadXRot();
}

bool Wolf::isImmobile() const
{
	return isOrderedToSit() || m_bIsWet;
}

void Wolf::checkHurtTarget(Entity* ent, float dist)
{
	if (dist > 2.0F && dist < 6.0F && m_random.nextInt(10) == 0)
	{
		if (m_bOnGround) {
			real diffX = ent->m_pos.x - m_pos.x;
			real diffZ = ent->m_pos.z - m_pos.z;
			float length = Mth::sqrt(diffX * diffX + diffZ * diffZ);
			m_vel.x = diffX / length * 0.5 * 0.8F + m_vel.x * 0.2F;
			m_vel.z = diffZ / length * 0.5 * 0.8F + m_vel.z * 0.2F;
			m_vel.y = 0.4F;
		}
	}
	else if (dist < 1.5 && ent->m_hitbox.max.y > m_hitbox.min.y && ent->m_hitbox.min.y < m_hitbox.max.y)
	{
		m_attackTime = 20;

		ent->hurt(this, isTamed() ? 4 : 2);
	}
}

std::shared_ptr<Entity> Wolf::findAttackTarget()
{
	return isAngry() ? m_pLevel->getNearestPlayer(this, 16.0f) : nullptr;
}

bool Wolf::hurt(Entity* ent, int dmg)
{
	setOrderedToSit(false);
	if (ent && !ent->isPlayer() && ent->getType() != EntityType::arrow)
		dmg = (dmg + 1) / 2;

	if (!Animal::hurt(ent, dmg))
		return false;
	else
	{
		if (!isTamed() && !isAngry())
		{
			if (ent && ent->isPlayer())
			{
				setAngry(true);
				m_pAttackTarget = ent->shared_from_this();
			}

			if (ent && ent->getType() == EntityType::arrow && ((Arrow*)ent)->m_owner)
				ent = ((Arrow*)ent)->m_owner.get();

			if (ent->getCategory().contains(EntityCategories::MOB))
			{
				AABB hit(m_pos, m_pos + 1);
				hit.grow(16.0, 4.0, 16.0);
				auto wolfs = m_pLevel->getEntitiesOfType(EntityType::wolf, hit);

				for (auto& entity : wolfs)
				{
					std::shared_ptr<Wolf> wolf = std::dynamic_pointer_cast<Wolf>(entity);
					if (!wolf->isTamed() && !wolf->m_pAttackTarget) {
						wolf->m_pAttackTarget = ent->shared_from_this();
						if (ent->isPlayer())
							wolf->setAngry(true);
					}
				}
			}
		}
		else if (ent != this && ent)
		{
			if (isTamed() && ent->isPlayer() && ((Player*)ent)->m_name == getOwner())
				return true;

			m_pAttackTarget = ent->shared_from_this();
		}

		return true;
	}
}

bool Wolf::interact(Player* player)
{
	std::shared_ptr<ItemInstance> selected = player->getSelectedItem();
	if (!isTamed())
	{
		if (selected && selected->m_itemID == Item::bone->m_itemID && !isAngry())
		{
			--selected->m_count;
			if (selected->m_count <= 0)
				player->m_pInventory->setSelectedItem(nullptr);

			if (!m_pLevel->m_bIsOnline) {
				if (m_random.nextInt(3) == 0)
				{
					setTamed(true);
					m_path.clear();
					setOrderedToSit(true);
					m_health = getMaxHealth();
					setOwner(player->m_name);
					addTamingParticles(true);
					m_pLevel->entityEvent(this, 7);
				}
				else
				{
					addTamingParticles(false);
					m_pLevel->entityEvent(this, 6);
				}
			}

			return true;
		}
	}
	else {
		if (selected && selected->getItem()->isFood())
		{
			if (selected->getItem()->isWolfFood() && m_entityData.get<int>(18) < 20)
			{
				--selected->m_count;
				if (selected->m_count <= 0)
					player->m_pInventory->setSelectedItem(nullptr);

				heal(((FoodItem*)Item::rawPorkchop)->getNutrition());
				return true;
			}
		}

		if (player->m_name == getOwner())
		{
			if (!m_pLevel->m_bIsOnline)
			{
				setOrderedToSit(!isOrderedToSit());
				m_bJumping = false;
				m_path.clear();
			}

			return true;
		}
	}

	return false;
}

void Wolf::handleEntityEvent(int event)
{
	switch (event)
	{
	case 6:
	case 7:
		addTamingParticles(event == 7);
		break;
	case 8:
		m_bIsWet = true;
		m_shakeAnim = 0.0F;
		m_shakeAnimO = 0.0F;
		break;
	default:
		Animal::handleEntityEvent(event);
		break;
	}
}

float Wolf::getTailAngle() const
{
	if (isAngry())
		return M_PI_2;
	else
		return isTamed() ? (0.55F - (getMaxHealth() - m_entityData.get<int>(18)) * 0.02F) * M_PI : (M_PI / 5);
}

bool Wolf::isShaking() const
{
	return m_bIsShaking;
}
