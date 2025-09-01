#pragma once

#include "world/entity/Mob.hpp"

class Slime : public Mob
{
public:
	Slime(Level* pLevel);

	void setSize(int);

	void addAdditionalSaveData(CompoundIO tag) override;
	void readAdditionalSaveData(CompoundIO tag) override;
	void tick() override;
	void updateAi() override;
	void remove() override;
	void playerTouch(Player*) override;

	std::string getHurtSound() const override { return "mob.slime"; }
	std::string getDeathSound() const override { return "mob.slime"; }

	float getSoundVolume() const override { return 0.6f; }
	int getDeathLoot() const override { return m_size == 1 ? Item::slimeBall->m_itemID : 0; }
	bool canSpawn() override;

public:
	float m_oSquish;
	float m_squish;
	int m_size;

private:
	int m_jumpDelay;
};