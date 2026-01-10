/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "world/level/Level.hpp"
#include "world/phys/AABB.hpp"
#include "world/entity/EntityIO.hpp"
#include <common/ListTag.hpp>

bool LevelChunk::touchedSky = false;

LevelChunk::~LevelChunk()
{
	SAFE_DELETE_ARRAY(m_pBlockData);
}


// Temp function for checking oob accesses into levelchunks
void CheckPosition(const ChunkTilePos& pos)
{
	assert(pos.x >= 0 && pos.y >= 0 && pos.z >= 0 && pos.x < 16 && pos.z < 16 && pos.y < 128);
}

void LevelChunk::_init()
{
	field_4 = 0;
	m_bLoaded = false;
	m_chunkPos = ChunkPos(0, 0);
	m_bIsTerrainPopulated = false;
	m_bUnsaved = false;
	m_bDontSave = 0;
	field_237 = 0;
	m_lastSaveHadEntities = false;
	m_lastSaveTime = 0;
	m_pBlockData = nullptr;
}

void LevelChunk::save(CompoundIO levelTag) 
{
	levelTag->putInt("xPos", m_chunkPos.x);
	levelTag->putInt("zPos", m_chunkPos.z);
	levelTag->putLong("LastUpdate", m_pLevel->getTime());
	levelTag->putByteArray("Blocks", m_pBlockData, 32768);
	levelTag->putByteArray("Data", m_tileData.m_pData, m_tileData.m_size);
	levelTag->putByteArray("SkyLight", m_lightSky.m_pData, m_lightSky.m_size);
	levelTag->putByteArray("BlockLight", m_lightBlk.m_pData, m_lightBlk.m_size);
	levelTag->putByteArray("HeightMap", m_heightMap, 256);

	levelTag->putBoolean("TerrainPopulated", m_bIsTerrainPopulated);

	auto entities = std::make_shared<ListTag>(TagType::TAG_Compound);
	for (auto& p : m_entities)
	{
		for (auto& entity : p) 
		{
			auto entityTag = std::make_shared<CompoundTag>();
			if (entity->save(entityTag))
				entities->add(entityTag);
		}
	}
	levelTag->put("Entities", entities);

	auto tileEntities = std::make_shared<ListTag>(TagType::TAG_Compound);
	for (auto& p : m_tileEntities)
	{
		auto tileEntityTag = std::make_shared<CompoundTag>();
		if (p.second) p.second->save(tileEntityTag);
		tileEntities->add(tileEntityTag);
	}
	levelTag->put("TileEntities", tileEntities);
}

void LevelChunk::load(CompoundIO tag) 
{
	memcpy(m_pBlockData, tag->getByteArray("Blocks").data(), 32768);
	memcpy(m_tileData.m_pData, tag->getByteArray("Data").data(), m_tileData.m_size);

	bool updateLight = false;

	if (tag->contains("SkyLight"))
		memcpy(m_lightSky.m_pData, tag->getByteArray("SkyLight").data(), m_lightSky.m_size);
	else updateLight = true;

	memcpy(m_lightBlk.m_pData, tag->getByteArray("BlockLight").data(), m_lightBlk.m_size);

	if (tag->contains("HeightMap"))
		memcpy(m_heightMap, tag->getByteArray("HeightMap").data(), 256);
	else updateLight = true;

	if (updateLight) lightLava();

	auto entities = tag->getList("Entities");

	for (auto& entityTag : entities->getValue())
	{
		auto entity = EntityIO::loadStatic(std::dynamic_pointer_cast<CompoundTag>(entityTag), m_pLevel);
		m_lastSaveHadEntities = true;
		if (entity) addEntity(entity);
	}

	auto tileEntities = tag->getList("TileEntities");

	for (auto& entityTag : tileEntities->getValue())
	{
		auto tileEntity = TileEntity::loadStatic(std::dynamic_pointer_cast<CompoundTag>(entityTag));
		if (tileEntity) addTileEntity(tileEntity);
	}

	int savedX = tag->getInt("xPos");
	int savedZ = tag->getInt("zPos");

	if (savedX != m_chunkPos.x || savedZ != m_chunkPos.z)
		LOG_I("Chunk file at %d,%d is in the wrong location; relocating. (Expected %d, %d, got %d, %d)", m_chunkPos.x, m_chunkPos.z, m_chunkPos.x, m_chunkPos.z, savedX, savedZ);
	m_bIsTerrainPopulated = tag->getBoolean("TerrainPopulated");
}

