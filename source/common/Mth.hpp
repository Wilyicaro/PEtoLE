/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <cmath>

#include "Random.hpp"

class Mth
{
	static Random g_Random;

public:
	static float Max(float, float);
	static double Max(double, double);
	static int Max(int, int);
	static float Min(float, float);
	static double Min(double, double);
	static int Min(int, int);
	static inline float abs(float f) { return fabs(f); }
	static inline double abs(double d) { return std::abs(d); }
	static int abs(int);
	static float absMax(float, float);
	static float absMaxSigned(float, float);
	static float atan(float);
	static float atan2(float y, float x);
	static float cos(float);
	static float clamp(float, float, float);
	static int floor(float);
	static int floor(double);
	static int round(double d);
	static void initMth();
	static int intFloorDiv(int, int);
	static float invSqrt(float);
	static double sqr(double d);
	static int random(int);
	static float random(void);
	static float sin(float);
	static unsigned fastRandom();
	static int HSBtoRGB(float hue, float saturation, float brightness);

	static inline float sqrt(float f)
	{
		return sqrtf(f);
	}

	static inline double sqrt(double f)
	{
		return std::sqrt(f);
	}
    
	static inline constexpr float Lerp(float a, float b, float progress)
	{
		return a + progress * (b - a);
	}
};

