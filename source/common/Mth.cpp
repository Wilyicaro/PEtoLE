/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <cmath>
#include <stdint.h>
#include <stdlib.h>

#include "Mth.hpp"

#define C_SIN_TABLE_MULTIPLIER (10430.0f) // (3320.0f * 3.14156f)

#define ANG_TO_SIN_TABLE_INDEX(ang) ((int)  ((ang) * C_SIN_TABLE_MULTIPLIER))
#define SIN_TABLE_INDEX_TO_ANG(ang) ((float)((ang) / C_SIN_TABLE_MULTIPLIER))

float g_SinTable[65536];

Random Mth::g_Random;

void Mth::initMth()
{
	for (int i = 0; i < 65536; i++)
	{
		g_SinTable[i] = sinf(SIN_TABLE_INDEX_TO_ANG(i)); // value is 10430
	}
}

int Mth::intFloorDiv(int a2, int a3)
{
	if (a2 < 0)
		return ~(~a2 / a3);
	
	return a2 / a3;
}

float Mth::invSqrt(float number)
{
	// It looks familiar. With IDA I get a convoluted mess. I'm going to assume
	// they just stole it from Quake.

	int32_t i;
	float x2, y;
	const float threehalfs = 1.5F;
	
	x2 = number * 0.5F;
	y  = number;
	i  = * ( int32_t * ) &y;                   // evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );              // what the fuck?
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );  // 1st iteration
  // y  = y * ( threehalfs - ( x2 * y * y ) );  // 2nd iteration, this can be removed

	return y;
}

double Mth::sqr(double d) {
	return d * d;
}

float Mth::sin(float a2)
{
	int angle = ANG_TO_SIN_TABLE_INDEX(a2) & 0xFFFF;

	return g_SinTable[angle];
}

float Mth::cos(float a2)
{
	int angle = (ANG_TO_SIN_TABLE_INDEX(a2) + 16384) & 0xFFFF;

	return g_SinTable[angle];
}

double Mth::clamp(double a, double min, double max)
{
	if (a > max)
		return max;
	if (a > min)
		return a;
	else
		return min;
	return max;
}

// ported from 0.6.1
unsigned Mth::fastRandom()
{
	int x0;
	static int  x1, x2, x3, x4;

	x0 = x1;
	x1 = x2;
	x2 = x3;
	x3 = x4;
	return(x4 = x4 ^ (unsigned(x4) >> 19) ^ x0 ^ (x0 << 11) ^ ((x0 ^ unsigned(x0 << 11)) >> 8));
}

float Mth::clamp(float a, float min, float max)
{
	if (a > max)
		return max;
	if (a > min)
		return a;
	else
		return min;
	return max;
}

int Mth::clamp(int a, int min, int max)
{
	if (a > max)
		return max;
	if (a > min)
		return a;
	else
		return min;
	return max;
}

int Mth::floor(float f)
{
	int result = int(f);

	if (f < result)
		result--;

	return result;
}

int Mth::floor(double d)
{
	int result = int(d);

	if (d < result)
		result--;

	return result;
}

int Mth::round(double d)
{
	return floor(d + 0.5);
}

float Mth::atan(float f)
{
	return atanf(f);
}

float Mth::atan2(float y, float x)
{
	return atan2f(y, x);
}

float Mth::min(float a, float b)
{
	return a < b ? a : b;
}

double Mth::min(double a, double b)
{
	return a < b ? a : b;
}

int Mth::min(int a, int b)
{
	return a < b ? a : b;
}

float Mth::max(float a, float b)
{
	return a > b ? a : b;
}

double Mth::max(double a, double b)
{
	return a > b ? a : b;
}

int Mth::max(int a, int b)
{
	return a > b ? a : b;
}

/*float Mth::abs(float f)
{
	if (f < 0.0f)
		f = -f;
	return f;
}*/

int Mth::abs(int d)
{
	if (d < 0)
		d = -d;
	return d;
}

float Mth::absMax(float a2, float a3)
{
	if (a2 < 0.0f)
		a2 = -a2;
	if (a3 < 0.0f)
		a3 = -a3;
	if (a2 <= a3)
		a2 = a3;
	return a2;
}

float Mth::absMaxSigned(float a2, float a3)
{
	if (abs(a2) <= abs(a2))
		a2 = a3;
	return a2;
}

int Mth::random(int max)
{
	return int(g_Random.nextInt(max));
}

float Mth::random()
{
	return g_Random.nextFloat();
}


int Mth::HSBtoRGB(float hue, float saturation, float brightness)
{
	int r = 0, g = 0, b = 0;
	if (saturation == 0) {
		r = g = b = (int)(brightness * 255.0f + 0.5f);
	}
	else {
		float h = (hue - floor(hue)) * 6.0f;
		float f = h - floor(h);
		float p = brightness * (1.0f - saturation);
		float q = brightness * (1.0f - saturation * f);
		float t = brightness * (1.0f - (saturation * (1.0f - f)));
		switch ((int)h) {
		case 0:
			r = (int)(brightness * 255.0f + 0.5f);
			g = (int)(t * 255.0f + 0.5f);
			b = (int)(p * 255.0f + 0.5f);
			break;
		case 1:
			r = (int)(q * 255.0f + 0.5f);
			g = (int)(brightness * 255.0f + 0.5f);
			b = (int)(p * 255.0f + 0.5f);
			break;
		case 2:
			r = (int)(p * 255.0f + 0.5f);
			g = (int)(brightness * 255.0f + 0.5f);
			b = (int)(t * 255.0f + 0.5f);
			break;
		case 3:
			r = (int)(p * 255.0f + 0.5f);
			g = (int)(q * 255.0f + 0.5f);
			b = (int)(brightness * 255.0f + 0.5f);
			break;
		case 4:
			r = (int)(t * 255.0f + 0.5f);
			g = (int)(p * 255.0f + 0.5f);
			b = (int)(brightness * 255.0f + 0.5f);
			break;
		case 5:
			r = (int)(brightness * 255.0f + 0.5f);
			g = (int)(p * 255.0f + 0.5f);
			b = (int)(q * 255.0f + 0.5f);
			break;
		}
	}
	return 0xff000000 | (r << 16) | (g << 8) | (b << 0);
}

float Mth::normalDegrees(float rot)
{
	return Mth::abs(rot) > 360.0f ? fmod(rot, 360.0f) : rot;
}