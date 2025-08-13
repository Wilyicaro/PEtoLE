#pragma once

#include "world/entity/Monster.hpp"

class Giant : public Monster
{
public:
	Giant(Level* pLevel);

	float getWalkTargetValue(const TilePos&) const override;
};