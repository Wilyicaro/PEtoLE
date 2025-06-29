/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Entity.hpp"
#include "Player.hpp"
#include "world/level/Level.hpp"
#include "EntityIO.hpp"

#define TOTAL_AIR_SUPPLY (300)

int Entity::entityCounter;

void Entity::_init()
{
	m_random = Random();
	m_bInAChunk = false;
	m_chunkPos = ChunkPos(0, 0);
	field_20 = 0;
	field_24 = 0;
	field_28 = 0;
	m_noNeighborUpdate = 1.0f;
    m_bBlocksBuilding = false;
	m_pLevel = nullptr;
	m_rot = Vec2::ZERO;
	m_rotPrev = Vec2::ZERO;
	m_onGround = false;
	m_bHorizontalCollision = false;
	m_bVerticalCollision = false;
	m_bCollision = false;
	m_bHurt = false;
	m_bInWeb = false;
	slide = true;
	m_bRemoved = false;
	m_heightOffset = 0.0f;
	m_bbWidth = 0.6f;
	m_bbHeight = 1.8f;
	field_90 = 0.0f;
	m_walkDist = 0.0f;
	m_bMakeStepSound = true;
	m_ySlideOffset = 0.0f;
	m_footSize = 0.0f;
	m_bNoPhysics = false;
	m_pushThrough = 0.0f;
    m_tickCount = 0;
	m_invulnerableTime = 0;
	m_airCapacity = TOTAL_AIR_SUPPLY;
	m_fireTicks = 0;
	m_flameTime = 1;
    m_tickCount = 0;
	m_renderType = 0;  // @NOTE: Render type? (eEntityRenderType)
	m_distanceFallen = 0.0f;
	m_airSupply = TOTAL_AIR_SUPPLY;
	m_bWasInWater = 0;
	m_bFireImmune = false;
	m_bFirstTick = true;
	m_nextStep = 1;
	m_minBrightness = 0.0f;
	m_entityData = SynchedEntityData();
	m_pEntityType = EntityType::unknown;
}

Entity::Entity(Level* pLevel)
{
	_init();

	m_pLevel = pLevel;
	m_EntityID = ++entityCounter;
	setPos(Vec3::ZERO);

	m_entityData.define<int8_t>(0, 0);
}

Entity::~Entity()
{
}

void Entity::setLevel(Level* pLvl)
{
	m_pLevel = pLvl;
}

void Entity::removed()
{
	m_bRemoved = true;
}

void Entity::setPos(const Vec3& pos)
{
	m_pos = pos;

	real halfSize = m_bbWidth / 2;
	real lowY = m_pos.y - m_heightOffset + m_ySlideOffset;

	m_hitbox = AABB(
		m_pos.x - halfSize,
		lowY,
		m_pos.z - halfSize,
		m_pos.x + halfSize,
		lowY + m_bbHeight,
		m_pos.z + halfSize);
}

void Entity::remove()
{
	m_bRemoved = true;
}

