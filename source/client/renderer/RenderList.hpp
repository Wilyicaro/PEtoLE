/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "RenderChunk.hpp"
#include <world/phys/Vec3T.hpp>

class RenderList
{
public:
	RenderList();
	~RenderList();

	void add(int x);
	void addR(RenderChunk&);
	void clear();
	void init(const Vec3& off);
	bool isAt(const RenderChunk& rc) const;
	void render();
	void renderChunks();

public:
	Vec3 m_off;
	int* field_C;
	RenderChunk** m_renderChunks;
	int m_index;
	bool inited;
	bool rendered;
	int m_remaining;
};

