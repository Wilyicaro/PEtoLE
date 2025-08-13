/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include "Mob.hpp"
#include "world/level/path/Path.hpp"

class PathfinderMob : public Mob
{
public:
	PathfinderMob(Level* pLevel);

	virtual Entity* getAttackTarget();
	virtual void setAttackTarget(std::shared_ptr<Entity>);
	virtual std::shared_ptr<Entity> findAttackTarget();
	virtual void checkHurtTarget(Entity*, float);
	virtual void checkCantSeeTarget(Entity*, float);
	virtual float getWalkTargetValue(const TilePos& pos) const;
	virtual bool shouldHoldGround() const;
	virtual void findRandomStrollLocation();

	float getWalkingSpeedModifier() const override;
	bool canSpawn() override;
	void updateAi() override;

	void setPath(Path& path);
	bool isPathFinding();

protected:
	friend class Animal;
	std::shared_ptr<Entity> m_pAttackTarget;
	bool m_bHoldGround;
	int m_ticksRunning;
	Path m_path;
};
