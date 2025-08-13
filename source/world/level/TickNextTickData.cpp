/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TickNextTickData.hpp"

int TickNextTickData::last;

TickNextTickData::TickNextTickData(const TilePos& tilePos, int d)
{
	m_ID = ++last;
	m_pos = tilePos;
	m_tileId = d;

#ifndef ORIGINAL_CODE
	m_delay = 0;
#endif
}

int TickNextTickData::hashCode() const
{
	return m_tileId + ((m_pos.y + ((m_pos.z + (m_pos.x << 10)) << 7)) << 8);
}

bool TickNextTickData::operator<(const TickNextTickData& other) const
{
	if (m_delay < other.m_delay)
		return true;
	if (m_delay > other.m_delay)
		return false;

	return m_ID < other.m_ID;
}

bool TickNextTickData::operator==(const TickNextTickData& other) const
{
	return
		m_pos  == other.m_pos  &&
		m_tileId == other.m_tileId;
}

void TickNextTickData::setDelay(int32_t l)
{
	m_delay = l;
}