LevelChunk::LevelChunk(Level* pLevel, const ChunkPos& pos) :
	m_tileData(nullptr),
	m_lightSky(nullptr),
	m_lightBlk(nullptr)
{
	_init();

	m_pLevel = pLevel;
	m_chunkPos = pos;

	init();
}

LevelChunk::LevelChunk(Level* pLevel, TileID* pData, const ChunkPos& pos) :
	m_tileData(0x4000),
	m_lightSky(0x4000),
	m_lightBlk(0x4000)
{
	_init();

	m_pLevel = pLevel;

	m_pBlockData = pData;

	m_chunkPos = pos;

	init();
}

void LevelChunk::init()
{
	m_bIsTerrainPopulated = false;
	m_bUnsaved = false;
	field_237 = 0;
	m_lastSaveHadEntities = false;
	m_lastSaveTime = 0;
	memset(m_heightMap, 0, sizeof m_heightMap);
}

void LevelChunk::unload()
{
	m_bLoaded = false;

	for (auto& te : m_tileEntities)
	{
		te.second->setRemoved();
	}

	for (auto& t : m_entities)
		m_pLevel->removeEntities(t);
}

bool LevelChunk::isAt(const ChunkPos& pos)
{
	return m_chunkPos == pos;
}

int LevelChunk::getHeightmap(const ChunkTilePos& pos)
{
	CheckPosition(pos);
	return m_heightMap[MakeHeightMapIndex(pos)];
}

void LevelChunk::recalcHeightmap()
{
	int minHeight = 127;

	ChunkTilePos ctp;
	for (ctp.x = 0; ctp.x < 16; ctp.x++)
	{
		for (ctp.z = 0; ctp.z < 16; ctp.z++)
		{
			ctp.y = 127;
		
			for (; ctp.y > 0 && Tile::lightBlock[getTile(ChunkTilePos(ctp.x, ctp.y - 1, ctp.z))] == 0; ctp.y--) {
			}

			m_heightMap[ctp.z << 4 | ctp.x] = ctp.y;

			if (ctp.y < minHeight)
				minHeight = ctp.y;

			if (!m_pLevel->m_pDimension->m_bHasCeiling)
			{
				int light = 15;
				ctp.y = 127;

				do
				{
					light -= Tile::lightBlock[getTile(ctp)];
					if (light > 0)
						m_lightSky.set(ctp, light);

					--ctp.y;
				} while (ctp.y > 0 && light > 0);
			}
		}
	}

	m_minHeight = minHeight;

	for (ctp.x = 0; ctp.x < 16; ctp.x++)
		for (ctp.z = 0; ctp.z < 16; ctp.z++)
			lightGaps(ctp);

	m_bUnsaved = true;
}

void LevelChunk::recalcHeightmapOnly()
{
	int minHeight = 127;

	ChunkTilePos ctp;
	for (ctp.x = 0; ctp.x < 16; ctp.x++)
	{
		for (ctp.z = 0; ctp.z < 16; ctp.z++)
		{
			ctp.y = 127;


			for (; ctp.y > 0 && Tile::lightBlock[getTile(ChunkTilePos(ctp.x, ctp.y - 1, ctp.z))] == 0; ctp.y--) {
			}

			m_heightMap[ctp.z << 4 | ctp.x] = ctp.y;

			if (ctp.y < minHeight)
				minHeight = ctp.y;
		}
	}

	m_minHeight = minHeight;

	m_bUnsaved = true;
}

void LevelChunk::lightGaps(const ChunkTilePos& pos)
{
	CheckPosition(pos);
	uint8_t heightMap = getHeightmap(pos);
	TilePos actual = pos.toTilePos(m_chunkPos);;

	for (Facing::Name rel : Facing::HORIZONTAL)
	{
		lightGap(actual.relative(rel), heightMap);
	}
}