int Entity::move(const Vec3& pos)
{
	if (m_bNoPhysics) {
		m_hitbox.move(pos);
		m_pos.x = (m_hitbox.min.x + m_hitbox.max.x) / 2.0;
		m_pos.y = m_hitbox.min.y + m_heightOffset - m_ySlideOffset;
		m_pos.z = (m_hitbox.min.z + m_hitbox.max.z) / 2.0;
	}
	else {
		m_ySlideOffset *= 0.4;
		Vec3 newPos(pos);
		if (m_bInWeb) {
			m_bInWeb = false;
			newPos.x *= 0.25;
			newPos.y *= 0.05;
			newPos.z *= 0.25;
			m_vel *= 0;
			m_distanceFallen = 0.0F;
		}
		real aPosX = m_pos.x;
		real aPosZ = m_pos.z;
		real cPosX = newPos.x;
		real cPosY = newPos.y;
		real cPosZ = newPos.z;
		AABB var17 = m_hitbox;
		bool validSneaking = m_onGround && isSneaking();
		if (validSneaking) {
			real var19;
			for (var19 = 0.05; pos.x != 0.0 && m_pLevel->getCubes(this, m_hitbox.cloneMove(newPos.x, -1.0, 0.0))->size() == 0; cPosX = newPos.x) {
				if (newPos.x < var19 && newPos.x >= -var19) {
					newPos.x = 0.0;
				}
				else if (newPos.x > 0.0) {
					newPos.x -= var19;
				}
				else {
					newPos.x += var19;
				}
			}

			for (; newPos.z != 0.0 && m_pLevel->getCubes(this, m_hitbox.cloneMove(0.0, -1.0, newPos.z))->size() == 0; cPosZ = newPos.z) {
				if (newPos.z < var19 && newPos.z >= -var19) {
					newPos.z = 0.0;
				}
				else if (newPos.z > 0.0) {
					newPos.z -= var19;
				}
				else {
					newPos.z += var19;
				}
			}
		}

		AABB toExpand = m_hitbox;
		toExpand.expand(newPos.x, newPos.y, newPos.z);
		auto var35 = m_pLevel->getCubes(this, toExpand);

		for (int var20 = 0; var20 < var35->size(); ++var20) {
			newPos.y = var35->at(var20).clipYCollide(m_hitbox, newPos.y);
		}

		m_hitbox.move(0.0, newPos.y, 0.0);
		if (!slide && cPosY != newPos.y) {
			newPos.z = 0.0;
			newPos.y = 0.0;
			newPos.x = 0.0;
		}

		bool var36 = m_onGround || cPosY != newPos.y && cPosY < 0.0;

		int var21;
		for (var21 = 0; var21 < var35->size(); ++var21) {
			newPos.x = var35->at(var21).clipXCollide(m_hitbox, newPos.x);
		}

		m_hitbox.move(newPos.x, 0.0, 0.0);
		if (!slide && cPosX != newPos.x) {
			newPos.z = 0.0;
			newPos.y = 0.0;
			newPos.x = 0.0;
		}

		for (var21 = 0; var21 < var35->size(); ++var21) {
			newPos.z = var35->at(var21).clipZCollide(m_hitbox, newPos.z);
		}

		m_hitbox.move(0.0, 0.0, newPos.z);
		if (!slide && cPosZ != newPos.z) {
			newPos.z = 0.0;
			newPos.y = 0.0;
			newPos.x = 0.0;
		}

		real var23;
		int var28;
		real var37;
		if (m_footSize > 0.0F && var36 && m_ySlideOffset < 0.05F && (cPosX != newPos.x || cPosZ != newPos.z)) {
			var37 = newPos.x;
			var23 = newPos.y;
			real var25 = newPos.z;
			newPos.x = cPosX;
			newPos.y = m_footSize;
			newPos.z = cPosZ;
			AABB var27 = m_hitbox;
			toExpand = m_hitbox = var17;
			toExpand.expand(cPosX, newPos.y, cPosZ);
			var35 = m_pLevel->getCubes(this, toExpand);

			for (var28 = 0; var28 < var35->size(); ++var28) {
				newPos.y = var35->at(var28).clipYCollide(m_hitbox, newPos.y);
			}

			m_hitbox.move(0.0, newPos.y, 0.0);
			if (!slide && cPosY != newPos.y) {
				newPos.z = 0.0;
				newPos.y = 0.0;
				newPos.x = 0.0;
			}

			for (var28 = 0; var28 < var35->size(); ++var28) {
				newPos.x = var35->at(var28).clipXCollide(m_hitbox, newPos.x);
			}

			m_hitbox.move(newPos.x, 0.0, 0.0);
			if (!slide && cPosX != newPos.x) {
				newPos.z = 0.0;
				newPos.y = 0.0;
				newPos.x = 0.0;
			}

			for (var28 = 0; var28 < var35->size(); ++var28) {
				newPos.z = var35->at(var28).clipZCollide(m_hitbox, newPos.z);
			}

			m_hitbox.move(0.0, 0.0, newPos.z);
			if (!slide && cPosZ != newPos.z) {
				newPos.z = 0.0;
				newPos.y = 0.0;
				newPos.x = 0.0;
			}

			if (var37 * var37 + var25 * var25 >= newPos.x * newPos.x + newPos.z * newPos.z) {
				newPos.x = var37;
				newPos.y = var23;
				newPos.z = var25;
				m_hitbox = var27;
			}
			else {
				real var40 = m_hitbox.min.y - ((int)m_hitbox.min.y);
				if (var40 > 0.0) {
					m_ySlideOffset = m_ySlideOffset + var40 + 0.01;
				}
			}
		}

		m_pos.x = (m_hitbox.min.x + m_hitbox.max.x) / 2.0;
		m_pos.y = m_hitbox.min.y + m_heightOffset - m_ySlideOffset;
		m_pos.z = (m_hitbox.min.z + m_hitbox.max.z) / 2.0;
		m_bHorizontalCollision = cPosX != newPos.x || cPosZ != newPos.z;
		m_bVerticalCollision = cPosY != newPos.y;
		m_onGround = cPosY != newPos.y && cPosY < 0.0;
		m_bCollision = m_bHorizontalCollision || m_bVerticalCollision;
		checkFallDamage(newPos.y, m_onGround);
		if (cPosX != newPos.x) {
			m_vel.x = 0.0;
		}

		if (cPosY != newPos.y) {
			m_vel.y = 0.0;
		}

		if (cPosZ != newPos.z) {
			m_vel.z = 0.0;
		}

		var37 = m_pos.x - aPosX;
		var23 = m_pos.z - aPosZ;
		if (m_bMakeStepSound && !validSneaking) {
			m_walkDist = float(m_walkDist + Mth::sqrt(var37 * var37 + var23 * var23) * 0.6);
			TilePos tp(m_pos.x, m_pos.y - real(0.2) - m_heightOffset, m_pos.z);
			var28 = m_pLevel->getTile(tp);
			if (m_walkDist > m_nextStep && var28 > 0) {
				m_nextStep = m_walkDist + 1;
				if (m_pLevel->getTile(tp.above()) == Tile::topSnow->m_ID) {
					auto sound = Tile::topSnow->m_pSound;
					m_pLevel->playSound(this, sound->m_name, sound->volume * 0.15F, sound->pitch);
				}
				else if (!Tile::tiles[var28]->m_pMaterial->isLiquid()) {
					auto sound = Tile::tiles[var28]->m_pSound;
					m_pLevel->playSound(this, sound->m_name, sound->volume * 0.15F, sound->pitch);
				}

				Tile::tiles[var28]->stepOn(m_pLevel, tp, this);
			}
		}

		TilePos minPos(m_hitbox.min + 0.001);
		TilePos maxPos(m_hitbox.max - 0.001);
		TilePos tilePos;

		if (m_pLevel->hasChunksAt(minPos, TilePos(maxPos)))
		{
			for (tilePos.x = minPos.x; tilePos.x <= maxPos.x; tilePos.x++)
				for (tilePos.y = minPos.y; tilePos.y <= maxPos.y; tilePos.y++)
					for (tilePos.z = minPos.z; tilePos.z <= maxPos.z; tilePos.z++)
					{
						TileID tileID = m_pLevel->getTile(tilePos);
						if (tileID > 0)
							Tile::tiles[tileID]->entityInside(m_pLevel, tilePos, this);
					}
		}

		bool bIsInWater = wasInWater();

		if (m_pLevel->containsFireTile(m_hitbox))
		{
			burn(1);

			if (!bIsInWater)
			{
				m_fireTicks++;
				if (m_fireTicks == 0)
					m_fireTicks = 300;
			}
		}
		else if (m_fireTicks <= 0)
		{
			m_fireTicks = -m_flameTime;
		}

		if (bIsInWater && m_fireTicks > 0)
		{
			m_pLevel->playSound(this, "random.fizz", 0.7f, 1.6f + (m_random.nextFloat() - m_random.nextFloat()) * 0.4f);
			m_fireTicks = -m_flameTime;
		}

	}

    return 1300;
}

