/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "world/phys/Vec3T.hpp"
#include "world/phys/Vec2.hpp"
#include "world/phys/AABB.hpp"
#include "world/level/Material.hpp"
#include "world/level/levelgen/chunk/ChunkPos.hpp"
#include "world/tile/Tile.hpp"
#include "world/item/ItemInstance.hpp"
#include "SynchedEntityData.hpp"
#include "EntityType.hpp"
#include "common/Utils.hpp"
#include "common/CompoundTag.hpp"
#include <string>

class Level;
class MinecraftServer;
class Player;
class Mob;
class ItemInstance;
class ItemEntity;
class LightningBolt;
class RakNetInstance;
struct EntityPos
{
	Vec3 m_pos;
	Vec2 m_rot;
	bool m_bHasRot, m_bHasPos;

	EntityPos()
		: m_pos(Vec3::ZERO), m_rot(Vec2::ZERO)
	{
        m_bHasRot = false; m_bHasPos = false;
	};

	EntityPos(const Vec3& pos)
		: m_pos(pos), m_rot(Vec2::ZERO)
	{
		m_bHasPos = true; m_bHasRot = false;
	}

	EntityPos(const Vec2& rot)
		: m_pos(Vec3::ZERO), m_rot(rot)
	{
		m_bHasPos = false; m_bHasRot = true;
	}

	EntityPos(const Vec3& pos, const Vec2& rot)
		: m_pos(pos), m_rot(rot)
	{
		m_bHasPos = true; m_bHasRot = true;
	}
};

class Entity : public std::enable_shared_from_this<Entity>
{
private:
	void _init();
public:
	Entity() { _init(); }
	Entity(Level*);
	virtual ~Entity();
	virtual void reset();
	virtual void setLevel(Level*);
	virtual void removed();
	virtual void setPos(const Vec3& pos);
	virtual void remove();
	virtual int move(const Vec3& pos);
	virtual void moveTo(const Vec3& pos, const Vec2& rot = Vec2::ZERO);
	virtual void absMoveTo(const Vec3& pos, const Vec2& rot = Vec2::ZERO);
	virtual void moveRelative(const Vec3& pos);
	virtual void lerpTo(const Vec3& pos);
	virtual void lerpTo(const Vec3& pos, const Vec2& rot, int i);
	virtual void lerpMotion(const Vec3& pos);
	virtual void turn(const Vec2& rot);
	virtual void interpolateTurn(const Vec2& rot);
	virtual void tick();
	virtual void baseTick();
	virtual void rideTick();
	virtual void positionRider();
	virtual real getRidingHeight();
	virtual real getRideHeight();
	virtual void ride(std::shared_ptr<Entity>);
	virtual bool intersects(const Vec3& min, const Vec3& max) const;
	virtual bool isFree(const Vec3& off) const;
	virtual bool isFree(const Vec3& off, float expand) const;
	virtual bool isInWall() const;
	virtual bool checkInWater();
	virtual bool isWet();
	virtual bool wasInWater();
	virtual bool isInLava() const;
	virtual bool isUnderLiquid(Material*) const;
	virtual float getHeadHeight() const { return m_heightOffset; }
	virtual float getShadowHeightOffs() const { return m_bbHeight / 2.0f; }
	virtual float getBrightness(float f) const;
	virtual float distanceTo(Entity*) const;
	virtual float distanceToSqr(const Vec3& pos) const;
	virtual float distanceTo(const Vec3& pos) const;
	virtual float distanceToSqr(Entity*) const;
	virtual int interactPreventDefault();
	virtual bool interact(Player*);
	virtual void playerTouch(Player*);
	virtual void push(Entity*);
	virtual void push(const Vec3& pos);
	virtual bool isPickable() const { return false; }
	virtual bool isPushable() const { return false; }
	virtual bool isShootable() const { return false; }
	virtual bool isSneaking() const { return getSharedFlag(1); }
	virtual void setSneaking(bool);
	virtual bool isAlive() const { return m_bRemoved; }
	virtual bool isOnFire() const { return m_fireTicks > 0 || getSharedFlag(0); }
	virtual bool isPlayer() const { return getType() == EntityType::player; }
	virtual bool isCreativeModeAllowed() const { return false; }
	virtual bool shouldRender(Vec3& camPos) const;
	virtual bool shouldRenderAtSqrDistance(real distSqr) const;
	virtual bool hurt(Entity*, int);
	virtual bool getSharedFlag(int) const;
	virtual void setSharedFlag(int, bool);
	virtual void animateHurt();
	virtual float getPickRadius() const { return 0.1f; }
	virtual Vec3 getLookAngle() const { return Vec3::ZERO; }
	virtual bool isRiding() { return m_pRiding != nullptr || getSharedFlag(2); }
	virtual std::shared_ptr<ItemEntity> spawnAtLocation(std::shared_ptr<ItemInstance>, float);
	virtual std::shared_ptr<ItemEntity> spawnAtLocation(int, int);
	virtual std::shared_ptr<ItemEntity> spawnAtLocation(int, int, float);
	virtual void awardKillScore(Entity* pKilled, int score);
	virtual void setItemSlot(int slot, int item, int aux);
	virtual void setRot(const Vec2& rot);
	virtual void setSize(float rad, float height);
	virtual void setPos(EntityPos*);
	virtual void resetPos();
	virtual void outOfWorld();
	virtual void checkFallDamage(float f, bool b);
	virtual void causeFallDamage(float f);
	virtual void markHurt();
	virtual void burn(int);
	virtual void lavaHurt();
	virtual const AABB* getCollideBox() const;
	virtual AABB* getCollideAgainstBox(Entity* ent) const;
	virtual void handleInsidePortal();
	virtual void handleEntityEvent(EntityEvent event);
	virtual void thunderHit(LightningBolt*);
	virtual void killed(Mob*);
	void load(CompoundIO tag);
	bool save(CompoundIO tag);
	void saveWithoutId(CompoundIO tag);
	virtual void addAdditionalSaveData(CompoundIO tag);
	virtual void readAdditionalSaveData(CompoundIO tag);
	virtual void defineSynchedData();
	void startSynchedData();
	std::string getEncodeId();
	virtual std::array<std::shared_ptr<ItemInstance>, 5>* getEquipmentSlots();

