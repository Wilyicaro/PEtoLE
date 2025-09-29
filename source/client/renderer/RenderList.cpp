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
	m_off = Vec3::ZERO;
	m_index = 0;
	inited = false;
	rendered = false;
	m_remaining = 0;

	field_C = new int[C_MAX_RENDERS];
	m_renderChunks = new RenderChunk*[C_MAX_RENDERS];
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
		init(m_off);
		m_remaining = 0;
		rendered = false;
	}
#endif

	field_C[m_index] = x;

	if (m_index == C_MAX_RENDERS)
		render();

}

void RenderList::addR(RenderChunk& rc)
{
	// @BUG: If too many chunks are rendered, this has the potential to overflow.
#ifndef ORIGINAL_CODE
	if (m_index == C_MAX_RENDERS)
	{
		render();
		init(m_off);
		m_remaining = 0;
		rendered = false;
	}
#endif
	

	m_renderChunks[m_index] = &rc;

	m_index++;

	if (m_index == C_MAX_RENDERS)
		render();
}

void RenderList::clear()
{
	inited = false;
	rendered = false;
}

void RenderList::init(const Vec3& off)
{
	m_off = off;
	m_index = 0;
	inited = true;
}

inline static bool sameSlice(int x0, int x1)
{
	return (x0 - x0 & 1023) == (x1 - x1 & 1023);
}


bool RenderList::isAt(const RenderChunk& rc) const
{
	if (!inited) return false;
	auto actual = m_renderChunks[0];
	return sameSlice(actual->m_pos.x, rc.m_pos.x) && sameSlice(actual->m_pos.z, rc.m_pos.z);
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
		renderChunks();
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
			RenderChunk& chk = *m_renderChunks[i];
			glPushMatrix();

			glTranslatef(chk.m_pos.x - m_off.x, chk.m_pos.y - m_off.y, chk.m_pos.z - m_off.z);
			xglBindBuffer(GL_ARRAY_BUFFER, chk.m_glID);
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
