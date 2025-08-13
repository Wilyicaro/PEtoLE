/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Vec2.hpp"

const Vec2 Vec2::ZERO = Vec2(0, 0);

void Vec2::_init(float y, float x)
{
    this->y = y;
	this->x = x;
}

Vec2::Vec2()
{
    _init(0, 0);
}

Vec2::Vec2(float y, float x)
{
	_init(y, x);
}

Vec2 Vec2::normalize() const
{
	float dist = Mth::sqrt(y * y + x * x);
	if (dist < 0.0001f)
		return ZERO;

	return Vec2(y / dist, x / dist);
}
