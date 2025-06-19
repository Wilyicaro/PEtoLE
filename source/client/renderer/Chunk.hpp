/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "common/Utils.hpp"
#include "FrustumCuller.hpp"
#include "RenderList.hpp"
#include "Tesselator.hpp"

class Level;
class Entity;
class TileEntity;

class Chunk
{
public:
	Chunk(Level*, std::vector<std::shared_ptr<TileEntity>>& renderableTileEntities, const TilePos& pos, int, int, GLuint*);
	~Chunk();

	float distanceToSqr(const Entity*) const;
	float squishedDistanceToSqr(const Entity*) const;
	void reset();
	int getList(int idx);
	RenderChunk* getRenderChunk(int idx);
	int getAllLists(int* arr, int arr_idx, int idx);
	void cull(Culler* pCuller);
	void renderBB();
	bool isEmpty();
	void setDirty();
	void setPos(const TilePos& pos);
	void setClean();
	bool isDirty();
	void rebuild();
	void translateToPos();

public:
	static int updates;

public:
	Level* m_pLevel;
	std::vector<std::shared_ptr<TileEntity>>& m_globalRenderableTileEntities;
	std::vector<std::shared_ptr<TileEntity>> m_renderableTileEntities;
	TilePos m_pos;
	TilePos field_10;
	bool empty[2];
	TilePos m_pos2;
	float field_2C;
	AABB m_aabb;
	int field_48;
	bool m_bVisible;
	bool m_bOcclusionVisible;
	bool field_4E;
	int field_50;
	bool m_bSkyLit;
	RenderChunk m_renderChunks[2];
	Tesselator* m_pTesselator;
	int field_8C;
	GLuint* field_90;
	bool m_bCompiled;
	bool m_bDirty;
};