void Entity::moveTo(const Vec3& pos, const Vec2& rot)
{
	Vec3 newPos(pos);
	newPos.y += m_heightOffset;

	setPos(newPos);
	m_oPos = newPos;
	m_posPrev = newPos;

	m_rot = rot;
}

void Entity::absMoveTo(const Vec3& pos, const Vec2& rot)
{
	m_ySlideOffset = 0.0f;

	m_rotPrev = rot;
	setRot(rot);

	setPos(pos);
	m_oPos = pos;

	// This looks like a rebounding check for the angle
	float dyRot = (m_rotPrev.x - m_rot.x);
	if (dyRot < -180.0f)
		m_rotPrev.x += 360.0f;
	if (dyRot >= 180.0f)
		m_rotPrev.x -= 360.0f;
}

void Entity::moveRelative(const Vec3& pos)
{
	real d = Mth::sqrt(pos.x * pos.x + pos.z * pos.z);
	if (d < 0.01f) return;
	if (d < 1.0f)
		d = 1.0f;

	Vec3 vel(pos);

	vel.y /= d;
	vel.x *= vel.y;
	vel.z *= vel.y;

	real yaw = m_rot.y * float(M_PI);
	real syaw = sinf(yaw / 180.0f);
	real cyaw = cosf(yaw / 180.0f);

	m_vel.x += vel.x * cyaw - vel.z * syaw;
	m_vel.z += vel.x * syaw + vel.z * cyaw;
}

