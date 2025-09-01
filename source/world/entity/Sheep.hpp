#pragma once

#include "Animal.hpp"

class Sheep : public Animal
{
public:
	static const float COLOR[][3];

public:
	Sheep(Level* pLevel);

	std::string getAmbientSound() override { return "mob.sheep"; };
	std::string getDeathSound() const override { return "mob.sheep"; }
	std::string getHurtSound() const override { return "mob.sheep"; }
#ifdef ENH_b1_7
	virtual void dropDeathLoot() override;
	virtual bool interact(Player*) override;
	virtual int getDeathLoot() const override;
#else
	virtual bool hurt(Entity*, int) override;
#endif
	void addAdditionalSaveData(CompoundIO tag) override;
	void readAdditionalSaveData(CompoundIO tag) override;
	void defineSynchedData() override;

	Entity* getBreedOffspring(Animal* pOther) { return new Sheep(m_pLevel); }

    int getColor() const;
    void setColor(int);
    bool isSheared() const;
    void setSheared(bool);

	static int getSheepColor(Random&);
};
