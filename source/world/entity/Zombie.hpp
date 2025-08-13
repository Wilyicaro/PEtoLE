#pragma once

#include "world/entity/Monster.hpp"

class Zombie : public Monster
{
public:
	Zombie(Level* pLevel);

	std::string getAmbientSound() override { return "mob.zombie"; }
	std::string getDeathSound() const override { return "mob.zombiedeath"; }
	std::string getHurtSound() const override { return "mob.zombiehurt"; }
	int getDeathLoot() const override { return Item::feather->m_itemID; }
	int getMaxHealth() const override { return 10; }
	float getSoundVolume() const override { return 0.4f; }

	void aiStep() override;
};