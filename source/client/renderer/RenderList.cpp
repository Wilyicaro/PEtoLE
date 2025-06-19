/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "RenderList.hpp"
#include "common/Utils.hpp"
#include "Tesselator.hpp"

#include <cstddef>

constexpr int C_MAX_RENDERS = 3072;

RenderList::RenderList()
{
	m_offX = 0.0f;
	m_offY = 0.0f;
	m_offZ = 0.0f;
	m_index = 0;
	inited = false;
	rendered = false;
	m_remaining = 0;

	field_C = new int[C_MAX_RENDERS];
	m_renderChunks = new RenderChunk[C_MAX_RENDERS];
}

RenderList::~RenderList()
{
	if (field_C)
		delete[] field_C;

	if (m_renderChunks)
		delete[] m_renderChunks;
}

void RenderList::add(int x)
{
	// @BUG: If too many chunks are rendered, this has the potential to overflow.
#ifndef ORIGINAL_CODE
	if (m_index == C_MAX_RENDERS)
	{
		render();
		init(m_offX, m_offY, m_offZ);
		m_remaining = 0;
		rendered = false;
	}
#endif

	field_C[m_index] = x;

	if (m_index == C_MAX_RENDERS)
		render();

}

void RenderList::addR(const RenderChunk& rc)
{
	// @BUG: If too many chunks are rendered, this has the potential to overflow.
#ifndef ORIGINAL_CODE
	if (m_index == C_MAX_RENDERS)
	{
		render();
		init(m_offX, m_offY, m_offZ);
		m_remaining = 0;
		rendered = false;
	}
#endif
	

	m_renderChunks[m_index] = rc;

	m_index++;

	if (m_index == C_MAX_RENDERS)
		render();
}

void RenderList::clear()
{
	inited = false;
	rendered = false;
}

void RenderList::init(float x, float y, float z)
{
	m_offX = x;
	m_offY = y;
	m_offZ = z;
	m_index = 0;
	inited = true;
}

inline static bool sameSlice(int x0, int x1)
{
	return (x0 - x0 & 1023) == (x1 - x1 & 1023);
}


bool RenderList::isAt(const RenderChunk& rc)
{
	if (!inited) return false;
	auto& actual = m_renderChunks[0];
	return sameSlice(actual.m_posX, rc.m_posX) && sameSlice(actual.m_posZ, rc.m_posZ);
}

void RenderList::render()
{
	if (!inited) return;

	if (!rendered)
	{
		rendered = true;
		m_remaining = m_index;
		m_index = 0;
	}

	if (m_index < m_remaining)
	{
		glPushMatrix();
		glTranslatef(-m_offX, -m_offY, -m_offZ);
		renderChunks();
		glPopMatrix();
	}
}

void RenderList::renderChunks()
{
	xglEnableClientState(GL_VERTEX_ARRAY);
	xglEnableClientState(GL_COLOR_ARRAY);
	xglEnableClientState(GL_TEXTURE_COORD_ARRAY);

	if (m_remaining > 0)
	{
		for (int i = 0; i < m_remaining; i++)
		{
			RenderChunk& chk = m_renderChunks[i];
			glPushMatrix();

			glTranslatef(chk.m_posX, chk.m_posY, chk.m_posZ);
			xglBindBuffer(GL_ARRAY_BUFFER, chk.field_0);
			xglVertexPointer  (3, GL_FLOAT,         sizeof(Tesselator::Vertex), (void*)offsetof(Tesselator::Vertex, m_x));
			xglTexCoordPointer(2, GL_FLOAT,         sizeof(Tesselator::Vertex), (void*)offsetof(Tesselator::Vertex, m_u));
			xglColorPointer   (4, GL_UNSIGNED_BYTE, sizeof(Tesselator::Vertex), (void*)offsetof(Tesselator::Vertex, m_color));
			xglDrawArrays(GL_TRIANGLES, 0, chk.field_4);

			glPopMatrix();
		}
	}

	xglDisableClientState(GL_VERTEX_ARRAY);
	xglDisableClientState(GL_COLOR_ARRAY);
	xglDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
