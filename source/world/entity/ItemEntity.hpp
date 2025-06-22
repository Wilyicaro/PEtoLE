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
private:
	void _init(const ItemInstance* itemInstance = nullptr);
	void _init(const ItemInstance* itemInstance, const Vec3& pos);
public:
	ItemEntity(Level* level) : Entity(level) { _init(); }
	ItemEntity(Level* level, const Vec3& pos, const ItemInstance* itemInstance) : Entity(level) { _init(itemInstance, pos); }

	void burn(int damage) override;
	bool hurt(Entity* pCulprit, int damage) override;
	bool wasInWater() override;
	void playerTouch(Player*) override;
	void tick() override;

	void addAdditionalSaveData(std::shared_ptr<CompoundTag>) override;

	void readAdditionalSaveData(std::shared_ptr<CompoundTag>) override;

public:

	std::shared_ptr<ItemInstance> m_itemInstance;


	int m_age;
	int m_throwTime;
	int m_tickCount;
	int m_health;
	float m_bobOffs;
};

