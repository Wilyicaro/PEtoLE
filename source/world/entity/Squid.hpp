#pragma once

#include "WaterAnimal.hpp"

class Squid : public WaterAnimal
{
public:
	Squid(Level* pLevel);

	void dropDeathLoot() override;
	std::string getAmbientSound() override { return ""; };
	std::string getDeathSound() const override { return ""; }
	std::string getHurtSound() const override { return ""; }
	int getDeathLoot() const override { return Item::dyePowder->m_itemID; }
	float getSoundVolume() const override { return 0.4f; }
	bool wasInWater() override;
	void aiStep() override;
	void travel(const Vec2&) override;
	void updateAi() override;

public:
	float m_xBodyRot = 0.0F;
	float m_xBodyRotO = 0.0F;
	float m_zBodyRot = 0.0F;
	float m_zBodyRotO = 0.0F;
	float m_tentacleMovement = 0.0F;
	float m_oldTentacleMovement = 0.0F;
	float m_tentacleAngle = 0.0F;
	float m_oldTentacleAngle = 0.0F;

private:
	float m_speed = 0.0F;
	float m_tentacleSpeed = 0.0F;
	float m_rotateSpeed = 0.0F;
	Vec3f m_tPos;
};
