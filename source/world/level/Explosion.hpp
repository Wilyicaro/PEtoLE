/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <unordered_set>
#include "Level.hpp"
#include "world/entity/Entity.hpp"

class Explosion
{
public:
	Explosion(Level*, Entity*, const Vec3& pos, float power);
	void addParticles();
	void explode();

	// @NOTE: This is inlined
	inline void setFiery(bool b)
	{
		m_bIsFiery = b;
	}

public:
	Vec3 m_pos;
	float m_radius;
	std::unordered_set<TilePos> m_toBlow;
	int field_20;

	bool m_bIsFiery;
	Entity* m_pEntity;
	Random m_random;
	Level* m_pLevel;

};

