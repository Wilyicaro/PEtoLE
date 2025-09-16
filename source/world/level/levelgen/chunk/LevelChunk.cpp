/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "world/level/Level.hpp"
#include "world/phys/AABB.hpp"
#include "world/entity/EntityIO.hpp"

bool LevelChunk::touchedSky = false;

LevelChunk::~LevelChunk()
{
	SAFE_DELETE(m_lightBlk);
	SAFE_DELETE(m_lightSky);
	SAFE_DELETE(m_tileData);
	SAFE_DELETE(m_pBlockData);
}

constexpr int MakeBlockDataIndex (const ChunkTilePos& pos)
{
	return (pos.x << 11) | (pos.z << 7) | pos.y;
}

constexpr int MakeHeightMapIndex(const ChunkTilePos& pos)
{
	return pos.x | (pos.z * 16);
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
	m_tileData = nullptr;
	m_tileDataCnt = 0;
	m_lightSky = nullptr;
	m_lightSkyCnt = 0;
	m_lightBlk = nullptr;
	m_lightBlkCnt = 0;
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
	levelTag->putByteArray("Blocks", m_pBlockData, 32768);
	levelTag->putByteArray("Data", m_tileData, 16384);
	levelTag->putByteArray("SkyLight", m_lightSky, 16384);
	levelTag->putByteArray("BlockLight", m_lightBlk, 16384);
	levelTag->putByteArray("HeightMap", m_heightMap, 256);

	levelTag->putLong("LastUpdate", m_pLevel->getTime());
	levelTag->putInt("xPos", m_chunkPos.x);
	levelTag->putInt("zPos", m_chunkPos.z);
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
		p.second->save(tileEntityTag);
		tileEntities->add(tileEntityTag);
	}
	levelTag->put("TileEntities", tileEntities);
}

