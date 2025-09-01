/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

class MapColor
{
public:
	MapColor(int, int);

	static void initMapColors();
	static void teardownMapColors();

public:
	static MapColor* mapColors[16];
	static MapColor
		*air,
		*grass,
		*sand,
		*cloth,
		*red,
		*ice,
		*metal,
		*foliage,
		*snow,
		*clay,
		*dirt,
		*stone,
		*water,
		*wood;

	const int m_id;
	const int m_color;
};
