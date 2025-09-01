/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include "Animal.hpp"

class Wolf : public Animal
{
public:
	Wolf(Level* pLevel);

	void defineSynchedData() override;
	std::string getTexture() const override { return isTamed() ? "mob/wolf_tame.png" : (isAngry() ? "mob/wolf_angry.png" : Animal::getTexture()); };
	std::string getAmbientSound() override { return isAngry() ? "mob.wolf.growl" : (m_random.nextInt(3) == 0 ? (isTamed() && m_entityData.get<int>(18) < 10 ? "mob.wolf.whine" : "mob.wolf.panting") : "mob.wolf.bark"); };
	std::string getDeathSound() const override { return "mob.wolf.death"; }
	std::string getHurtSound() const override { return "mob.wolf.hurt"; }
	int getDeathLoot() const override;
	int getMaxHealth() const override { return 20; }
	int getMaxSpawnClusterSize() const override { return 8; }
	bool interact(Player*) override;
	void addAdditionalSaveData(CompoundIO tag) override;
	void readAdditionalSaveData(CompoundIO tag) override;
	bool canDespawn() const override;;
	void updateAi() override;
	void aiStep() override;
	void tick() override;
	float getHeadHeight() const override;
	int getMaxHeadXRot() const override;
	bool isImmobile() const override;
	void checkHurtTarget(Entity*, float) override;
	std::shared_ptr<Entity> findAttackTarget() override;
	bool hurt(Entity*, int) override;
	void handleEntityEvent(int event) override;

	float getWetShade(float) const;
	float getBodyRollAngle(float, float) const;
	float getHeadRollAngle(float) const;
	float getTailAngle() const;
	bool isShaking() const;
	std::string getOwner();
	void setOwner(std::string);
	bool isAngry() const;
	void setAngry(bool b);
	bool isTamed() const;
	void setTamed(bool b);
	bool isOrderedToSit() const;
	void setOrderedToSit(bool b);

	Entity* getBreedOffspring(Animal* pOther) { return new Wolf(m_pLevel); }

private:
	void updateAttackTarget(Entity*, float);
	void addTamingParticles(bool);

private:
	bool m_bInterested = false;
	float m_interestedAngle = 0.0f;
	float m_interestedAngleO = 0.0f;
	bool m_bIsShaking = false;
	bool m_bIsWet = false;
	float m_shakeAnim = 0.0f;
	float m_shakeAnimO = 0.0f;
};
