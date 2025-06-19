/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "RenderChunk.hpp"

class RenderList
{
public:
	RenderList();
	~RenderList();

	void add(int x);
	void addR(const RenderChunk&);
	void clear();
	void init(float, float, float);
	bool isAt(const RenderChunk& rc);
	void render();
	void renderChunks();

public:
	float m_offX;
	float m_offY;
	float m_offZ;
	int* field_C;
	RenderChunk* m_renderChunks;
	int m_index;
	bool inited;
	bool rendered;
	int m_remaining;
};