void Entity::lerpTo(const Vec3& pos)
{
	setPos(pos);
}

void Entity::lerpTo(const Vec3& pos, const Vec2& rot, int p)
{
	lerpTo(pos);
	setRot(rot);
}

void Entity::lerpMotion(const Vec3& pos)
{
	m_vel = pos;
}

void Entity::turn(const Vec2& rot)
{
	if (rot == Vec2::ZERO) return;

	Vec2 rotOld = m_rot;

	interpolateTurn(rot);

	m_rotPrev.y += m_rot.y - rotOld.y;
	m_rotPrev.x += m_rot.x - rotOld.x;
}

void Entity::reset()
{
	// TODO is this it
	m_posPrev = m_oPos = m_pos;
	m_rotPrev = m_rot;
	m_bRemoved = false;
	m_distanceFallen = 0.0f;
	m_bFireImmune = false;
	m_fireTicks = 0;
}

void Entity::interpolateTurn(const Vec2& rot)
{
	setRot(Vec2(
		m_rot.y + rot.y * 0.15f,
		m_rot.x - rot.x * 0.15f
	));

	// can't rotate more than facing fully up or fully down
	if (m_rot.x < -90.0f)
		m_rot.x = -90.0f;
	if (m_rot.x > 90.0f)
		m_rot.x = 90.0f;
}

void Entity::tick()
{
	baseTick();
}

