/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "PerlinNoise.hpp"
#include "common/Utils.hpp"

PerlinNoise::PerlinNoise(int nOctaves)
{
	init(new Random(), nOctaves);
}

PerlinNoise::PerlinNoise(Random* pRandom, int nOctaves)
{
	if (nOctaves == 10)
	{
		LOG_I("PerlinNoise octaves are 10");
	}

	init(pRandom, nOctaves);
}

void PerlinNoise::init(Random* pRandom, int nOctaves)
{
	m_nOctaves = nOctaves;
	m_pImprovedNoise = new ImprovedNoise * [nOctaves];

	for (int i = 0; i < nOctaves; i++)
	{
		m_pImprovedNoise[i] = new ImprovedNoise(pRandom);
	}
}

PerlinNoise::~PerlinNoise()
{
	for (int i = 0; i < m_nOctaves; i++)
		delete m_pImprovedNoise[i];

	delete[] m_pImprovedNoise;
}

real PerlinNoise::getValue(real x, real y)
{
	if (m_nOctaves <= 0) return 0.0f;

	real result = 0.0f, x1 = 1.0f;

	for (int i = 0; i < m_nOctaves; i++)
	{
		result += m_pImprovedNoise[i]->getValue(x * x1, y * x1) / x1;
		x1 /= 2;
	}

	return result;
}

real PerlinNoise::getValue(real x, real y, real z)
{
	if (m_nOctaves <= 0) return 0.0;

	real result = 0.0f, x1 = 1.0;

	for (int i = 0; i < m_nOctaves; i++)
	{
		result += m_pImprovedNoise[i]->getValue(x * x1, y * x1, z * x1) / x1;
		x1 /= 2;
	}

	return result;
}

const std::vector<real>& PerlinNoise::getRegion(std::vector<real>& pMem, real a3, real a4, real a5, int a6, int a7, int a8, real a9, real a10, real a11)
{
	pMem.assign(a6 * a7 * a8, 0.0);
	
	real x = 1.0;
	for (int i = 0; i < m_nOctaves; i++)
	{
		m_pImprovedNoise[i]->add(pMem, a3, a4, a5, a6, a7, a8, a9 * x, a10 * x, a11 * x, x);
		x /= 2;
	}

	return pMem;
}


const std::vector<real>& PerlinNoise::getBiomeRegion(std::vector<real>& pMem, real a3, real a4, int a5, int a6, real a7, real a8, real a9, real a10)
{
	a7 /= 1.5;
	a8 /= 1.5;
	pMem.assign(a5 * a6, 0.0);

	real x = 1.0, y = 1.0;
	for (int i = 0; i < m_nOctaves; i++)
	{
		m_pImprovedNoise[i]->addBiome(pMem, a3, a4, a5, a6, a7 * y, a8 * y, 0.55 / x);
		x *= a10;
		y *= a9;
	}

	return pMem;
}
