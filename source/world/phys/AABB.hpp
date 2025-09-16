/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Vec3T.hpp"
#include "HitResult.hpp"

class AABB
{
public:
	Vec3 min, max;

	AABB();
	AABB(Vec3 min, Vec3 max);
	AABB(real minX, real minY, real minZ, real maxX, real maxY, real maxZ);

public:
	HitResult clip(const Vec3&, const Vec3&);
	real clipXCollide(const AABB& bud, real f) const;
	real clipYCollide(const AABB& bud, real f) const;
	real clipZCollide(const AABB& bud, real f) const;

	real clipCollide(const AABB& bud, real f, Axis axis) const {
		switch (axis) {
		case Axis::X: return clipXCollide(bud, f);
		case Axis::Y: return clipYCollide(bud, f);
		case Axis::Z: return clipZCollide(bud, f);
		}
	}

	bool containsX(Vec3* pVec);
	bool containsY(Vec3* pVec);
	bool containsZ(Vec3* pVec);

	bool intersect(const AABB& other) const;

	real getSize() const;

	// @NOTE: Names for `move`, `grow` and `expand` were taken from really early minecraft (rd-132211 to be exact).
	// @NOTE: Why not use the leaked verson of b1.2? lol
	void move(const Vec3& vec);
	void move(real x, real y, real z);
	AABB copyMove(const Vec3& vec);
	AABB copyMove(real x, real y, real z);
	// same thing
	void grow(const Vec3& vec);
	void grow(real x, real y, real z);
	// same thing
	void grow(real x);
	void expand(real x, real y, real z);
	void expand(const Vec3& vec);
	bool contains(const Vec3& v) const;
	float byIndex(int index) {
		switch (index) {
			case 0: return min.x;
			case 1: return min.y;
			case 2: return min.z;
			case 3: return max.x;
			case 4: return max.y;
			case 5: return max.z;
		}
		return 0;
	}
};