void Entity::baseTick()
{
	//@TODO: untangle the gotos

	field_90 = m_walkDist;
	m_oPos = m_pos;
    m_tickCount++;
	m_rotPrev = m_rot;
	if (isInWater())
	{
		if (!m_bWasInWater && !m_bFirstTick)
		{
			float dist = Mth::sqrt(m_vel.y * m_vel.y + m_vel.x * m_vel.x * 0.2f + m_vel.z * m_vel.z * 0.2f) * 0.2f;
			if (dist > 1.0f)
				dist = 1.0f;

			m_pLevel->playSound(this, "random.splash", dist, 1.0f + 0.4f * (m_random.nextFloat() - m_random.nextFloat()));

			float f1 = floorf(m_hitbox.min.y);

			for (int i = 0; float(i) < m_bbWidth * 20.0f + 1.0f; i++)
			{
				m_pLevel->addParticle(
					"bubble",
					Vec3(
						m_pos.x + m_bbWidth * (m_random.nextFloat() * 2.0f - 1.0f),
						f1 + 1.0f,
						m_pos.z + m_bbWidth * (m_random.nextFloat() * 2.0f - 1.0f)
					),
					Vec3(
						m_vel.x,
						m_vel.y - 0.2f * m_random.nextFloat(),
						m_vel.z
					)
				);
			}

			for (int i = 0; float(i) < m_bbWidth * 20.0f + 1.0f; i++)
			{
				m_pLevel->addParticle(
					"splash",
					Vec3(
						m_pos.x + m_bbWidth * (m_random.nextFloat() * 2.0f - 1.0f),
						f1 + 1.0f,
						m_pos.z + m_bbWidth * (m_random.nextFloat() * 2.0f - 1.0f)
					),
					Vec3(
						m_vel.x,
						m_vel.y - 0.2f * m_random.nextFloat(),
						m_vel.z
					)
				);
			}
		}

		m_bWasInWater = true;
		m_fireTicks = 0;
		m_distanceFallen = 0;

		if (m_pLevel->m_bIsOnline)
			goto label_4;
	}
	else
	{
		m_bWasInWater = false;

		if (m_pLevel->m_bIsOnline)
		{
		label_4:
			m_fireTicks = 0;
			goto label_5;
		}
	}

	if (m_fireTicks <= 0)
	{
	label_5:
		if (!isInLava())
			goto label_6;
		goto label_15;
	}

	if (m_bFireImmune)
	{
		m_fireTicks -= 4;
		if (m_fireTicks < 0)
			m_fireTicks = 0;
		goto label_5;
	}

	if (m_fireTicks % 20 == 0)
	{
		hurt(nullptr, 1);
	}

	m_fireTicks--;

	if (isInLava())
	{
	label_15:
		lavaHurt();
	}

label_6:
	if (m_pos.y < -64.0f)
		outOfWorld();

	m_bFirstTick = false;
}

bool Entity::intersects(const Vec3& min, const Vec3& max) const
{
	AABB aabb(min, max);
	return aabb.intersect(m_hitbox);
}

bool Entity::isFree(const Vec3& off) const
{
	AABB aabb = m_hitbox;
	aabb.move(off);

	AABBVector* pCubes = m_pLevel->getCubes(this, aabb);
	if (pCubes->size() > 0)
		return false;

	return !m_pLevel->containsAnyLiquid(aabb);
}

bool Entity::isFree(const Vec3& off, float expand) const
{
	AABB aabb = m_hitbox;
	aabb.move(off);
	aabb.grow(expand, expand, expand);

	AABBVector* pCubes = m_pLevel->getCubes(this, aabb);
	if (pCubes->size() > 0)
		return false;

	return !m_pLevel->containsAnyLiquid(aabb);
}

bool Entity::isInWall() const
{
	return m_pLevel->isSolidTile(TilePos(m_pos.x, m_pos.y + getHeadHeight(), m_pos.z));
}

bool Entity::isInWater()
{
	AABB aabb = m_hitbox;
	aabb.grow(0, -0.4f, 0);
	aabb.grow(-0.001, -0.001, -0.001);
	return m_pLevel->checkAndHandleWater(aabb, Material::water, this);
}

bool Entity::wasInWater()
{
	return m_bWasInWater;
}

bool Entity::isInLava() const
{
	AABB aabb = m_hitbox;
	aabb.grow(-0.1f, -0.4f, -0.1f);
	return m_pLevel->containsMaterial(aabb, Material::lava);
}