void LevelChunk::lightGap(const TilePos& pos, uint8_t heightMap)
{
	// @BUG: This is flawed. getHeightmap calls getChunk which creates another chunk and calls this over
	// and over again, creating a stack overflow. Since the level is limited a stack overflow is only
	// possible on devices with really tight stacks.
	uint8_t currHeightMap = m_pLevel->getHeightmap(pos);
	if (currHeightMap > heightMap)
	{
		m_pLevel->updateLight(LightLayer::Sky, TilePos(pos.x, heightMap, pos.z), TilePos(pos.x, currHeightMap, pos.z));
		m_bUnsaved = true;
	}
	else if (currHeightMap < heightMap)
	{
		m_pLevel->updateLight(LightLayer::Sky, TilePos(pos.x, currHeightMap, pos.z), TilePos(pos.x, heightMap, pos.z));
		m_bUnsaved = true;
	}
}

int LevelChunk::getBrightness(const LightLayer& ll, const ChunkTilePos& pos)
{
	CheckPosition(pos);

	if (&ll == &LightLayer::Sky)
	{
		return m_lightSky.get(pos);
	}

	if (&ll == &LightLayer::Block)
	{
		return m_lightBlk.get(pos);
	}

	return 0;
}

void LevelChunk::setBrightness(const LightLayer& ll, const ChunkTilePos& pos, int brightness)
{
	m_bUnsaved = true;
	CheckPosition(pos);

	if (&ll == &LightLayer::Sky)
	{
		m_lightSky.set(pos, brightness);
		return;
	}

	if (&ll == &LightLayer::Block)
	{
		m_lightBlk.set(pos, brightness);
		return;
	}
}

int LevelChunk::getRawBrightness(const ChunkTilePos& pos, int skySubtract)
{
	CheckPosition(pos);

	int bSky = m_lightSky.get(pos);

	if (bSky > 0)
		touchedSky = true;

	bSky -= skySubtract;

	int bBlk = m_lightBlk.get(pos);

	// if it's smaller than 0 it'll probably sort itself out
	if (bBlk > bSky)
		bSky = bBlk;
	
	return bSky;
}

void LevelChunk::addEntity(std::shared_ptr<Entity> pEnt)
{
	assert(pEnt != nullptr); // Cannot add a null entity
	m_lastSaveHadEntities = true;

	int yCoord = ChunkPos::ToChunkCoordinate((int)pEnt->m_pos.y);
	if (yCoord < 0) yCoord = 0;
	if (yCoord > 7) yCoord = 7;
	pEnt->m_bInChunk = true;
	pEnt->m_chunkPos = m_chunkPos;
	pEnt->m_chunkPosY = yCoord;

	m_entities[yCoord].push_back(pEnt);
}

void LevelChunk::updateEntity(std::shared_ptr<Entity> pEnt)
{
	assert(pEnt != nullptr);
	assert(pEnt->m_bInChunk);
	assert(pEnt->m_chunkPos == m_chunkPos);

	int newYCoord = ChunkPos::ToChunkCoordinate((int)pEnt->m_pos.y);
	if (newYCoord < 0) newYCoord = 0;
	if (newYCoord > 7) newYCoord = 7;

	int oldYCoord = pEnt->m_chunkPosY;
	if (oldYCoord == newYCoord)
	{
		return;
	}

	if (oldYCoord < 0 || oldYCoord > 7)
	{
		assert(false);
		return;
	}

	std::vector<std::shared_ptr<Entity>>& oldTerrainLayer = m_entities[oldYCoord];
	std::vector<std::shared_ptr<Entity>>& newTerrainLayer = m_entities[newYCoord];

	std::vector<std::shared_ptr<Entity>>::iterator it = std::find(oldTerrainLayer.begin(), oldTerrainLayer.end(), pEnt);
	if (it != oldTerrainLayer.end())
	{
		oldTerrainLayer.erase(it);
	}
	else
	{
		assert(false);
	}

	assert(std::find(newTerrainLayer.begin(), newTerrainLayer.end(), pEnt) == newTerrainLayer.end());
	newTerrainLayer.push_back(pEnt);
	pEnt->m_chunkPosY = newYCoord;
}

void LevelChunk::deleteBlockData()
{
	if (m_pBlockData)
		delete[] m_pBlockData;

	m_pBlockData = nullptr;
}

void LevelChunk::removeEntity(std::shared_ptr<Entity> pEnt)
{
	removeEntity(pEnt, pEnt->m_chunkPosY);
}

