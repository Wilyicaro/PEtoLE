/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Entity.hpp"

#define C_MAX_MOB_HEALTH (20)

class Mob : public Entity
{
public:
	Mob(Level* pLevel);
	virtual ~Mob();

	//overrides
	virtual void reset() override;
	virtual void lerpTo(const Vec3& pos, const Vec2& rot, int steps) override;
	virtual void tick() override;
	virtual void baseTick() override;
	virtual float getHeadHeight() const override { return 0.85f * m_bbHeight; }
	virtual bool isPickable() const override { return !m_bRemoved; }
	virtual bool isPushable() const override { return !m_bRemoved; }
	virtual bool isShootable() const override { return true; }
	virtual bool isAlive() const override;
	virtual bool hurt(Entity*, int) override;
	virtual void animateHurt() override;
	virtual void setSize(float rad, float height) override;
	virtual void outOfWorld() override;
	virtual void causeFallDamage(float level) override;
	virtual void handleEntityEvent(int event) override;

	//virtuals
	virtual void knockback(Entity* pEnt, int a, float x, float z);
	virtual void die(Entity* pCulprit);
	virtual bool canSee(Entity* pEnt) const;
	virtual bool onLadder() const;
	virtual void spawnAnim();
	virtual std::string getTexture() const;
	virtual bool isWaterMob() const { return false; }
	virtual void playAmbientSound();
	virtual int getAmbientSoundInterval() const;
	virtual void superTick() { Entity::tick(); }
	virtual void heal(int health);
	virtual HitResult pick(float, float);
	virtual void travel(const Vec2& pos);
	virtual void updateWalkAnim();
	virtual void aiStep();
	virtual void addAdditionalSaveData(std::shared_ptr<CompoundTag> tag) override;
	virtual void readAdditionalSaveData(std::shared_ptr<CompoundTag> tag) override;
	virtual void lookAt(Entity* pEnt, float, float);
	virtual bool isLookingAtAnEntity() { return m_pEntLookedAt != nullptr; }
	virtual Entity* getLookingAt() const { return m_pEntLookedAt; }
	virtual void beforeRemove() { }
	virtual bool canSpawn();
	virtual float getAttackAnim(float f) const;
	virtual Vec3 getPos(float f) const;
	virtual Vec3 getLookAngle(float f) const { return getViewVector(1.0f); }
	virtual Vec3 getViewVector(float f) const;
	virtual int getMaxSpawnClusterSize() const { return 4; }
	virtual bool isBaby() const { return false; }
	virtual void actuallyHurt(int damage);
	virtual bool removeWhenFarAway() const { return true; }
	virtual int getDeathLoot() const { return 0; }
	virtual void dropDeathLoot();
	virtual bool isImmobile() const { return m_health <= 0; }
	virtual void jumpFromGround();
	virtual void updateAi();
	virtual int getMaxHeadXRot() const { return 10; }
	virtual int getMaxHealth() const { return 10; }
	virtual float getSoundVolume() const { return 1.0f; }
	virtual std::string getAmbientSound() const { return ""; }
	virtual std::string getHurtSound() const { return "random.hurt"; }
	virtual std::string getDeathSound() const { return "random.hurt"; }
	virtual float getWalkingSpeedModifier() const { return 0.7f; }
	virtual void checkDespawn(Mob* nearestMob);
	virtual void checkDespawn();
	virtual void swing();

	float rotlerp(float, float, float);
	void updateAttackAnim();

public:
	int m_invulnerableDuration;
	float field_E0;
	float field_E4;
	float m_yBodyRot;
	float m_yBodyRotO;
	char m_bInterpolateOnly;
	float m_oAttackAnim;
	float m_attackAnim;
	int m_health;
	int m_lastHealth;
	int m_ambientSoundTime;
	int m_hurtTime;
	int m_hurtDuration;
	float m_hurtDir;
	int m_deathTime;
	int m_attackTime;
	float m_oTilt;
	float m_tilt;
	int field_120;
	int field_124;
	float field_128;
	float m_walkAnimSpeed;
	float field_130;
	Random m_random;
	int m_noActionTime;
	Vec2 m_moving;
	float m_yRotA;
	bool m_bJumping;
	float field_B10;
	float m_runSpeed;
	std::string m_skinUrl;
	std::string m_texture;
	int field_B48;
	float field_B4C;
	float field_B50;
	float m_animStep;
	float m_animStepO;
	float m_rotOffs;
	float field_B60;
	int field_B64;
	char field_B68;
	char field_B69;
	int m_lSteps;
	Vec3 m_lPos;
	Vec2 m_lRot;
	int m_lastHurt;
	Entity* m_pEntLookedAt;

	bool m_bSwinging;
	int m_swingTime;
};