	const EntityType* getType() const { return m_pType; }
	const EntityCategories getCategory() const { return getType()->getCategory(); }
	SynchedEntityData& getEntityData() const { return m_entityData; }
	MinecraftServer* getServer();
	//TODO: Replace this with something more accurate
	RakNetInstance* getConnection();

	int hashCode() const { return m_EntityID; }

	bool operator==(const Entity& other) const;

	void checkInTile(const Vec3& pos);

	float distanceToSqr_inline(const Vec3& pos) const
	{
		return
			(m_pos.x - pos.x) * (m_pos.x - pos.x) +
			(m_pos.y - pos.y) * (m_pos.y - pos.y) +
			(m_pos.z - pos.z) * (m_pos.z - pos.z);
	}

public:
	static int entityCounter;
	Random m_random;

	std::shared_ptr<Entity> m_pRider;
	std::shared_ptr<Entity> m_pRiding;
	Vec2 m_rideRotA;
	Vec3 m_pos;
	bool m_bInChunk;
	ChunkPos m_chunkPos;
	int m_chunkPosY;
	Vec3i m_pPos;
	int field_28;
	int m_EntityID;
	float m_viewScale;
	bool m_bBlocksBuilding;
	Level* m_pLevel;
	Vec3 m_oPos; // "o" in Java or "xo" ""yo" "zo"
	Vec3 m_vel;
	Vec2 m_rot;
	//maybe these are the actual m_yaw and m_pitch, and
	//the one I annotated are the destination yaw and pitch.
	//interpolateTurn doesn't modify them, so I highly suspect
	//this to be the case.
	Vec2 m_rotPrev;
	AABB m_hitbox;
	bool m_definedData;
	bool m_bOnGround;
	bool m_bHorizontalCollision;
	bool m_bVerticalCollision;
	bool m_bCollision;
	bool m_bHurt;
	bool m_bInWeb;
	bool m_bSlide;
	bool m_bRemoved;
	float m_heightOffset;
	float m_bbWidth;
	float m_bbHeight;
	float m_walkDistO;
	float m_walkDist;
	Vec3 m_posPrev;
	float m_ySlideOffset;
	float m_footSize;
	bool m_bNoPhysics;
	bool m_bNoMove;
	float m_pushThrough;
	int m_tickCount;
	int m_invulnerableTime;
	int m_airCapacity;
	int m_fireTicks;
	int m_flameTime;
	float m_distanceFallen; // Supposed to be protected
	int m_airSupply;
	bool m_bWasInWater;
	bool m_bFireImmune;
	bool m_bFirstTick;
	bool m_bIgnoreFrustum;
	int m_nextStep;
	float m_minBrightness;

protected:
	mutable SynchedEntityData m_entityData;
	bool m_bMakeStepSound;
	const EntityType* m_pType;
};
