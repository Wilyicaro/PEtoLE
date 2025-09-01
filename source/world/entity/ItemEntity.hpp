/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Entity.hpp"

class ItemEntity : public Entity
{
public:
	ItemEntity(Level* level);
	ItemEntity(Level* level, const Vec3& pos, std::shared_ptr<ItemInstance> itemInstance);

	void burn(int damage) override;
	bool hurt(Entity* pCulprit, int damage) override;
	bool wasInWater() override;
	void playerTouch(Player*) override;
	void tick() override;

	void addAdditionalSaveData(CompoundIO) override;

	void readAdditionalSaveData(CompoundIO) override;

public:

	std::shared_ptr<ItemInstance> m_itemInstance;


	int m_age;
	int m_throwTime;
	int m_tickCount;
	int m_health;
	float m_bobOffs;
};