void LevelChunk::load(CompoundIO tag) 
{
	memcpy(m_pBlockData, tag->getByteArray("Blocks").data(), 32768);
	memcpy(m_tileData, tag->getByteArray("Data").data(), 16384);

	bool updateLight = false;

	if (tag->contains("SkyLight"))
		memcpy(m_lightSky, tag->getByteArray("SkyLight").data(), 16384);
	else updateLight = true;

	memcpy(m_lightBlk, tag->getByteArray("BlockLight").data(), 16384);

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

LevelChunk::LevelChunk(Level* pLevel, const ChunkPos& pos)
{
	_init();

	m_pLevel = pLevel;
	m_chunkPos = pos;

	init();
}

LevelChunk::LevelChunk(Level* pLevel, TileID* pData, const ChunkPos& pos)
{
	_init();

	m_pLevel = pLevel;

	// The game will blow its brains out if this if statement is executed
	// I have not the slightest idea as to why...
	/*if (pData)
	{*/
	m_tileDataCnt = 0x4000;
	field_4 = 0x8000;
	m_tileData = new uint8_t[m_tileDataCnt];
	memset(m_tileData, 0, m_tileDataCnt);

	//Space saving measure: Store 2 blocks' light field instead of only one block's, per byte.
	m_lightSkyCnt = 16 * 16 * 128 / 2;
	m_lightSky = new uint8_t[m_lightSkyCnt];
	memset(m_lightSky, 0, m_lightSkyCnt);

	m_lightBlkCnt = 16 * 16 * 128 / 2;
	m_lightBlk = new uint8_t[m_lightBlkCnt];
	memset(m_lightBlk, 0, m_lightBlkCnt);

	m_pBlockData = pData;
	//}

	m_chunkPos = TilePos(pos, 0);

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
	int x1 = 127;

	ChunkTilePos pos(0, 0, 0);

	for (pos.x = 0; pos.x < 16; pos.x++)
	{
		for (pos.z = 0; pos.z < 16; pos.z++)
		{
			pos.y = 0;
			int index1 = MakeBlockDataIndex(pos);
			pos.y = 126;

			int x2 = 0;
			while (!Tile::lightBlock[getTile(pos)])
			{
				if (pos.y-- == 0)
				{
					x2 = 0;
					goto label_7;
				}
			}

			x2 = pos.y + 1;
		label_7:
			if (x1 >= x2)
				x1 = x2;
			pos.y = 0;
			m_heightMap[MakeHeightMapIndex(pos)] = x2;

			if (!m_pLevel->m_pDimension->m_bHasCeiling)
			{
				int x4 = 15;
				for (pos.y = 127; pos.y > 0; pos.y--)
				{
					x4 -= Tile::lightBlock[getTile(pos)];
					if (x4 <= 0)
						break;

					int x = pos.y + index1;
					int index = x >> 1, offs = x & 1;

					if (offs)
						m_lightSky[index] = (m_lightSky[index] & 0x0F) | (x4 << 4); // set the upper 4 bits to x4
					else
						m_lightSky[index] = (m_lightSky[index] & 0xF0) | x4; // set the lower 4 bits to x4
				}
			}
		}
	}

	m_minHeight = x1;
	if (!m_bDontSave) {
		for (int i = 0; i < 16; i++)
		{
			for (int j = 0; j < 16; j++)
			{
				lightGaps(ChunkTilePos(i, 0, j));
			}
		}
	}
	m_bUnsaved = true;
}

void LevelChunk::recalcHeightmapOnly()
{
	int x1 = 127;

	ChunkTilePos pos(0, 0, 0);
	for (pos.x = 0; pos.x < 16; pos.x++)
	{
		for (pos.z = 0; pos.z < 16; pos.z++)
		{
			pos.y = 0;
			int index1 = MakeBlockDataIndex(pos);
			pos.y = 126;

			int x2 = 0;
			while (!Tile::lightBlock[getTile(pos)])
			{
				if (pos.y-- == 0)
				{
					x2 = 0;
					goto label_7;
				}
			}

			x2 = pos.y + 1;
			pos.y = 0;
		label_7:
			if (x1 >= x2)
				x1 = x2;
			m_heightMap[MakeHeightMapIndex(pos)] = x2;
		}
	}

	m_minHeight = x1;

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

	// why the hell is it doing it like that.
	if (&ll == &LightLayer::Sky)
	{
		int bitIdx = MakeBlockDataIndex(pos);
		int index = bitIdx >> 1, offs = bitIdx & 1;
		if (offs)
			return m_lightSky[index] >> 4;
		else
			return m_lightSky[index] & 0xF;
	}

	if (&ll == &LightLayer::Block)
	{
		int bitIdx = MakeBlockDataIndex(pos);
		int index = bitIdx >> 1, offs = bitIdx & 1;
		if (offs)
			return m_lightBlk[index] >> 4;
		else
			return m_lightBlk[index] & 0xF;
	}

	return 0;
}

void LevelChunk::setBrightness(const LightLayer& ll, const ChunkTilePos& pos, int brightness)
{
	m_bUnsaved = true;
	CheckPosition(pos);
	// why the hell is it doing it like that.

	if (&ll == &LightLayer::Sky)
	{
		int bitIdx = MakeBlockDataIndex(pos);
		int index = bitIdx >> 1, offs = bitIdx & 1;
		if (offs)
			m_lightSky[index] = (m_lightSky[index] & 0x0F) | (brightness << 4);
		else
			m_lightSky[index] = (m_lightSky[index] & 0xF0) | (brightness & 0xF);

		return;
	}

	if (&ll == &LightLayer::Block)
	{
		int bitIdx = MakeBlockDataIndex(pos);
		int index = bitIdx >> 1, offs = bitIdx & 1;
		if (offs)
			m_lightBlk[index] = (m_lightBlk[index] & 0x0F) | (brightness << 4);
		else
			m_lightBlk[index] = (m_lightBlk[index] & 0xF0) | (brightness & 0xF);

		return;
	}
}

int LevelChunk::getRawBrightness(const ChunkTilePos& pos, int skySubtract)
{
	CheckPosition(pos);
	int bitIdx = MakeBlockDataIndex(pos);
	int index = bitIdx >> 1, offs = bitIdx & 1;

	uint8_t bSky, bBlk;
	if (offs)
		bSky = m_lightSky[index] >> 4;
	else
		bSky = m_lightSky[index] & 0xF;

	if (m_lightSky)
		touchedSky = true;

	int br = bSky - skySubtract;

	if (offs)
		bBlk = m_lightBlk[index] >> 4;
	else
		bBlk = m_lightBlk[index] & 0xF;

	// if it's smaller than 0 it'll probably sort itself out
	if (br < bBlk)
		br = bBlk;
	
	return br;
}

void LevelChunk::addEntity(std::shared_ptr<Entity> pEnt)
{
	assert(pEnt != nullptr); // Cannot add a null entity
	m_lastSaveHadEntities = true;

	int yCoord = Mth::floor(pEnt->m_pos.y / 16);
	if (yCoord < 0) yCoord = 0;
	if (yCoord > 7) yCoord = 7;
	pEnt->m_bInChunk = true;
	pEnt->m_chunkPos = m_chunkPos;
	pEnt->m_chunkPosY = yCoord;

	m_entities[yCoord].push_back(pEnt);
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

	uint8_t& hmap = m_heightMap[MakeHeightMapIndex(pos)];

	int index = MakeBlockDataIndex(ChunkTilePos(pos.x, 0, pos.z));

	int x1 = hmap;
	if (x1 < pos.y)
		x1 = pos.y;

	if (x1 > 0)
	{
		while (!Tile::lightBlock[m_pBlockData[x1 - 1 + index]] && x1)
		{
			--x1;
		}
	}

	if (x1 != hmap)
	{
		m_pLevel->lightColumnChanged(pos.x, pos.z, x1, hmap);
		hmap = x1;
		if (m_minHeight <= x1)
		{
			int x6 = 127;
			ChunkTilePos pos(0, 0, 0);
			for (pos.x = 0; pos.x < 16; pos.x++)
			{
				for (pos.z = 0; pos.z < 16; pos.z++)
				{
					int x = m_heightMap[MakeHeightMapIndex(pos)];
					if (x6 >= x)
						x6 = x;
				}
			}

			m_minHeight = x6;
		}
		else
		{
			m_minHeight = x1;
		}

		int globalX = pos.x + 16 * m_chunkPos.x;
		int globalZ = pos.z + 16 * m_chunkPos.z;
		if (hmap <= x1)
		{
			m_pLevel->updateLight(LightLayer::Sky, TilePos(globalX, hmap, globalZ), TilePos(globalX, x1, globalZ));

			if (hmap < x1)
			{
				for (int i = 0; i < x1 - hmap; i++)
				{
					int bitIdx = index | (i + hmap);
					int _idx = bitIdx >> 1;
					int _off = bitIdx & 1;

					if (_off)
						m_lightSky[_idx] &= 0xF0;
					else
						m_lightSky[_idx] &= 0x0F;
				}
			}
		}
		else
		{
			for (int i = 0; i < hmap; i++)
			{
				int v15 = (i | index) >> 1;
				//int v16 = (i | index) <<31;

				if ((i | index) & 1)
				{
					m_lightSky[v15] = (m_lightSky[v15] & 0xF0) | 0x0F;
				}
				else
				{
					m_lightSky[v15] = (m_lightSky[v15] & 0x0F) | 0xF0;
				}
			}
		}

		int x2 = x1;
		int x3 = 15;
		while (x3 > 0 && x2 > 0)
		{
			TileID tile = getTile(ChunkTilePos(pos.x, --x2, pos.z));
			int bitIdx = x2 | index;

			int x4 = Tile::lightBlock[tile];
			if (!x4)
				x4 = 1; //@HUH: what is this?

			int x5 = x3 - x4;
			int _idx = bitIdx >> 1;
			int _off = (bitIdx & 1);

			x3 = x5;
			if (x3 < 0)
				x3 = 0;

			if (_off)
				m_lightSky[_idx] = (m_lightSky[_idx] & 0x0F) | ((x3) << 4);
			else
				m_lightSky[_idx] = (m_lightSky[_idx] & 0xF0) | x3;
		}

		if (x2 > 0)
		{
			for (; x2 > 0; x2--)
			{
				if (Tile::lightBlock[getTile(ChunkTilePos(pos.x, x2 - 1, pos.z))])
					break;
			}
		}

		if (x2 != x1)
		{
			m_pLevel->updateLight(LightLayer::Sky, TilePos(globalX - 1, x2, globalZ - 1), TilePos(globalX + 1, x1, globalZ + 1));
		}
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
	TileID tile = m_pBlockData[MakeBlockDataIndex(pos)];
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

	int index = MakeBlockDataIndex(pos);

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

	// clear the data value of the block
	if (index & 1)
		m_tileData[index >> 1] &= 0xF;
	else
		m_tileData[index >> 1] &= 0xF0;

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

	int index = MakeBlockDataIndex(pos);

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
	if (index & 1)
		m_tileData[index >> 1] = (m_tileData[index >> 1] & 0x0F) | (data << 4);
	else
		m_tileData[index >> 1] = (m_tileData[index >> 1] & 0xF0) | (data);

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

	int index = MakeBlockDataIndex(pos);

	uint8_t data = m_tileData[index >> 1];
	if (index & 1)
		return data >> 4;
	return data & 0xF;
}

void LevelChunk::setData(const ChunkTilePos& pos, int data)
{
	CheckPosition(pos);

	assert((data & ~0xF) == 0);
	data &= 0xF;

	int index = MakeBlockDataIndex(pos);

	uint8_t& xdata = m_tileData[index >> 1];
	if (index & 1)
		xdata = (xdata & 0x0F) | (data << 4);
	else
		xdata = (xdata & 0xF0) | (data);

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

// This function appears to be unused, and is completely removed as of 0.9.2
int LevelChunk::setBlocksAndData(uint8_t* pData, int xStart, int yStart, int zStart,
	int xEnd, int yEnd, int zEnd, int dataOffset)
{
	LOG_I("LevelChunk::setBlocksAndData");

	const int height = yEnd - yStart;
	const int area = height;
	const int metaSz = area / 2;

	for (int x = xStart; x < xEnd; ++x)
	{
		for (int z = zStart; z < zEnd; ++z)
		{
			const int index = MakeBlockDataIndex(ChunkTilePos(x, yStart, z));
			memcpy(&m_pBlockData[index], &pData[dataOffset], area);
			dataOffset += area;
		}
	}

	recalcHeightmapOnly();


	for (int x = xStart; x < xEnd; ++x)
	{
		for (int z = zStart; z < zEnd; ++z)
		{
			const int index = MakeBlockDataIndex(ChunkTilePos(x, yStart, z)) >> 1;
			memcpy(&m_tileData[index], &pData[dataOffset], metaSz);
			dataOffset += metaSz;
		}
	}

	for (int x = xStart; x < xEnd; ++x)
	{
		for (int z = zStart; z < zEnd; ++z)
		{
			const int index = MakeBlockDataIndex(ChunkTilePos(x, yStart, z)) >> 1;
			memcpy(&m_lightBlk[index], &pData[dataOffset], metaSz);
			dataOffset += metaSz;
		}
	}

	for (int x = xStart; x < xEnd; ++x)
	{
		for (int z = zStart; z < zEnd; ++z)
		{
			const int index = MakeBlockDataIndex(ChunkTilePos(x, yStart, z)) >> 1;
			memcpy(&m_lightSky[index], &pData[dataOffset], metaSz);
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

// This function appears to be unused, and is completely removed as of 0.9.2
int LevelChunk::getBlocksAndData(uint8_t* pData, int a3, int a4, int a5, int a6, int a7, int a8, int a9)
{
	if (a3 >= a6)
	{
		recalcHeightmapOnly();
		return a9;
	}

	// load the tile IDs
	int x2 = a7 - a4;
	for (int x1 = a3; x1 < a6; x1++)
	{
		if (a5 >= a8) continue;

		uint8_t* dst = &pData[a9];

		for (int x3 = a5; x3 < a8; x3++)
		{
			uint8_t* src = &m_pBlockData[MakeBlockDataIndex(ChunkTilePos(x1, a4, x3))];
			memcpy(dst, src, x2 * sizeof(TileID));
			dst += x2 * sizeof(TileID);
			a9 += x2 * sizeof(TileID);
		}
	}

	recalcHeightmapOnly();

	int x5 = x2 / 2;

	// load the tile data
	for (int x1 = a3; x1 < a6; x1++)
	{
		if (a5 >= a8) continue;

		uint8_t* dst = &pData[a9];

		for (int x3 = a5; x3 < a8; x3++)
		{
			uint8_t* src = &m_tileData[MakeBlockDataIndex(ChunkTilePos(x1, a4, x3)) >> 1];
			memcpy(dst, src, x5);
			dst += x5;
			a9 += x5;
		}
	}

	// load the block lights
	for (int x1 = a3; x1 < a6; x1++)
	{
		if (a5 >= a8) continue;

		uint8_t* dst = &pData[a9];

		for (int x3 = a5; x3 < a8; x3++)
		{
			uint8_t* src = &m_lightBlk[MakeBlockDataIndex(ChunkTilePos(x1, a4, x3)) >> 1];
			memcpy(dst, src, x5);
			dst += x5;
			a9 += x5;
		}
	}

	// load the sky lights
	for (int x1 = a3; x1 < a6; x1++)
	{
		if (a5 >= a8) continue;

		uint8_t* dst = &pData[a9];

		for (int x3 = a5; x3 < a8; x3++)
		{
			uint8_t* src = &m_lightSky[MakeBlockDataIndex(ChunkTilePos(x1, a4, x3)) >> 1];
			memcpy(dst, src, x5);
			dst += x5;
			a9 += x5;
		}
	}

	return a9;
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