bool Entity::isUnderLiquid(Material* pMtl) const
{
	TilePos tilePos(m_pos.x, m_pos.y + getHeadHeight(), m_pos.z);

	Tile* pTile = Tile::tiles[m_pLevel->getTile(tilePos)];
	if (!pTile || pTile->m_pMaterial != pMtl)
		return false;

	int data = m_pLevel->getData(tilePos);
	int level = data <= 7 ? data + 1 : 1;

	return float(tilePos.y) < float(tilePos.y + 1) - (float(level) / 9.0f - 0.11111f);
}

float Entity::getBrightness(float f) const
{
	TilePos tilePos(m_pos.x, m_pos.y - m_heightOffset + (m_hitbox.max.y - m_hitbox.min.y) * 0.66f, m_pos.z);

	TilePos tileMin(m_hitbox.min);
	TilePos tileMax(m_hitbox.max);

	if (!m_pLevel->hasChunksAt(tileMin, tileMax))
		return 0;

	return Mth::Max(m_minBrightness, m_pLevel->getBrightness(tilePos));
}

float Entity::distanceTo(Entity* pEnt) const
{
	return distanceTo(pEnt->m_pos);
}

float Entity::distanceTo(const Vec3& pos) const
{
	return m_pos.distanceTo(pos);
}

float Entity::distanceToSqr(Entity* pEnt) const
{
	return distanceToSqr(pEnt->m_pos);
}

float Entity::distanceToSqr(const Vec3& pos) const
{
	return m_pos.distanceToSqr(pos);
}

int Entity::interactPreventDefault()
{
	return 0;
}

bool Entity::interact(Player* player)
{
	return false;
}

void Entity::playerTouch(Player* player)
{

}

void Entity::push(Entity* bud)
{
	real diffX = bud->m_pos.x - m_pos.x;
	real diffZ = bud->m_pos.z - m_pos.z;
	real maxDiff = Mth::absMax(diffX, diffZ);

	if (maxDiff < 0.01f) return;

	real x1 = Mth::sqrt(maxDiff);
	real x2 = 1.0f / x1;
	if (x2 > 1.0f)
		x2 = 1.0f;
	real x3 = 1.0f - this->m_pushThrough;
	real x4 = x3 * diffX / x1 * x2 * 0.05f;
	real x5 = x3 * diffZ / x1 * x2 * 0.05f;

	push(Vec3(-x4, 0.0f, -x5));
	bud->push(Vec3(x4, 0.0f, x5));
}

void Entity::push(const Vec3& pos)
{
	m_vel += pos;
}

bool Entity::shouldRender(Vec3& camPos) const
{
	return shouldRenderAtSqrDistance(distanceToSqr(camPos));
}

bool Entity::shouldRenderAtSqrDistance(float distSqr) const
{
	float maxDist = (m_noNeighborUpdate * 64.0f) * (((m_hitbox.max.z - m_hitbox.min.z) + (m_hitbox.max.x - m_hitbox.min.x) + (m_hitbox.max.y - m_hitbox.min.y)) / 3.0f);

	return maxDist * maxDist > distSqr;
}

bool Entity::hurt(Entity* pAttacker, int damage)
{
	markHurt();
	return 0;
}

void Entity::animateHurt()
{

}

std::shared_ptr<ItemEntity> Entity::spawnAtLocation(ItemInstance* itemInstance, float y)
{
	auto itemEntity = std::make_shared<ItemEntity>(m_pLevel, Vec3(m_pos.x, m_pos.y + y, m_pos.z), itemInstance);
	delete(itemInstance);
	itemEntity->m_throwTime = 10;
	m_pLevel->addEntity(itemEntity);
	
	return itemEntity;
}

std::shared_ptr<ItemEntity> Entity::spawnAtLocation(int itemID, int amount)
{
	return spawnAtLocation(itemID, amount, 0);
}

std::shared_ptr<ItemEntity> Entity::spawnAtLocation(int itemID, int amount, float y)
{
	return spawnAtLocation(new ItemInstance(itemID, amount, 0), y);
}