void LevelChunk::removeEntity(std::shared_ptr<Entity> pEnt, int vec)
{
	if (vec < 0) vec = 0;
	if (vec > 7) vec = 7;

	std::vector<std::shared_ptr<Entity>>::iterator it = std::find(m_entities[vec].begin(), m_entities[vec].end(), pEnt);

	if (it != m_entities[vec].end())
		m_entities[vec].erase(it);
}

bool LevelChunk::isSkyLit(const ChunkTilePos& pos)
{
	CheckPosition(ChunkTilePos(pos.x, 64, pos.z));

	return m_heightMap[MakeHeightMapIndex(pos)] <= pos.y;
}

void LevelChunk::lightLava()
{
	uint8_t var1 = 32;

	for (int var2 = 0; var2 < 16; ++var2) {
		for (int var3 = 0; var3 < 16; ++var3) {
			int var4 = var2 << 11 | var3 << 7;

			int var5;
			int var6;
			for (var5 = 0; var5 < 128; ++var5) {
				var6 = Tile::lightEmission[m_pBlockData[var4 + var5]];
				if (var6 > 0) {
					setBrightness(LightLayer::Block, ChunkTilePos(var2, var5, var3), var6);
				}
			}

			var5 = 15;

			for (var6 = var1 - 2; var6 < 128 && var5 > 0; setBrightness(LightLayer::Block, ChunkTilePos(var2, var6, var3), var5)) {
				++var6;
				uint8_t var7 = m_pBlockData[var4 + var6];
				int var8 = Tile::lightBlock[var7];
				int var9 = Tile::lightEmission[var7];
				if (var8 == 0) {
					var8 = 1;
				}

				var5 -= var8;
				if (var9 > var5) {
					var5 = var9;
				}

				if (var5 < 0) {
					var5 = 0;
				}
			}
		}
	}

	m_pLevel->updateLight(LightLayer::Block, TilePos(m_chunkPos, var1 - 1), TilePos(m_chunkPos + 16, var1 + 1));
	m_bUnsaved = true;
}

void LevelChunk::recalcBlockLights()
{

}

void LevelChunk::recalcHeight(const ChunkTilePos& pos)
{
	CheckPosition(ChunkTilePos(pos.x, 64, pos.z));

	int minY = m_heightMap[pos.z << 4 | pos.x] & 255;
	int maxY = minY;
	if (pos.y > minY)
		maxY = pos.y;


	for (int var6 = pos.x << 11 | pos.z << 7; maxY > 0 && Tile::lightBlock[m_pBlockData[var6 + maxY - 1] & 255] == 0; maxY--) {
	}

	if (maxY != minY)
	{
		m_pLevel->lightColumnChanged(pos.x, pos.z, maxY, minY);
		m_heightMap[pos.z << 4 | pos.x] = maxY;
		int globalX;
		int globalZ;
		if (maxY < m_minHeight)
			m_minHeight = maxY;
	
		else
		{
			globalX = 127;

			for (globalZ = 0; globalZ < 16; ++globalZ)
				for (int heightY = 0; heightY < 16; ++heightY)
					if ((m_heightMap[heightY << 4 | globalZ] & 255) < globalX)
						globalX = m_heightMap[heightY << 4 | globalZ] & 255;

			m_minHeight = globalX;
		}

		globalX = m_chunkPos.x * 16 + pos.x;
		globalZ = m_chunkPos.z * 16 + pos.z;
		ChunkTilePos ctp = pos;
		if (maxY < minY)
		{
			for (ctp.y = maxY; ctp.y < minY; ctp.y++)
				m_lightSky.set(ctp, 15);
		}
		else
		{
			m_pLevel->updateLight(LightLayer::Sky, TilePos(globalX, minY, globalZ), TilePos(globalX, maxY, globalZ));

			for (ctp.y = minY; ctp.y < maxY; ctp.y++)
				m_lightSky.set(ctp, 0);
		}

		int light = 15;

		for (ctp.y = maxY; ctp.y > 0 && light > 0; m_lightSky.set(ctp, light))
		{
			ctp.y--;
			int lightBlock = Tile::lightBlock[getTile(ctp)];
			if (lightBlock == 0)
				lightBlock = 1;

			light -= lightBlock;
			if (light < 0)
				light = 0;
		}

		while (ctp.y > 0 && Tile::lightBlock[getTile(ChunkTilePos(ctp.x, ctp.y - 1, ctp.z))] == 0)
			ctp.y--;

		if (ctp.y != maxY)
			m_pLevel->updateLight(LightLayer::Sky, TilePos(globalX - 1, ctp.y, globalZ - 1), TilePos(globalX + 1, maxY, globalZ + 1));

		m_bUnsaved = true;
	}
}

