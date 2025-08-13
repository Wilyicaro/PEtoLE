#pragma once

#include "world/entity/Mob.hpp"
#include "world/level/Level.hpp"

class FlyingMob : public Mob
{
public:
	FlyingMob(Level* pLevel);

	void causeFallDamage(float) override;

	void travel(const Vec2&) override;

	bool onLadder() const override;
};