void Entity::awardKillScore(Entity* pKilled, int score)
{

}

void Entity::setEquippedSlot(int a, int b, int c)
{

}

void Entity::setRot(const Vec2& rot)
{
	m_rot.y = /*Mth::abs(rot.x) > 360.0f ? fmod(rot.x, 360.0f) :*/ rot.y;
	m_rot.x = /*Mth::abs(rot.y) > 360.0f ? fmod(rot.y, 360.0f) :*/ rot.x;
}

void Entity::setSize(float rad, float height)
{
	m_bbWidth = rad;
	m_bbHeight = height;
}

void Entity::setPos(EntityPos* pPos)
{
	if (pPos->m_bHasPos)
		setPos(pPos->m_pos);
	else
		setPos(m_pos);

	if (pPos->m_bHasRot)
		setRot(pPos->m_rot);
	else
		setRot(m_rot);
}

void Entity::resetPos()
{
	if (!m_pLevel)
		// No level?  Fine
		return;

	if (m_pos.y <= 0.0f)
		// It's in the void.  It's probably not inside of any tiles.
		return;

	// Push the entity up while it's inside solid tiles.
	while (true)
	{
		setPos(m_pos);

		AABBVector* pCubes = m_pLevel->getCubes(this, m_hitbox);

		// if we aren't inside any tiles, great!
		if (!pCubes->size())
			break;

		m_pos.y += 1.0f;
	}

	m_vel = Vec3::ZERO;
	m_rot.x = 0.0f;
}

void Entity::outOfWorld()
{
	remove();
}

void Entity::checkFallDamage(float fDeltaY, bool bHitGround)
{
	if (bHitGround)
	{
		if (m_distanceFallen > 0.0f)
		{
			causeFallDamage(m_distanceFallen);
			m_distanceFallen = 0.0f;
		}
	}
	else if (fDeltaY < 0.0f)
	{
		m_distanceFallen -= fDeltaY;
	}
}

void Entity::causeFallDamage(float f)
{
	// stub
}

void Entity::markHurt()
{
	m_bHurt = true;
}

void Entity::burn(int x)
{
	if (!m_bFireImmune)
		hurt(nullptr, 4);
}

void Entity::lavaHurt()
{
	if (!m_bFireImmune)
	{
		hurt(nullptr, 4);
		m_fireTicks = 600;
	}
}

int Entity::queryEntityRenderer()
{
	// If field_C8 is equal to RENDER_DYNAMIC, EntityRenderDispatcher
	// calls here. Used for sheared sheep.
	return 0;
}

void Entity::handleEntityEvent(int event)
{
}

void Entity::load(std::shared_ptr<CompoundTag> tag)
{
	std::shared_ptr<ListTag> var2 = tag->getList("Pos");
	std::shared_ptr<ListTag> var3 = tag->getList("Motion");
	std::shared_ptr<ListTag> var4 = tag->getList("Rotation");
	setPos(Vec3::ZERO);
	m_vel.x = std::dynamic_pointer_cast<DoubleTag>(var3->getValue().at(0))->getValue();
	m_vel.y = std::dynamic_pointer_cast<DoubleTag>(var3->getValue().at(1))->getValue();
	m_vel.z = std::dynamic_pointer_cast<DoubleTag>(var3->getValue().at(2))->getValue();
	if (Mth::abs(m_vel.x) > 10.0) {
		m_vel.x = 0.0;
	}

	if (Mth::abs(m_vel.y) > 10.0) {
		m_vel.y = 0.0;
	}

	if (Mth::abs(m_vel.z) > 10.0) {
		m_vel.z = 0.0;
	}
	m_posPrev.x = m_oPos.x = m_pos.x = std::dynamic_pointer_cast<DoubleTag>(var2->getValue().at(0))->getValue();
	m_posPrev.y = m_oPos.y = m_pos.y = std::dynamic_pointer_cast<DoubleTag>(var2->getValue().at(1))->getValue();
	m_posPrev.z = m_oPos.z = m_pos.z = std::dynamic_pointer_cast<DoubleTag>(var2->getValue().at(2))->getValue();
	m_rotPrev.y = m_rot.y = std::dynamic_pointer_cast<FloatTag>(var4->getValue().at(0))->getValue();
	m_rotPrev.x = m_rot.x = std::dynamic_pointer_cast<FloatTag>(var4->getValue().at(1))->getValue();
	m_distanceFallen = tag->getFloat("FallDistance");
	m_fireTicks = tag->getShort("Fire");
	m_airSupply = tag->getShort("Air");
	m_onGround = tag->getBoolean("OnGround");
	setPos(m_pos);
	readAdditionalSaveData(tag);
}