void LevelChunk::skyBrightnessChanged()
{

}

void LevelChunk::load()
{
	m_bLoaded = true;

	m_pLevel->addAllPendingTileEntities(m_tileEntities);

	for (auto& v : m_entities)
		m_pLevel->addEntities(v);
}

bool LevelChunk::shouldSave(bool force)
{
	if (m_bDontSave)
		return false;

	if (force)
	{
		if (m_lastSaveHadEntities && m_pLevel->getTime() != m_lastSaveTime)
			return true;
	} 
	else 
	{
		if (m_lastSaveHadEntities && m_pLevel->getTime() >= m_lastSaveTime + 600)
			return true;
	}

	return m_bUnsaved;
}

void LevelChunk::markUnsaved()
{
	m_bUnsaved = true;
}

TileID LevelChunk::getTile(const ChunkTilePos& pos)
{
	CheckPosition(pos);
	TileID tile = m_pBlockData[DataLayer::MakeBlockDataIndex(pos)];
	if (tile && !Tile::tiles[tile]) LOG_I("Unknown block: %d", tile);
	return tile;
}

int LevelChunk::countEntities()
{
	int n = 0;
	for (int i = 0; i < 8; i++)
		n += int(m_entities[i].size());
	return n;
}

const AABB& LevelChunk::getEntityAABB(Entity* entity)
{
	return entity->m_hitbox;
}

bool LevelChunk::setTile(const ChunkTilePos& pos, TileID tile)
{
	CheckPosition(pos);

	int index = DataLayer::MakeBlockDataIndex(pos);

	TileID oldTile = m_pBlockData[index];

	uint8_t height = m_heightMap[MakeHeightMapIndex(pos)];

	if (oldTile == tile)
		return false;

	TilePos tilePos(m_chunkPos, pos.y);
	tilePos.x += pos.x;
	tilePos.z += pos.z;
	m_pBlockData[index] = tile;
	if (oldTile)
	{
		Tile::tiles[oldTile]->onRemove(m_pLevel, tilePos);
	}

	m_tileData.set(pos, 0);

	if (Tile::lightBlock[tile])
	{
		if (height <= pos.y)
			recalcHeight(ChunkTilePos(pos.x, pos.y + 1, pos.z));
	}
	else if (height - 1 == pos.y)
	{
		recalcHeight(pos);
	}

	m_pLevel->updateLight(LightLayer::Sky, tilePos, tilePos);
	m_pLevel->updateLight(LightLayer::Block, tilePos, tilePos);

	lightGaps(pos);
	if (tile)
	{
		if (!m_pLevel->m_bIsOnline)
			Tile::tiles[tile]->onPlace(m_pLevel, tilePos);
	}

	m_bUnsaved = true;

	return true;
}

bool LevelChunk::setTileAndData(const ChunkTilePos& pos, TileID tile, int data)
{
	CheckPosition(pos);

	assert((data & ~0xF) == 0);
	data &= 0xF;

	int index = DataLayer::MakeBlockDataIndex(pos);

	TileID oldTile = m_pBlockData[index];

	uint8_t height = m_heightMap[MakeHeightMapIndex(pos)];

	if (oldTile == tile)
	{
		// make sure we're at least updating the data. If not, simply return false
		if (getData(pos) == data)
			return false;
	}

	TilePos tilePos(m_chunkPos, pos.y);
	tilePos.x += pos.x;
	tilePos.z += pos.z;
	m_pBlockData[index] = tile;
	if (oldTile)
	{
		Tile::tiles[oldTile]->onRemove(m_pLevel, tilePos);
	}

	// update the data value of the block
	m_tileData.set(pos, data);

	if (!m_pLevel->m_pDimension->m_bHasCeiling)
	{
		if (Tile::lightBlock[tile])
		{
			if (height <= pos.y)
				recalcHeight(ChunkTilePos(pos.x, pos.y + 1, pos.z));
		}
		else if (height - 1 == pos.y)
		{
			recalcHeight(pos);
		}
		m_pLevel->updateLight(LightLayer::Sky, tilePos, tilePos);
	}

	m_pLevel->updateLight(LightLayer::Block, tilePos, tilePos);

	if (tile)
	{
		if (!m_pLevel->m_bIsOnline)
			Tile::tiles[tile]->onPlace(m_pLevel, tilePos);
	}

	m_bUnsaved = true;

	return true;
}

