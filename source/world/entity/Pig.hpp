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

	void defineSynchedData() override;
	std::string getAmbientSound() override { return "mob.pig"; };
	std::string getDeathSound() const override { return "mob.pigdeath"; }
	std::string getHurtSound() const override { return "mob.pig"; }
	int getDeathLoot() const override;
	int getMaxHealth() const override { return 10; }
	bool interact(Player*) override;
	void addAdditionalSaveData(std::shared_ptr<CompoundTag> tag) override;
	void readAdditionalSaveData(std::shared_ptr<CompoundTag> tag) override;

	bool hasSaddle() const;
	void setSaddle(bool b);
	Entity* getBreedOffspring(Animal* pOther) { return new Pig(m_pLevel); }
};
