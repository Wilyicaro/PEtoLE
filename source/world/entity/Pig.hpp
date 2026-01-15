/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include "Animal.hpp"

class Pig : public Animal
{
public:
	Pig(Level* pLevel);

	virtual void defineSynchedData() override;
	virtual std::string getAmbientSound() override { return "mob.pig"; };
	virtual std::string getDeathSound() const override { return "mob.pigdeath"; }
	virtual std::string getHurtSound() const override { return "mob.pig"; }
	virtual int getDeathLoot() const override;
	virtual int getMaxHealth() const override { return 10; }
	virtual bool interact(Player*) override;
	virtual void addAdditionalSaveData(CompoundIO tag) override;
	virtual void readAdditionalSaveData(CompoundIO tag) override;
	virtual void thunderHit(LightningBolt*) override;
	virtual void causeFallDamage(float) override;

	bool hasSaddle() const;
	void setSaddle(bool b);
	Entity* getBreedOffspring(Animal* pOther) { return new Pig(m_pLevel); }
};
