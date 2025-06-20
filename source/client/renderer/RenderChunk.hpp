/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "thirdparty/GL/GL.hpp"

class RenderChunk
{
	static int runningId;

public:
	GLuint field_0;
	int field_4;
	int m_id;
	float m_posX;
	float m_posY;
	float m_posZ;

public:
	RenderChunk()
	{
		field_0 = -1;
		field_4 = 0;
		m_posX = 0.0f;
		m_posY = 0.0f;
		m_posZ = 0.0f;

		m_id = ++runningId;
	}
	RenderChunk(GLuint a1, int a2)
	{
		m_posX = 0.0f;
		m_posY = 0.0f;
		m_posZ = 0.0f;

		m_id = ++runningId;
		field_0 = a1;
		field_4 = a2;
	}
};