bool Entity::save(std::shared_ptr<CompoundTag> tag)
{
	std::string var2 = getEncodeId();
	if (!m_bRemoved && !var2.empty()) {
		tag->putString("id", var2);
		saveWithoutId(tag);
		return true;
	}
	else {
		return false;
	}
}

void Entity::saveWithoutId(std::shared_ptr<CompoundTag> tag)
{
	tag->putDoubleList("Pos", { m_pos.x, m_pos.y + m_ySlideOffset, m_pos.z });
	tag->putDoubleList("Motion", { m_vel.x, m_vel.y, m_vel.z });
	tag->putFloatList("Rotation", {m_rot.y, m_rot.x});
	tag->putFloat("FallDistance", m_distanceFallen);
	tag->putShort("Fire", m_fireTicks);
	tag->putShort("Air", m_airSupply);
	tag->putBoolean("OnGround", m_onGround);
	addAdditionalSaveData(tag);
}

void Entity::addAdditionalSaveData(std::shared_ptr<CompoundTag> tag)
{
}

void Entity::readAdditionalSaveData(std::shared_ptr<CompoundTag> tag)
{
}

std::string Entity::getEncodeId()
{
	return getType().getName();
}

bool Entity::operator==(const Entity& other) const
{
	return m_EntityID == other.m_EntityID;
}

void Entity::checkInTile(const Vec3& pos)
{
	TilePos flPos = pos;

	if (!Tile::solid[m_pLevel->getTile(pos)])
		return;

	bool solidXN = Tile::solid[m_pLevel->getTile(flPos.west())];
	bool solidXP = Tile::solid[m_pLevel->getTile(flPos.east())];
	bool solidYN = Tile::solid[m_pLevel->getTile(flPos.below())];
	bool solidYP = Tile::solid[m_pLevel->getTile(flPos.above())];
	bool solidZN = Tile::solid[m_pLevel->getTile(flPos.north())];
	bool solidZP = Tile::solid[m_pLevel->getTile(flPos.south())];

	real mindist = 9999.0;
	int mindir = -1;

	Vec3 diff = pos - flPos;
	if (!solidXN && diff.x < mindist) mindist = diff.x, mindir = 0;
	if (!solidXP && 1.0f - diff.x < mindist) mindist = 1.0f - diff.x, mindir = 1;
	if (!solidYN && diff.y < mindist) mindist = diff.y, mindir = 2;
	if (!solidYP && 1.0f - diff.y < mindist) mindist = 1.0f - diff.y, mindir = 3;
	if (!solidZN && diff.z < mindist) mindist = diff.z, mindir = 4;
	if (!solidZP && 1.0f - diff.z < mindist) mindist = 1.0f - diff.z, mindir = 5;

	// the -1 case will be handled accordingly
	float force = 0.1f + 0.2f * m_random.nextFloat();
	switch (mindir)
	{
	case 0: m_vel.x = -force; break;
	case 1: m_vel.x = force; break;
	case 2: m_vel.y = -force; break;
	case 3: m_vel.y = force; break;
	case 4: m_vel.z = -force; break;
	case 5: m_vel.z = force; break;
	}
}
