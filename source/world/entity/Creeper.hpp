#pragma once

#include "world/entity/Monster.hpp"

#define DATA_SWELL_DIR (16)
#define DATA_POWERED (17)
#define MAX_SWELL (30)

class Creeper : public Monster
{
public:
	Creeper(Level* pLevel);

	void tick() override;
	std::string getHurtSound() const override { return "mob.creeper"; }
	std::string getDeathSound() const override { return "mob.creeperdeath"; }
	float getSwelling(float f) const;
	bool isPowered() const;
	int getSwellDir() const { return m_entityData.get<int8_t>(DATA_SWELL_DIR); }
	void setSwellDir(int value) { m_entityData.set<int8_t>(DATA_SWELL_DIR, value); }
	int getDeathLoot() const override { return ITEM_SULPHUR; }

	virtual void die(Entity* pCulprit) override;
	virtual void checkHurtTarget(Entity*, float) override;
	virtual void thunderHit(LightningBolt*) override;

	void addAdditionalSaveData(CompoundIO tag) override;
	void readAdditionalSaveData(CompoundIO tag) override;

	void defineSynchedData() override;

public:
	int m_swell;
	int m_oldSwell;
};