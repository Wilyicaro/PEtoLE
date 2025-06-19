/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "common/Random.hpp"
#include "common/Mth.hpp"
#include "Synth.hpp"

// note: appears to maybe inherit from a class called Synth?
// the only purpose that it serves I guess is to provide
// a virtual getValue
class ImprovedNoise : public Synth
{
public:
	real getValue(real, real) override;

	ImprovedNoise();
	ImprovedNoise(Random* pRandom);

	void init(Random* pRandom);
	real getValue(real, real, real);
	real noise(real, real, real);
	real grad(int, real, real, real) const;
	real grad2(int, real, real) const;
	real lerp(real prog, real a, real b) const;
	real fade(real x) const; // inlined in the code
	void add(real* a2, real a3, real a4, real a5, int a6, int a7, int a8, real a9, real a10, real a11, real a12);

	void addBiome(real* var1, real var2, real var4, int var6, int var7, real var8, real var10, real var12);

public:
	real m_offsetX;
	real m_offsetY;
	real m_offsetZ;
	int m_permutation[512];
	static real field_4294_f;
	static real field_4293_g;
	static constexpr int field_4296_d[12][3] = { {1, 1, 0}, {-1, 1, 0}, {1, -1, 0}, {-1, -1, 0}, {1, 0, 1}, {-1, 0, 1}, {1, 0, -1}, {-1, 0, -1}, {0, 1, 1}, {0, -1, 1}, {0, 1, -1}, {0, -1, -1} };
	static real func_4156_a(const int* var0, real var1, real var3) {
		return (real)var0[0] * var1 + (real)var0[1] * var3;
	}
};

