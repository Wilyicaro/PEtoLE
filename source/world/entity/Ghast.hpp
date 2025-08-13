#pragma once

#include "world/entity/FlyingMob.hpp"

class Ghast : public FlyingMob
{
public:
	Ghast(Level* pLevel);

	void updateAi() override;

	std::string getAmbientSound() override { return "mob.ghast.moan"; }
	std::string getHurtSound() const override { return "mob.ghast.scream"; }
	std::string getDeathSound() const override { return "mob.ghast.death"; }

	float getSoundVolume() const override { return 10.0f; }
	int getMaxSpawnClusterSize() const override { return 1; }
	int getDeathLoot() const override { return Item::sulphur->m_itemID; }
	bool canSpawn() override;

private:
	bool canReach(const Vec3&, real);

public:
	int m_floatDuration;
	Vec3 m_targetPos;
	int m_oCharge;
	int m_charge;

private:
	std::shared_ptr<Entity> m_target;
	int m_retargetTime;
};