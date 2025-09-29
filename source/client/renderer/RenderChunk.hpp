/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "thirdparty/GL/GL.hpp"
#include <world/phys/Vec3T.hpp>

class RenderChunk
{
	static int runningId;

public:
	GLuint m_glID;
	int field_4;
	int m_id;
	Vec3 m_pos;

public:
	RenderChunk()
	{
		m_glID = -1;
		field_4 = 0;
		m_pos = Vec3::ZERO;

		m_id = ++runningId;
	}
	RenderChunk(GLuint a1, int a2)
	{
		m_pos = Vec3::ZERO;

		m_id = ++runningId;
		m_glID = a1;
		field_4 = a2;
	}
};

