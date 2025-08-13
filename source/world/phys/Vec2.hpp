/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "common/Mth.hpp"
// Needed for when we're missing nullptr in multiple files
#include "compat/LegacyCPPCompatibility.hpp"

class Vec2
{
public:
	static const Vec2 ZERO;

public:
	float x, y;
    
private:
    void _init(float y, float x);

public:
	// this constructor is nice to have, but it's probably inlined
	Vec2();
	Vec2(float y, float x);
    
    Vec2 normalize() const;

	// these are also likely inlined, but I'll declare them in the header
	Vec2 operator+(const Vec2& b) const
	{
		return Vec2(y + b.y, x + b.x);
	}

	Vec2 operator-(const Vec2& b) const
	{
		return Vec2(y - b.y, x - b.x);
	}

	void operator+=(const Vec2& b)
	{
		y += b.y;
		x += b.x;
	}

	void operator-=(const Vec2& b)
	{
		(*this) += -b;
	}

	void operator*=(float f)
	{
		y *= f;
		x *= f;
	}

	Vec2 operator-() const
	{
		return Vec2(-y, -x);
	}

	Vec2 operator*(float f) const
	{
		return Vec2(f * y, f * x);
	}

	Vec2 operator/(float f) const
	{
		return Vec2(y / f, x / f);
	}

	bool operator==(const Vec2& b) const
	{
		return y == b.y &&
			   x == b.x;
	}

	Vec2 translate(float tx, float ty) const
	{
		return Vec2(y + tx, x + ty);
	}

	float lengthSqr() const
	{
		return y * y + x * x;
	}

	float length() const
	{
		return Mth::sqrt(lengthSqr());
	}

	Vec2 scale(float scale) const
	{
		return Vec2(y * scale, x * scale);
	}
};