int LevelChunk::getData(const ChunkTilePos& pos)
{
	CheckPosition(pos);

	return m_tileData.get(pos);
}

void LevelChunk::setData(const ChunkTilePos& pos, int data)
{
	CheckPosition(pos);

	m_tileData.set(pos, data);
	m_bUnsaved = true;
}

// seems to set block data in 8192 block (4*16*128) chunks for some reason ?
// This function appears to be unused, and is completely removed as of 0.9.2
void LevelChunk::setBlocks(uint8_t* pData, int y)
{
	LOG_I("LevelChunk::setBlocks");
	for (int i = 0; i < 8192; i++)
	{
		m_pBlockData[8192 * y + i] = pData[i];
	}

	int x1 = 4 * y;
	int x2 = 4 * (y + 1);
	if (4 * y < x2)
	{
		for (int x3 = 4 * y; x3 < x2; x3++)
		{
			for (int j = 0; j < 16; j++)
			{
				recalcHeight(ChunkTilePos(x3, 0, j));
			}
		}
	}

	TilePos tilePos(m_chunkPos, 0);
	tilePos.x += x1;
	TilePos tilePos2 = tilePos;
	tilePos.x += 4;
	tilePos.y = 128;
	tilePos.x += 16;

	m_pLevel->updateLight(LightLayer::Sky,   tilePos, tilePos2);
	m_pLevel->updateLight(LightLayer::Block, tilePos, tilePos2);
	m_pLevel->setTilesDirty(tilePos, tilePos2);
}

int LevelChunk::setBlocksAndData(uint8_t* pData, int xStart, int yStart, int zStart, int xEnd, int yEnd, int zEnd, int dataOffset)
{
	LOG_I("LevelChunk::setBlocksAndData");

	const int height = yEnd - yStart;
	const int area = height;
	const int metaSz = area / 2;

	ChunkTilePos ctp(0, yStart, 0);

	for (ctp.x = xStart; ctp.x < xEnd; ++ctp.x)
	{
		for (ctp.z = zStart; ctp.z < zEnd; ++ctp.z)
		{
			const int index = DataLayer::MakeBlockDataIndex(ctp);
			memcpy(&m_pBlockData[index], &pData[dataOffset], area);
			dataOffset += area;
		}
	}

	recalcHeightmapOnly();


	for (ctp.x = xStart; ctp.x < xEnd; ++ctp.x)
	{
		for (ctp.z = zStart; ctp.z < zEnd; ++ctp.z)
		{
			const int index = DataLayer::MakeBlockDataIndex(ctp) >> 1;
			memcpy(&m_tileData.m_pData[index], &pData[dataOffset], metaSz);
			dataOffset += metaSz;
		}
	}

	for (ctp.x = xStart; ctp.x < xEnd; ++ctp.x)
	{
		for (ctp.z = zStart; ctp.z < zEnd; ++ctp.z)
		{
			const int index = DataLayer::MakeBlockDataIndex(ctp) >> 1;
			memcpy(&m_lightBlk.m_pData[index], &pData[dataOffset], metaSz);
			dataOffset += metaSz;
		}
	}

	for (ctp.x = xStart; ctp.x < xEnd; ++ctp.x)
	{
		for (ctp.z = zStart; ctp.z < zEnd; ++ctp.z)
		{
			const int index = DataLayer::MakeBlockDataIndex(ctp) >> 1;
			memcpy(&m_lightSky.m_pData[index], &pData[dataOffset], metaSz);
			dataOffset += metaSz;
		}
	}

	return dataOffset;
}

std::shared_ptr<TileEntity> LevelChunk::getTileEntity(const ChunkTilePos& pos)
{
	auto it = m_tileEntities.find(pos);
	if (it == m_tileEntities.end()) {
		int var6 = getTile(pos);
		if (!var6 || !Tile::isEntityTile[var6]) {
			return nullptr;
		}

		Tile* var7 = Tile::tiles[var6];
		var7->onPlace(m_pLevel, pos.toTilePos(m_chunkPos));
		return m_tileEntities[pos];
	}

	if (!it->second || it->second->isRemoved())
	{
		m_tileEntities.erase(it);
		return nullptr;
	}
	else
		return it->second;
}

