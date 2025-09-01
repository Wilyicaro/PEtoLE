#pragma once

#include "world/entity/Zombie.hpp"

class PigZombie : public Zombie
{
public:
	PigZombie(Level* pLevel);

	void tick() override;
	bool canSpawn() override;
	void addAdditionalSaveData(CompoundIO tag) override;
	void readAdditionalSaveData(CompoundIO tag) override;
	std::shared_ptr<Entity> findAttackTarget() override;
	bool hurt(Entity* ent, int dmg) override;
	std::shared_ptr<ItemInstance> getCarriedItem() override;

	std::string getAmbientSound() override { return "mob.zombiepig.zpig"; }
	std::string getDeathSound() const override { return "mob.zombiepig.zpigdeath"; }
	std::string getHurtSound() const override { return "mob.zombiepig.zpighurt"; }
	int getDeathLoot() const override { return Item::cookedPorkchop->m_itemID; }

private:
	void alert(Entity*);

private:
	int m_angerTime = 0;
	int m_playAngrySoundIn = 0;

	static std::shared_ptr<ItemInstance> sword;
};