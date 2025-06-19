/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "ImprovedNoise.hpp"

class PerlinNoise : public Synth
{
public:
	PerlinNoise(int nOctaves);
	PerlinNoise(Random*, int nOctaves);
	virtual ~PerlinNoise();
	void init(Random* pRandom, int nOctaves);

	real getValue(real, real) override;
	real getValue(real, real, real);

	real* getRegion(real* a2, int a3, int a4, int a5, int a6, real a7, real a8, real a9)
	{
		return getRegion(a2, real(a3), 10.0, real(a4), a5, 1, a6, a7, 1.0, a8);
	}

	real* getRegion(real*, real, real , real, int, int, int, real, real, real);

	real* getBiomeRegion(real* pMem, real a3, real a4, int a5, int a6, real a7, real a8, real a9)
	{
		return getBiomeRegion(pMem, a3, a4, a5, a6, a7, a8, a9, 0.5);
	}

	real* getBiomeRegion(real* pMem, real a3, real a4, int a5, int a6, real a7, real a8, real a9, real a10);

private:
	ImprovedNoise** m_pImprovedNoise;
	int m_nOctaves;
};

