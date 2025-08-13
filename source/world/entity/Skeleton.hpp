#pragma once

#include "world/entity/Monster.hpp"
#include "world/entity/projectile/Arrow.hpp"

class Skeleton : public Monster
{
private:
	static std::shared_ptr<ItemInstance> bow;

public:
	Skeleton(Level* pLevel);

	std::string getAmbientSound() override { return "mob.skeleton"; }
	std::string getDeathSound() const override { return "mob.skeletonhurt"; }
	std::string getHurtSound() const override { return "mob.skeletonhurt"; }
	
	void aiStep() override;
	
	void checkHurtTarget(Entity* ent, float f) override;
	
	int getDeathLoot() const override { return Item::arrow->m_itemID; }
	
	void dropDeathLoot() override;
	
	std::shared_ptr<ItemInstance> getCarriedItem() override;
};