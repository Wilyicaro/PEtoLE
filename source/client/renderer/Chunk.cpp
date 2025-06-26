/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Chunk.hpp"
#include "world/level/Level.hpp"
#include "world/level/Region.hpp"
#include "TileRenderer.hpp"
#include "TileEntityRenderDispatcher.hpp"
#include <set>
#include <algorithm>

int Chunk::updates;

float Chunk::distanceToSqr(const Entity* pEnt) const
{
	float dX = pEnt->m_pos.x - float(m_pos2.x);
	float dY = pEnt->m_pos.y - float(m_pos2.y);
	float dZ = pEnt->m_pos.z - float(m_pos2.z);
	return dX * dX + dY * dY + dZ * dZ;
}
float Chunk::squishedDistanceToSqr(const Entity* pEnt) const
{
	float dX = pEnt->m_pos.x - float(m_pos2.x);
	float dY = pEnt->m_pos.y - float(m_pos2.y);
	float dZ = pEnt->m_pos.z - float(m_pos2.z);

	dY *= 2;

	return dX * dX + dY * dY + dZ * dZ;
}

void Chunk::reset()
{
	empty[0] = true;
	empty[1] = true;
	m_bVisible = false;
	m_bCompiled = false;
}

int Chunk::getList(int idx)
{
	if (!m_bVisible)
		return -1;

	if (empty[idx])
		return -1;

	return field_8C + idx;
}

RenderChunk* Chunk::getRenderChunk(int idx)
{
	return &m_renderChunks[idx];
}

int Chunk::getAllLists(int* arr, int arr_idx, int idx)
{
	if (!m_bVisible)
		return arr_idx;

	if (empty[idx])
		return arr_idx;

	arr[arr_idx++] = field_8C + idx;

	return arr_idx;
}

void Chunk::cull(Culler* pCuller)
{
	m_bVisible = pCuller->isVisible(m_aabb);
}

void Chunk::renderBB()
{

}

bool Chunk::isEmpty()
{
	if (!m_bCompiled)
		return false;

	if (!empty[0])
		return false;

	if (!empty[1])
		return false;

	return true;
}

void Chunk::setDirty()
{
	m_bDirty = true;
}

void Chunk::setPos(const TilePos& pos)
{
	if (m_pos == pos)
		// No change.
		return;

	m_pos = pos;
	m_pos2 = pos + field_10 / 2;

	m_aabb = AABB(m_pos - 1, m_pos + field_10 + 1);

	setDirty();
}

void Chunk::setClean()
{
	m_bDirty = false;
}

bool Chunk::isDirty()
{
	return m_bDirty;
}

void Chunk::rebuild()
{
	if (!m_bDirty)
		return;

	updates++;

	LevelChunk::touchedSky = false;
	std::set<std::shared_ptr<TileEntity>> oldSet(m_renderableTileEntities.begin(), m_renderableTileEntities.end());
	m_renderableTileEntities.clear();
	empty[0] = true;
	empty[1] = true;

	TilePos min(m_pos), max(m_pos + field_10);

	Region region(m_pLevel, min - 1, max + 1);
	TileRenderer tileRenderer(&region);

	Tesselator& t = Tesselator::instance;

	TilePos tp(min);
	for (int layer = 0; layer < 2; layer++)
	{
		bool bTesselatedAnything = false, bDrewThisLayer = false, bNeedAnotherLayer = false;
		for (tp.y = min.y; tp.y < max.y; tp.y++)
		{
			for (tp.z = min.z; tp.z < max.z; tp.z++)
			{
				for (tp.x = min.x; tp.x < max.x; tp.x++)
				{
					TileID tile = region.getTile(tp);
					if (tile <= 0) continue;

					if (!bTesselatedAnything)
					{
						bTesselatedAnything = true;
						if (tileRenderer.useAmbientOcclusion())
							glShadeModel(GL_SMOOTH);
						t.begin();
						t.offset(float(-m_pos.x), float(-m_pos.y), float(-m_pos.z));
					}

					if (!layer && Tile::isEntityTile[tile]) {
						auto et = region.getTileEntity(tp);
						if (TileEntityRenderDispatcher::getInstance()->hasRenderer(et.get())) 
						{
							m_renderableTileEntities.push_back(et);
						}
					}

					Tile* pTile = Tile::tiles[tile];

					if (layer == pTile->getRenderLayer())
					{
						if (tileRenderer.tesselateInWorld(pTile, tp))
							bDrewThisLayer = true;
					}
					else
					{
						bNeedAnotherLayer = true;
					}
				}
			}
		}

		if (bTesselatedAnything)
		{
			RenderChunk rchk = t.end(field_90[layer]);
			RenderChunk* pRChk = &m_renderChunks[layer];

			*pRChk = rchk;
			pRChk->m_posX  = float(m_pos.x);
			pRChk->m_posY = float(m_pos.y);
			pRChk->m_posZ = float(m_pos.z);

			t.offset(0.0f, 0.0f, 0.0f);

			if (bDrewThisLayer)
				empty[layer] = false;
		}

		if (!bNeedAnotherLayer)
			break;
	}

	std::set<std::shared_ptr<TileEntity>> newSet(m_renderableTileEntities.begin(), m_renderableTileEntities.end());

	std::vector<std::shared_ptr<TileEntity>> toAdd, toRemove;

	std::set_difference(
		newSet.begin(), newSet.end(),
		oldSet.begin(), oldSet.end(),
		std::back_inserter(toAdd)
	);

	std::set_difference(
		oldSet.begin(), oldSet.end(),
		newSet.begin(), newSet.end(),
		std::back_inserter(toRemove)
	);

	for (auto& e : toAdd)
		m_globalRenderableTileEntities.push_back(e);

	for (auto& e : toRemove) {
		auto it = std::find(m_globalRenderableTileEntities.begin(), m_globalRenderableTileEntities.end(), e);
		if (it != m_globalRenderableTileEntities.end())
			m_globalRenderableTileEntities.erase(it);
	}
	m_bSkyLit = LevelChunk::touchedSky;
	m_bCompiled = true;
}

void Chunk::translateToPos()
{
	glTranslatef(float(m_pos.x), float(m_pos.y), float(m_pos.z));
}

Chunk::Chunk(Level* level, std::vector<std::shared_ptr<TileEntity>>& renderableTileEntities, const TilePos& pos, int a, int b, GLuint* bufs) : m_globalRenderableTileEntities(renderableTileEntities)
{
	m_bOcclusionVisible = true;
	field_4E = false;
	m_bCompiled = false;
	m_bDirty = false;

	m_pLevel = level;
	field_10 = TilePos(a, a, a);
	m_pTesselator = &Tesselator::instance;
	field_8C = b;
	m_pos.x = -999;
	field_2C = Vec3(pos).lengthSqr() / 2;
	field_90 = bufs;

	setPos(pos);
}