void LevelChunk::addTileEntity(std::shared_ptr<TileEntity> tileEntity)
{
	setTileEntity(tileEntity->m_pos, tileEntity);
	if (m_bLoaded)
		m_pLevel->m_tileEntityList.push_back(tileEntity);
}

int LevelChunk::getBlocksAndData(uint8_t* pData, int xStart, int yStart, int zStart, int xEnd, int yEnd, int zEnd, int dataOffset)
{
	if (xStart >= xEnd)
	{
		recalcHeightmapOnly();
		return dataOffset;
	}

	ChunkTilePos ctp(0, yStart, 0);

	// load the tile IDs
	int x2 = yEnd - yStart;
	for (ctp.x = xStart; ctp.x < xEnd; ctp.x++)
	{
		if (zStart >= zEnd) continue;

		uint8_t* dst = &pData[dataOffset];

		for (ctp.z = zStart; ctp.z < zEnd; ctp.z++)
		{
			uint8_t* src = &m_pBlockData[DataLayer::MakeBlockDataIndex(ctp)];
			memcpy(dst, src, x2 * sizeof(TileID));
			dst += x2 * sizeof(TileID);
			dataOffset += x2 * sizeof(TileID);
		}
	}

	recalcHeightmapOnly();

	int x5 = x2 / 2;

	// load the tile data
	for (ctp.x = xStart; ctp.x < xEnd; ctp.x++)
	{
		if (zStart >= zEnd) continue;

		uint8_t* dst = &pData[dataOffset];

		for (ctp.z = zStart; ctp.z < zEnd; ctp.z++)
		{
			uint8_t* src = &m_tileData.m_pData[DataLayer::MakeBlockDataIndex(ctp) >> 1];
			memcpy(dst, src, x5);
			dst += x5;
			dataOffset += x5;
		}
	}

	// load the block lights
	for (ctp.x = xStart; ctp.x < xEnd; ctp.x++)
	{
		if (zStart >= zEnd) continue;

		uint8_t* dst = &pData[dataOffset];

		for (ctp.z = zStart; ctp.z < zEnd; ctp.z++)
		{
			uint8_t* src = &m_lightBlk.m_pData[DataLayer::MakeBlockDataIndex(ctp) >> 1];
			memcpy(dst, src, x5);
			dst += x5;
			dataOffset += x5;
		}
	}

	// load the sky lights
	for (ctp.x = xStart; ctp.x < xEnd; ctp.x++)
	{
		if (zStart >= zEnd) continue;

		uint8_t* dst = &pData[dataOffset];

		for (ctp.z = zStart; ctp.z < zEnd; ctp.z++)
		{
			uint8_t* src = &m_lightSky.m_pData[DataLayer::MakeBlockDataIndex(ctp) >> 1];
			memcpy(dst, src, x5);
			dst += x5;
			dataOffset += x5;
		}
	}

	return dataOffset;
}

void LevelChunk::setTileEntity(const ChunkTilePos& pos, std::shared_ptr<TileEntity> tileEntity)
{
	tileEntity->m_pLevel = m_pLevel;
	auto tile = getTile(pos);
	tileEntity->m_pos = pos.toTilePos(m_chunkPos);
	if (tile && Tile::isEntityTile[tile])
	{
		tileEntity->clearRemoved();
		m_tileEntities[pos] = tileEntity;
	}
	else
		LOG_I("Attempted to place a tile entity where there was no entity tile! %d", tile);
}

void LevelChunk::removeTileEntity(const ChunkTilePos& pos)
{
	if (m_bLoaded)
	{
		auto it = m_tileEntities.find(pos);
		if (it != m_tileEntities.end())
		{
			if (it->second) it->second->setRemoved();
			m_tileEntities.erase(it);
		}
	}
}

Random LevelChunk::getRandom(int64_t l)
{
	Random random;

	int levelSeed = m_pLevel->getSeed();
	int chunkSeed = m_chunkPos.x * (4987142 * m_chunkPos.x + 5947611) + m_chunkPos.z * (4392871 * m_chunkPos.z + 389711);

	random.setSeed((levelSeed + chunkSeed) ^ l);

	return random;
}

bool LevelChunk::isEmpty()
{
	return false;
}
