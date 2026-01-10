/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <cstring>
#include <vector>
#include <algorithm>
#include "common/Utils.hpp"
#include "common/Random.hpp"
#include "client/renderer/LightLayer.hpp"
#include "world/phys/AABB.hpp"
#include "world/level/levelgen/chunk/ChunkPos.hpp"
#include "world/level/levelgen/chunk/ChunkTilePos.hpp"
#include "world/level/levelgen/chunk/DataLayer.hpp"
#include <common/CompoundTag.hpp>
#include <world/tile/entity/TileEntity.hpp>
#include <world/entity/EntityCategories.hpp>

class Level;
class Entity;

class LevelChunk
{
private:
	void _init();
protected:
	LevelChunk() { _init(); }
public:
	constexpr int MakeHeightMapIndex(const ChunkTilePos& pos)
	{
		return pos.x | (pos.z * 16);
	}

	void save(CompoundIO tag);

	void load(CompoundIO tag);

	LevelChunk(Level*, const ChunkPos& pos);
	LevelChunk(Level*, TileID* pBlockData, const ChunkPos& pos);
	virtual ~LevelChunk();

	void init();

	void lightGap(const TilePos& pos, uint8_t heightMap);
	void lightGaps(const ChunkTilePos& pos);
	void deleteBlockData();

	virtual bool isAt(const ChunkPos& pos);
	virtual int getHeightmap(const ChunkTilePos& pos);
	virtual void recalcHeightmap();
	virtual void recalcHeightmapOnly();
	virtual int getBrightness(const LightLayer& ll, const ChunkTilePos& pos);
	virtual void setBrightness(const LightLayer& ll, const ChunkTilePos& pos, int brightness);
	virtual int getRawBrightness(const ChunkTilePos& pos, int skySubtract);
	virtual void addEntity(std::shared_ptr<Entity>);
	virtual void updateEntity(std::shared_ptr<Entity> pEnt);
	virtual void removeEntity(std::shared_ptr<Entity>);
	virtual void removeEntity(std::shared_ptr<Entity>, int vec);
	virtual bool isSkyLit(const ChunkTilePos& pos);
	virtual void lightLava();
	virtual void recalcBlockLights();
	virtual void skyBrightnessChanged();
	virtual void load();
	virtual void unload();
	virtual bool shouldSave(bool b = false);
	virtual void markUnsaved();
	virtual int  countEntities();
	const AABB& getEntityAABB(Entity*);
	template<typename Predicate>
	void getEntities(Predicate predicate, const AABB& aabb, std::vector<std::shared_ptr<Entity>>& out)
	{
		int lowerBound = Mth::floor((aabb.min.y - 2.0) / 16.0);
		int upperBound = Mth::floor((aabb.max.y + 2.0) / 16.0);

		if (lowerBound < 0) lowerBound = 0;
		if (upperBound > 7) upperBound = 7;

		for (int b = lowerBound; b <= upperBound; b++)
		{
			for (std::vector<std::shared_ptr<Entity>>::iterator it = m_entities[b].begin(); it != m_entities[b].end(); it++)
			{
				std::shared_ptr<Entity> ent = *it;
				if (!predicate(ent.get())) continue;

				if (!aabb.intersect(getEntityAABB(ent.get()))) continue;

				out.push_back(ent);
			}
		}
	}
	virtual TileID getTile(const ChunkTilePos& pos);
	virtual bool setTile(const ChunkTilePos& pos, TileID tile);
	virtual bool setTileAndData(const ChunkTilePos& pos, TileID tile, int data);
	virtual int  getData(const ChunkTilePos& pos);
	virtual void setData(const ChunkTilePos& pos, int data);
	virtual void setBlocks(uint8_t* pData, int y);
	virtual int  getBlocksAndData(uint8_t* pData, int, int, int, int, int, int, int);
	virtual int  setBlocksAndData(uint8_t* pData, int, int, int, int, int, int, int);
	virtual std::shared_ptr<TileEntity> getTileEntity(const ChunkTilePos& pos);
	virtual void addTileEntity(std::shared_ptr<TileEntity> tileEntity);
	virtual void setTileEntity(const ChunkTilePos& pos, std::shared_ptr<TileEntity> tileEntity);
	virtual void removeTileEntity(const ChunkTilePos& pos);
	virtual Random getRandom(int64_t l);
	virtual void recalcHeight(const ChunkTilePos& pos);
	virtual bool isEmpty();
	//...

public:
	static bool touchedSky;

public:
	int field_4;
	bool m_bLoaded;
	Level* m_pLevel;
	DataLayer m_tileData;
	DataLayer m_lightSky;
	DataLayer m_lightBlk;
	uint8_t m_heightMap[256];
	int m_minHeight;
	ChunkPos m_chunkPos;
	bool m_bIsTerrainPopulated;
	bool m_bUnsaved;
	bool m_bDontSave;
	uint8_t field_237;
	int m_lastSaveHadEntities;
	int64_t m_lastSaveTime;
	TileID* m_pBlockData;
	std::vector<std::shared_ptr<Entity>> m_entities[128 / 16];
	std::unordered_map<ChunkTilePos, std::shared_ptr<TileEntity>> m_tileEntities;
};
