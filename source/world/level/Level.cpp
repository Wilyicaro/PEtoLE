/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Level.hpp"

#include <algorithm>
#include "common/Util.hpp"
#include "world/level/levelgen/chunk/ChunkCache.hpp"
#include "Explosion.hpp"
#include "Region.hpp"
#include "world/tile/LiquidTile.hpp"
#include "world/entity/MobSpawner.hpp"

Level::Level(LevelStorage* pStor, const std::string& str, int64_t seed, int storageVersion, Dimension *pDimension)
{
	m_bInstantTicking = false;
	m_bIsOnline = false;
	m_bPostProcessing = false;
	m_skyDarken = 0;
	m_noNeighborUpdate = false;
	m_pDimension = nullptr;
    m_difficulty = 2; // Java has no actual default, it just always pulls from Options. Putting 2 here just so there's no chance of mobs getting despawned accidentally.
	m_bCalculatingInitialSpawn = false;
	m_pChunkSource = nullptr;
	m_pLevelStorage = pStor;
	m_randValue = Random().nextInt();
	m_addend = 1013904223;
	m_bUpdateLights = true;
	m_maxRecurse = 0;
	m_bNoLevelData = false;

	m_random.setSeed(1); // initialize with a seed of 1

	LevelData* pData = m_pLevelStorage->prepareLevel(this);

	m_bNoLevelData = pData == nullptr;

	// @BUG: leaking a Dimension*?
	if (pDimension)
		m_pDimension = pDimension;
	else
		m_pDimension = new Dimension;

	if (!pData)
		m_levelData = LevelData(seed, str, storageVersion);
	else
		m_levelData = *pData;

	m_pDimension->init(this);

	m_pPathFinder = new PathFinder();

	m_pChunkSource = createChunkSource();
	updateSkyBrightness();
}

Level::~Level()
{
	SAFE_DELETE(m_pChunkSource);
	SAFE_DELETE(m_pDimension);
	SAFE_DELETE(m_pPathFinder);
	m_entities.clear();
	m_players.clear();
}
ChunkSource* Level::createChunkSource()
{
#ifndef MOD_USE_FLAT_WORLD
	if (m_pLevelStorage)
	{
		ChunkStorage* pChkStr = m_pLevelStorage->createChunkStorage(m_pDimension);
		ChunkSource* pChkSrc = m_pDimension->createRandomLevelSource();
		return new ChunkCache(this, pChkStr, pChkSrc);
	}
#endif

	LOG_I("No level data, calling dimension->createRandomLevelSource");
	return m_pDimension->createRandomLevelSource();
}

float Level::getTimeOfDay(float f)  const
{
	return m_pDimension->getTimeOfDay(getTime(), f);
}

int Level::getSkyDarken(float f) const
{
	float x = getTimeOfDay(f);
	float y = 1.0f - (2 * Mth::cos(x * M_PI * 2) + 0.5f);

	if (y < 0.0f)
		return 0;
	if (y > 1.0f)
		return 11.0f;

	return int(y * 11.0f);
}

bool Level::updateSkyBrightness()
{
	int skyDarken = getSkyDarken(1.0f);
	if (m_skyDarken != skyDarken)
	{
		m_skyDarken = skyDarken;
		return true;
	}

	return false;
}

BiomeSource* Level::getBiomeSource() const
{
	return m_pDimension->m_pBiomeSource;
}

ChunkSource* Level::getChunkSource() const
{
	return m_pChunkSource;
}

LevelChunk* Level::getChunk(const ChunkPos& pos) const
{
	return m_pChunkSource ? getChunkSource()->getChunk(pos) : nullptr;
}

TileID Level::getTile(const TilePos& pos) const
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (pos.x < C_MIN_X || pos.z < C_MIN_Z || pos.x >= C_MAX_X || pos.z >= C_MAX_Z || pos.y < C_MIN_Y || pos.y >= C_MAX_Y)
		// there's nothing out there!
		return 0;

	LevelChunk* pChunk = getChunk(pos);
	return pChunk->getTile(pos);
}

int Level::getData(const TilePos& pos) const
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (pos.x < C_MIN_X || pos.z < C_MIN_Z || pos.x >= C_MAX_X || pos.z > C_MAX_Z || pos.y < C_MIN_Y || pos.y >= C_MAX_Y)
		// there's nothing out there!
		return 0;

	LevelChunk* pChunk = getChunk(pos);
	return pChunk->getData(pos);
}

int Level::getBrightness(const LightLayer& ll, const TilePos& pos) const
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (pos.x < C_MIN_X || pos.z < C_MIN_Z || pos.x >= C_MAX_X || pos.z > C_MAX_Z || pos.y < C_MIN_Y || pos.y >= C_MAX_Y)
		// there's nothing out there!
		return ll.m_x;

	if (!hasChunk(pos))
		return 0;

	LevelChunk* pChunk = getChunk(pos);
	return pChunk->getBrightness(ll, pos);
}

float Level::getBrightness(const TilePos& pos) const
{
	return m_pDimension->m_brightnessRamp[getRawBrightness(pos)];
}

int Level::getRawBrightness(const TilePos& pos) const
{
	return getRawBrightness(pos, true);
}

int Level::getRawBrightness(const TilePos& pos, bool b) const
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (pos.x < C_MIN_X || pos.z < C_MIN_Z || pos.x >= C_MAX_X || pos.z > C_MAX_Z)
		return 15;

	// this looks like some kind of hack.
	if (b && (getTile(pos) == Tile::stoneSlabHalf->m_ID || getTile(pos) == Tile::farmland->m_ID))
	{
		int b1 = getRawBrightness(pos.above(), false);
		int b2 = getRawBrightness(pos.east(), false);
		int b3 = getRawBrightness(pos.west(), false);
		int b4 = getRawBrightness(pos.south(), false);
		int b5 = getRawBrightness(pos.north(), false);

		if (b2 < b1) b2 = b1;
		if (b3 < b2) b3 = b2;
		if (b4 < b3) b4 = b3;
		if (b5 < b4) b5 = b4;

		return b5;
	}

	if (pos.y < C_MIN_Y)
		return 0;

	if (pos.y >= C_MAX_Y)
	{
		int r = 15 - m_skyDarken;
		if (r < 0)
			r = 0;

		return r;
	}

	LevelChunk* pChunk = getChunk(pos);
	return pChunk->getRawBrightness(pos, m_skyDarken);
}

std::shared_ptr<TileEntity> Level::getTileEntity(const TilePos& pos) const 
{
	LevelChunk* pChunk = getChunk(pos);
	return pChunk ? pChunk->getTileEntity(pos) : nullptr;
}

void Level::setTileEntity(const TilePos& pos, std::shared_ptr<TileEntity> tileEntity) 
{
	LevelChunk* pChunk = getChunk(pos);
	if (pChunk) {
		pChunk->setTileEntity(pos, tileEntity);
	}

}

void Level::removeTileEntity(const TilePos& pos) 
{
	LevelChunk* pChunk = getChunk(pos);
	if (pChunk) {
		pChunk->removeTileEntity(pos);
	}

}

void Level::swap(const TilePos& pos1, const TilePos& pos2)
{
	int tile1 = getTile(pos1);
	int data1 = getData(pos1);
	int tile2 = getTile(pos2);
	int data2 = getData(pos2);

	setTileAndDataNoUpdate(pos1, tile2, data2);
	setTileAndDataNoUpdate(pos2, tile1, data1);

	updateNeighborsAt(pos1, tile2);
	updateNeighborsAt(pos2, tile1);
}

bool Level::isDay() const
{
	return m_skyDarken <= 3;
}

bool Level::isEmptyTile(const TilePos& pos) const
{
	return getTile(pos) == 0;
}

bool Level::isSolidTile(const TilePos& pos) const
{
	Tile* pTile = Tile::tiles[getTile(pos)];
	if (!pTile) return false;

	return pTile->isSolidRender();
}

Material* Level::getMaterial(const TilePos& pos) const
{
	Tile* pTile = Tile::tiles[getTile(pos)];
	if (!pTile) return Material::air;

	return pTile->m_pMaterial;
}

std::shared_ptr<Entity> Level::getEntity(int id) const
{
	// prioritize players first.
	for (std::vector<std::shared_ptr<Player>>::const_iterator it = m_players.begin(); it != m_players.end(); it++)
	{
		auto& pEnt = *it;
		if (pEnt->m_EntityID == id)
			return pEnt;
	}
	for (std::vector<std::shared_ptr<Entity>>::const_iterator it = m_entities.begin(); it != m_entities.end(); it++)
	{
		auto& pEnt = *it;
		if (pEnt->m_EntityID == id)
			return pEnt;
	}

	return nullptr;
}

const EntityVector* Level::getAllEntities() const
{
	return &m_entities;
}

bool Level::hasChunk(const ChunkPos& pos) const
{
	return m_pChunkSource->hasChunk(pos);
}

EntityVector Level::getEntities(std::shared_ptr<Entity> pEntExclude, const AABB& aabb) const
{
	EntityVector entities = EntityVector();

	long lowerXBound = floor((aabb.min.x - 2.0f) / 16);
	long lowerZBound = floor((aabb.min.z - 2.0f) / 16);
	long upperXBound = floor((aabb.max.x + 2.0f) / 16);
	long upperZBound = floor((aabb.max.z + 2.0f) / 16);

	for (long z = lowerZBound; z <= upperZBound; z++)
	{
		for (long x = lowerXBound; x <= upperXBound; x++)
		{
			if (!hasChunk(ChunkPos(x, z))) continue;

			LevelChunk* pChunk = getChunk(ChunkPos(x, z));
			pChunk->getEntities(pEntExclude, aabb, entities);
		}
	}

	return entities;
}

EntityVector Level::getEntitiesOfCategory(EntityCategories::CategoriesMask category, const AABB& aabb) const
{
	EntityVector entities = EntityVector();

	long lowerXBound = floor((aabb.min.x - 2.0f) / 16);
	long lowerZBound = floor((aabb.min.z - 2.0f) / 16);
	long upperXBound = floor((aabb.max.x + 2.0f) / 16);
	long upperZBound = floor((aabb.max.z + 2.0f) / 16);

	for (long z = lowerZBound; z <= upperZBound; z++)
	{
		for (long x = lowerXBound; x <= upperXBound; x++)
		{
			if (!hasChunk(ChunkPos(x, z))) continue;

			LevelChunk* pChunk = getChunk(ChunkPos(x, z));
			pChunk->getEntitiesOfCategory(category, aabb, entities);
		}
	}

	return entities;
}

void Level::setUpdateLights(bool b)
{
	m_bUpdateLights = b;
}

bool Level::updateLights()
{
	// if more than 49 concurrent updateLights() calls?
	if (m_maxRecurse > 49)
		return false;

	m_maxRecurse++;

	if (m_lightUpdates.empty())
	{
		m_maxRecurse--;
		return false;
	}

	for (int i = 499; i; i--)
	{
		LightUpdate lu = *(m_lightUpdates.end() - 1);
		m_lightUpdates.pop_back();

		lu.update(this);

		if (m_lightUpdates.empty())
		{
			m_maxRecurse--;
			return false;
		}
	}

	m_maxRecurse--;
	return true;
}

bool Level::hasChunksAt(const TilePos& min, const TilePos& max) const
{
	if (min.y >= C_MAX_Y || max.y < 0)
		return false;

	ChunkPos cpMin(min), cpMax(max), cp = ChunkPos();
	for (cp.x = cpMin.x; cp.x <= cpMax.x; cp.x++)
	{
		for (cp.z = cpMin.z; cp.z <= cpMax.z; cp.z++)
		{
			if (!hasChunk(cp))
				return false;
		}
	}

	return true;
}

bool Level::hasChunksAt(const TilePos& pos, int rad) const
{
	return hasChunksAt(pos - rad, pos + rad);
}

void Level::setBrightness(const LightLayer& ll, const TilePos& pos, int brightness)
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (pos.x < C_MIN_X || pos.z < C_MIN_Z || pos.x >= C_MAX_X || pos.z > C_MAX_Z || pos.y < C_MIN_Y || pos.y >= C_MAX_Y)
		// there's nothing out there!
		return;

	LevelChunk* pChunk = getChunk(pos);
	pChunk->setBrightness(ll, pos, brightness);

	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->tileBrightnessChanged(pos);
	}
}

void Level::setTime(int64_t time)
{
	_setTime(time);

	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->timeChanged(time);
	}
}

int Level::getDirectSignal(const TilePos& pos, Facing::Name face) const
{
	TileID tile = getTile(pos);
	if (!tile) return 0;

	return Tile::tiles[tile]->getDirectSignal(this, pos, face);
}

int Level::getSignal(const TilePos& pos, Facing::Name face) const
{
	if (isSolidTile(pos))
		return hasDirectSignal(pos);

	TileID tile = getTile(pos);
	if (!tile) return 0;

	return Tile::tiles[tile]->getSignal(this, pos, face);
}

bool Level::hasDirectSignal(const TilePos& pos) const
{
	if (getDirectSignal(pos.below(), Facing::DOWN)) return true;
	if (getDirectSignal(pos.above(), Facing::UP)) return true;
	if (getDirectSignal(pos.north(), Facing::NORTH)) return true;
	if (getDirectSignal(pos.south(), Facing::SOUTH)) return true;
	if (getDirectSignal(pos.west(), Facing::WEST)) return true;
	if (getDirectSignal(pos.east(), Facing::EAST)) return true;
	return false;
}

bool Level::hasNeighborSignal(const TilePos& pos) const
{
	if (getSignal(pos.below(), Facing::DOWN)) return true;
	if (getSignal(pos.above(), Facing::UP)) return true;
	if (getSignal(pos.north(), Facing::NORTH)) return true;
	if (getSignal(pos.south(), Facing::SOUTH)) return true;
	if (getSignal(pos.west(), Facing::WEST)) return true;
	if (getSignal(pos.east(), Facing::EAST)) return true;
	return false;
}

bool Level::hasChunkAt(const TilePos& pos) const
{
	return hasChunk(pos);
}

LevelChunk* Level::getChunkAt(const TilePos& pos) const
{
	return getChunk(pos);
}

void Level::updateLight(const LightLayer& ll, const TilePos& tilePos1, const TilePos& tilePos2, bool unimportant)
{
	static int nUpdateLevels;

	if ((m_pDimension->m_bHasCeiling && &ll == &LightLayer::Sky) || !m_bUpdateLights)
		return;

	nUpdateLevels++;
	if (nUpdateLevels == 50)
	{
		nUpdateLevels--;
		return;
	}

	TilePos idkbro((tilePos2.x + tilePos1.x) / 2, (tilePos2.y + tilePos1.y) / 2, (tilePos2.z + tilePos1.z) / 2);

	if (!hasChunkAt(idkbro) || getChunkAt(idkbro)->isEmpty())
	{
		nUpdateLevels--;
		return;
	}

	size_t size = m_lightUpdates.size();
	if (unimportant)
	{
		size_t count = 5;
		if (count > size)
			count = size;

		for (size_t i = 0; i < count; i++)
		{
			LightUpdate& update = m_lightUpdates[size - i - 1];
			if (update.m_lightLayer == &ll && update.expandToContain(tilePos1, tilePos2))
			{
				nUpdateLevels--;
				return;
			}
		}
	}

	m_lightUpdates.push_back(LightUpdate(ll, tilePos1, tilePos2));

	if (m_lightUpdates.size() > 1000000)
	{
		LOG_I("More than %d updates, aborting lighting updates", 1000000);
		m_lightUpdates.clear();
	}

	nUpdateLevels--;
}

void Level::updateLightIfOtherThan(const LightLayer& ll, const TilePos& tilePos, int bright)
{
	if (m_pDimension->m_bHasCeiling && &ll == &LightLayer::Sky)
		return;

	if (!hasChunkAt(tilePos))
		return;

	if (&ll == &LightLayer::Sky)
	{
		if (isSkyLit(tilePos))
			bright = 15;
	}
	else if (&ll == &LightLayer::Block)
	{
		TileID tile = getTile(tilePos);
		if (bright < Tile::lightEmission[tile])
			bright = Tile::lightEmission[tile];
	}

	int oldbr = getBrightness(ll, tilePos);
	if (bright != oldbr)
	{
		updateLight(ll, tilePos, tilePos);
	}
}

bool Level::isSkyLit(const TilePos& pos) const
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (pos.x < C_MIN_X || pos.z < C_MIN_Z || pos.x >= C_MAX_X || pos.z > C_MAX_Z || pos.y < C_MIN_Y)
		// there's nothing out there!
		return false;

	if (pos.y >= C_MAX_Y)
		return true;

	if (!hasChunk(pos))
		return false;

	return getChunk(pos)->isSkyLit(pos);
}

bool Level::setTileAndDataNoUpdate(const TilePos& pos, TileID tile, int data)
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (pos.x < C_MIN_X || pos.z < C_MIN_Z || pos.x >= C_MAX_X || pos.z > C_MAX_Z || pos.y < C_MIN_Y || pos.y >= C_MAX_Y)
		// there's nothing out there!
		return false;

	if (!hasChunk(pos))
		return false;

	return getChunk(pos)->setTileAndData(pos, tile, data);
}

int Level::getHeightmap(const TilePos& pos)
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (pos.x < C_MIN_X || pos.z < C_MIN_Z || pos.x >= C_MAX_X || pos.z > C_MAX_Z)
		// there's nothing out there!
		return 0;

	if (!hasChunk(pos))
		return 0;

	return getChunkAt(pos)->getHeightmap(pos);
}

void Level::lightColumnChanged(int x, int z, int y1, int y2)
{
	if (y1 > y2)
	{
		int tmp = y1;
		y1 = y2;
		y2 = tmp;
	}

	setTilesDirty(TilePos(x, y1, z), TilePos(x, y2, z));
}

bool Level::setDataNoUpdate(const TilePos& pos, int data)
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (pos.x < C_MIN_X || pos.z < C_MIN_Z || pos.x >= C_MAX_X || pos.z > C_MAX_Z || pos.y < C_MIN_Y || pos.y >= C_MAX_Y)
		// there's nothing out there!
		return false;

	if (!hasChunk(pos))
		return false;

	LevelChunk* pChk = getChunk(pos);
	if (pChk->getData(pos) == data)
		return false; // no update

	pChk->setData(pos, data);
	return true;
}

bool Level::setTileNoUpdate(const TilePos& pos, TileID tile)
{
	//@BUG: checking x >= C_MAX_X, but not z >= C_MAX_Z.
	if (pos.x < C_MIN_X || pos.z < C_MIN_Z || pos.x >= C_MAX_X || pos.z > C_MAX_Z || pos.y < C_MIN_Y || pos.y >= C_MAX_Y)
		// there's nothing out there!
		return false;

	if (!hasChunk(pos))
		return false;

	return getChunk(pos)->setTile(pos, tile);
}

void Level::sendTileUpdated(const TilePos& pos)
{
	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->tileChanged(pos);
	}
}

void Level::neighborChanged(const TilePos& pos, TileID tile)
{
	if (m_noNeighborUpdate || m_bIsOnline) return;

	Tile* pTile = Tile::tiles[getTile(pos)];
	if (pTile)
		pTile->neighborChanged(this, pos, tile);
}

void Level::updateNeighborsAt(const TilePos& pos, TileID tile)
{
	neighborChanged(pos.west(), tile);
	neighborChanged(pos.east(), tile);
	neighborChanged(pos.below(), tile);
	neighborChanged(pos.above(), tile);
	neighborChanged(pos.north(), tile);
	neighborChanged(pos.south(), tile);
}

void Level::tileUpdated(const TilePos& pos, TileID tile)
{
	sendTileUpdated(pos);
	updateNeighborsAt(pos, tile);
}

void Level::tileEntityChanged(const TilePos& pos, std::shared_ptr<TileEntity> tileEntity)
{
	if (hasChunkAt(pos)) {
		getChunkAt(pos)->markUnsaved();
	}

	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->tileEntityChanged(pos, tileEntity);
	}
}


bool Level::setTileAndData(const TilePos& pos, TileID tile, int data)
{
	if (setTileAndDataNoUpdate(pos, tile, data))
	{
		tileUpdated(pos, tile);
		return true;
	}
	return false;
}

bool Level::setData(const TilePos& pos, int data)
{
	if (setDataNoUpdate(pos, data))
	{
		tileUpdated(pos, getTile(pos));
		return true;
	}
	return false;
}

bool Level::setTile(const TilePos& pos, TileID tile)
{
	if (setTileNoUpdate(pos, tile))
	{
		tileUpdated(pos, tile);
		return true;
	}
	return false;
}

void Level::setTilesDirty(const TilePos& min, const TilePos& max)
{
	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->setTilesDirty(min, max);
	}
}

void Level::entityAdded(Entity* pEnt)
{
	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->entityAdded(pEnt);
	}
}

void Level::entityRemoved(Entity* pEnt)
{
	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->entityRemoved(pEnt);
	}
}

AABBVector* Level::getCubes(const Entity* pEntUnused, const AABB& aabb)
{
	m_aabbs.clear();

	TilePos lower(aabb.min);
	TilePos upper(aabb.max.x + 1, aabb.max.y + 1, aabb.max.z + 1);
	TilePos tp;
	for (tp.x = lower.x; tp.x <= upper.x; tp.x++)
	{
		for (tp.z = lower.z; tp.z <= upper.z; tp.z++)
		{
			if (!hasChunkAt(tp)) continue;

			for (tp.y = lower.y - 1; tp.y <= upper.y; tp.y++)
			{
				Tile* pTile = Tile::tiles[getTile(tp)];
				if (pTile)
					pTile->addAABBs(this, tp, &aabb, m_aabbs);
			}
		}
	}

	return &m_aabbs;
}

std::vector<LightUpdate>* Level::getLightsToUpdate()
{
	return &m_lightUpdates;
}

std::shared_ptr<Player> Level::getNearestPlayer(const Entity* entity, float f) const
{
	return getNearestPlayer(entity->m_pos, f);
}

std::shared_ptr<Player> Level::getNearestPlayer(const Vec3& pos, float maxDist) const
{
	float dist = -1.0f;
	std::shared_ptr<Player> pPlayer = nullptr;

	for (auto it = m_players.begin(); it != m_players.end(); it++)
	{
		std::shared_ptr<Player> player = *it;
		float ldist = player->distanceToSqr(pos);
		if ((maxDist < 0.0f || ldist < maxDist * maxDist) && (dist == -1.0f || dist > ldist))
		{
			pPlayer = player;
			dist = ldist;
		}
	}

	return pPlayer;
}

bool Level::containsFireTile(const AABB& aabb)
{
	TilePos min(aabb.min),
		    max(aabb.max + 1);

	if (!hasChunksAt(min, max))
		return false;
	
	TilePos pos(min);
	for (pos.x = min.x; pos.x < max.x; pos.x++)
		for (pos.y = min.y; pos.y < max.y; pos.y++)
			for (pos.z = min.z; pos.z < max.z; pos.z++)
			{
				TileID tileID = getTile(pos);
				
				if (tileID == Tile::fire->m_ID || tileID == Tile::lava->m_ID || tileID == Tile::calmLava->m_ID)
					return true;
			}

	return false;
}

bool Level::containsAnyLiquid(const AABB& aabb)
{
	TilePos min(aabb.min),
		    max(aabb.max + 1);

	if (!hasChunksAt(min, max))
		return false;
	
	TilePos pos(min);
	for (pos.x = min.x; pos.x < max.x; pos.x++)
		for (pos.y = min.y; pos.y < max.y; pos.y++)
			for (pos.z = min.z; pos.z < max.z; pos.z++)
			{
				TileID tileID = getTile(pos);
				
				if (Tile::tiles[tileID])
				{
					if (Tile::tiles[tileID]->m_pMaterial->isLiquid())
						return true;
				}
			}

	return false;
}

bool Level::containsLiquid(const AABB& aabb, const Material* pMtl)
{
	TilePos min(aabb.min),
		    max(aabb.max + 1);

	if (!hasChunksAt(min, max))
		return false;
	
	TilePos pos(min);
	for (pos.x = min.x; pos.x < max.x; pos.x++)
		for (pos.y = min.y; pos.y < max.y; pos.y++)
			for (pos.z = min.z; pos.z < max.z; pos.z++)
			{
				TileID tileID = getTile(pos);

				if (!Tile::tiles[tileID] || Tile::tiles[tileID]->m_pMaterial != pMtl)
					continue;

				int data = getData(pos);
				
				float height;
				if (data <= 7)
					height = (float(pos.y) + 1.0f) + float(data) / 8.0f;
				else
					height = float(pos.y + 1);

				if (aabb.min.y <= height)
					return true;
			}

	return false;
}

bool Level::containsMaterial(const AABB& aabb, const Material* pMtl)
{
	TilePos min(aabb.min),
		    max(aabb.max + 1);

	if (!hasChunksAt(min, max))
		return false;

	TilePos pos(min);
	for (pos.x = min.x; pos.x < max.x; pos.x++)
		for (pos.y = min.y; pos.y < max.y; pos.y++)
			for (pos.z = min.z; pos.z < max.z; pos.z++)
			{
				TileID tileID = getTile(pos);
				
				if (Tile::tiles[tileID] && Tile::tiles[tileID]->m_pMaterial == pMtl)
					return true;
			}

	return false;
}

bool Level::checkAndHandleWater(const AABB& aabb, const Material* pMtl, Entity* pEnt)
{
	TilePos min(aabb.min),
		    max(aabb.max + 1);

	if (!hasChunksAt(min, max))
		return false;

	Vec3 v;
	
	bool bInWater = false;
	TilePos pos(aabb.min);
	for (pos.x = min.x; pos.x < max.x; pos.x++)
	{
		for (pos.y = min.y; pos.y < max.y; pos.y++)
		{
			for (pos.z = min.z; pos.z < max.z; pos.z++)
			{
				Tile* pTile = Tile::tiles[getTile(pos)];
				if (!pTile || pTile->m_pMaterial != pMtl)
					continue;

				if (max.y >= (pos.y + 1 - LiquidTile::getWaterVolume(getData(pos))))
				{
					pTile->handleEntityInside(this, pos, pEnt, v);
					bInWater = true;
				}
			}
		}
	}

	if (v.length() > 0.0)
	{
		Vec3 norm = v.normalize() * 0.014;
		pEnt->m_vel += norm;
	}

	return bInWater;
}

TilePos Level::getSharedSpawnPos() const
{
	return m_levelData.getSpawn();
}

TileID Level::getTopTile(const TilePos& pos) const
{
	int y;
	for (y = C_MAX_Y / 2 - 1; !isEmptyTile(TilePos(pos.x, y + 1, pos.z)); y++);
	return getTile(TilePos(pos.x, y, pos.z));
}

int Level::getTopTileY(const TilePos& pos) const
{
	int y;
	for (y = C_MAX_Y / 2 - 1; !isEmptyTile(TilePos(pos.x, y + 1, pos.z)); y++);
	return y;
}

int Level::getTopSolidBlock(const TilePos& tilePos) const
{
	LevelChunk* pChunk = getChunkAt(tilePos);
	ChunkTilePos pos(tilePos);

	for (pos.y = 127; pos.y > 0; --pos.y) {
		int var5 = pChunk->getTile(pos);
		Material* var6 = var5 == 0 ? Material::air : Tile::tiles[var5]->m_pMaterial;
		if (var6->isSolid() || var6->isLiquid()) {
			return pos.y + 1;
		}
	}

	return -1;
}

int Level::countWithCategory(EntityCategories::CategoriesMask category)
{
	int count = 0;
	for (auto& entity : m_entities)
	{
		if (entity->getType().getCategory().contains(category)) count++;
	}
	return count;
}

void Level::validateSpawn()
{
	m_levelData.setYSpawn(C_MAX_Y / 2);

	TilePos spawn(0, 64, 0);

	while (!getTopTile(spawn)) {
		spawn.x += m_random.nextInt(8) - m_random.nextInt(8);
		spawn.z += m_random.nextInt(8) - m_random.nextInt(8);
	}

	m_levelData.setSpawn(spawn);
}

void Level::removeAllPendingEntityRemovals()
{
	Util::removeAll(m_entities, m_pendingEntityRemovals);

	for (EntityVector::iterator it = m_pendingEntityRemovals.begin(); it != m_pendingEntityRemovals.end(); it++)
	{
		std::shared_ptr<Entity> ent = *it;
		ent->removed();

		LevelChunk* chunk = getChunk(ent->m_chunkPos);
		if (chunk) chunk->removeEntity(ent);

		entityRemoved(ent.get());

		//@BUG: MEMORY LEAK -- probably leaking entities here?
	}

	m_pendingEntityRemovals.clear();
}

void Level::removeEntities(const EntityVector& vec)
{
	m_pendingEntityRemovals.insert(m_pendingEntityRemovals.end(), vec.begin(), vec.end());
}

bool Level::removeEntity(std::shared_ptr<Entity> pEnt)
{
	pEnt->remove();

	if (pEnt->isPlayer())
		return Util::remove(m_players, std::dynamic_pointer_cast<Player>(pEnt));

	return false;
}

bool Level::addEntity(std::shared_ptr<Entity> pEnt)
{
	auto pOldEnt = getEntity(pEnt->hashCode());
	if (pOldEnt)
	{
		LOG_W("Entity %d already exists.", pEnt->hashCode());
	}

	if (pEnt->isPlayer())
	{
		m_players.push_back(std::dynamic_pointer_cast<Player>(pEnt));
	}
	getChunk(pEnt->m_pos)->addEntity(pEnt);
	m_entities.push_back(pEnt);
	entityAdded(pEnt.get());

	return true;
}

void Level::loadPlayer(std::shared_ptr<Player> player)
{
	if (!player) return;

	m_levelData.setLoadedPlayerTo(player.get());

	addEntity(player);
}

void Level::prepare()
{
	while (m_pChunkSource->tick());
}

void Level::saveLevelData()
{
	m_pLevelStorage->saveLevelData(&m_levelData, m_players);
}

void Level::savePlayerData()
{
	m_pLevelStorage->savePlayerData(&m_levelData, m_players);
}

void Level::saveAllChunks()
{
	m_pChunkSource->saveAll();
}

void Level::saveUnsavedChunks(bool limited)
{
	m_pChunkSource->saveUnsaved(limited);
}

void Level::setInitialSpawn()
{
	m_bCalculatingInitialSpawn = true;

	int spawnX = 0, spawnZ;

	for (spawnZ = 0; !m_pDimension->isValidSpawn(TilePos(spawnX, 64, spawnZ)); spawnZ += m_random.nextInt(64) - m_random.nextInt(64)) {
		spawnX += m_random.nextInt(64) - m_random.nextInt(64);
	}

	m_levelData.setSpawn(TilePos(spawnX, 64, spawnZ));

	m_bCalculatingInitialSpawn = false;
}

bool Level::canSeeSky(const TilePos& pos) const
{
	LevelChunk* pChunk = getChunk(pos);

	//@BUG: no nullptr check
#ifndef ORIGINAL_CODE
	if (!pChunk)
		return true;
#endif

	return pChunk->isSkyLit(pos);
}

Vec3f Level::getSkyColor(Entity* pEnt, float f) const
{
	float var4 = Mth::cos(getSunAngle(f)) * 2.0F + 0.5F;
	if (var4 < 0.0F) {
		var4 = 0.0F;
	}

	if (var4 > 1.0F) {
		var4 = 1.0F;
	}

	int var5 = Mth::floor(pEnt->m_pos.x);
	int var6 = Mth::floor(pEnt->m_pos.z);
	float var7 = getBiomeSource()->getTemperature(var5, var6);
	int var8 = getBiomeSource()->getBiome(TilePos(var5, 0, var6))->getSkyColor(var7);
	Vec3f sky((var8 >> 16 & 255) / 255.0F, (var8 >> 8 & 255) / 255.0F, (var8 & 255) / 255.0F);
	sky.x *= var4;
	sky.y *= var4;
	sky.z *= var4;
	return sky;
}

Vec3f Level::getFogColor(float f) const
{
	return m_pDimension->getFogColor(getTimeOfDay(f), f);
}

Vec3 Level::getCloudColor(float f) const
{
	Vec3 result;

	float fTODCosAng = Mth::cos(getSunAngle(f));

	float mult = 2 * fTODCosAng + 0.5f;
	if (mult < 0.0f)
		mult = 0.0f;
	if (mult > 1.0f)
		mult = 1.0f;

	result.x = mult * 0.9f + 0.1f;
	result.y = result.x;
	result.z = mult * 0.85f + 0.15f;

	return result;
}

bool Level::canChunkExist(const ChunkPos& pos) {

    for (const auto& player : m_players) {

        int dx = Mth::abs(pos.x - player->m_chunkPos.x);
        int dz = Mth::abs(pos.z - player->m_chunkPos.z);

        if (dx <= viewDistance && dz <= viewDistance)
            return true;
    }

    return false;
}

bool Level::isUnobstructed(AABB* aabb) const
{
	EntityVector entities = getEntities(nullptr, *aabb);
	if (entities.size() <= 0)
		return true;

	for (std::vector<std::shared_ptr<Entity>>::iterator it = entities.begin(); it != entities.end(); it++)
	{
		std::shared_ptr<Entity> pEnt = *it;
		if (!pEnt->m_bRemoved && pEnt->m_bBlocksBuilding)
            return false;
	}

	return true;
}

bool Level::mayInteract(Player* player, const TilePos& pos) const
{
	return true;
}

bool Level::mayPlace(TileID tile, const TilePos& pos, bool b, Facing::Name face) const
{
	TileID oldTile = getTile(pos);

	Tile* pTile = Tile::tiles[tile], * pOldTile = Tile::tiles[oldTile];

	if (pTile == nullptr)
		return false;

	AABB* aabb = pTile->getAABB(this, pos);

	if (!b && aabb && !isUnobstructed(aabb))
		return false;

	if (pOldTile == Tile::water     ||
		pOldTile == Tile::calmWater ||
		pOldTile == Tile::lava      ||
		pOldTile == Tile::calmLava  ||
		pOldTile == Tile::fire      ||
		pOldTile == Tile::topSnow)
		return true;

	if (pOldTile || tile <= 0)
		return false;

	return pTile->mayPlace(this, pos, face);
}

void Level::removeListener(LevelListener* listener)
{
	std::vector<LevelListener*>::iterator iter = std::find(m_levelListeners.begin(), m_levelListeners.end(), listener);
	if (iter != m_levelListeners.end())
		m_levelListeners.erase(iter);
}

void Level::addListener(LevelListener* listener)
{
	m_levelListeners.push_back(listener);
}

void Level::tickPendingTicks(bool b)
{
	int size = Mth::Min(m_pendingTicks.size(), 65536);

	for (int i = 0; i < size; i++)
	{
		const TickNextTickData& t = *m_pendingTicks.begin();
		if (!b && t.m_delay > m_levelData.getTime())
			break;

		if (hasChunksAt(t.field_4 - 8, t.field_4 + 8))
		{
			TileID tile = getTile(t.field_4);
			if (tile == t.field_10 && tile > 0)
				Tile::tiles[tile]->tick(this, t.field_4, &m_random);
		}

		m_pendingTicks.erase(m_pendingTicks.begin());
	}
}

void Level::tickTiles()
{
	m_chunksToUpdate.clear();

	for (std::vector<std::shared_ptr<Player>>::iterator it = m_players.begin(); it != m_players.end(); it++)
	{
		auto& player = *it;

		int chkX = Mth::floor(player->m_pos.x / 16.0f);
		int chkZ = Mth::floor(player->m_pos.z / 16.0f);

		for (int x = -C_TICK_DISTANCE_CHKS; x <= C_TICK_DISTANCE_CHKS; x++)
		{
			for (int z = -C_TICK_DISTANCE_CHKS; z <= C_TICK_DISTANCE_CHKS; z++)
			{
				m_chunksToUpdate.insert(ChunkPos(chkX + x, chkZ + z));
			}
		}
	}

	for (std::set<ChunkPos>::iterator it = m_chunksToUpdate.begin(); it != m_chunksToUpdate.end(); it++)
	{
		ChunkPos pos = *it;
		LevelChunk* pChunk = getChunk(pos);

		for (int i = 0; i < 80; i++)
		{
			m_randValue = m_randValue * 3 + m_addend;
			int rand = m_randValue >> 2;

			TilePos tilePos(
				(rand)       & 15,
				(rand >> 16) & 127,
				(rand >> 8)  & 15);

			TileID tile = pChunk->getTile(tilePos);
			if (Tile::shouldTick[tile])
				Tile::tiles[tile]->tick(this, tilePos + pos, &m_random);
		}
	}
}

void Level::tick(std::shared_ptr<Entity> pEnt, bool b) const
{
	TilePos tilePos(pEnt->m_pos);

	if (b)
	{
		if (!hasChunksAt(TilePos(tilePos.x - 32, 0, tilePos.z - 32), TilePos(tilePos.x + 32, 128, tilePos.z + 32)))
		{
			return;
		}

		pEnt->m_posPrev = pEnt->m_pos;
		pEnt->m_rotPrev = pEnt->m_rot;

		if (pEnt->m_bInAChunk)
			pEnt->tick();
	}
	else
	{
		pEnt->m_posPrev = pEnt->m_pos;
		pEnt->m_rotPrev = pEnt->m_rot;
	}

	ChunkPos cp(pEnt->m_pos);

	if (!pEnt->m_bInAChunk || cp != pEnt->m_chunkPos)
	{
		// move the entity to the new chunk
		if (pEnt->m_bInAChunk && hasChunk(pEnt->m_chunkPos))
		{
			getChunk(pEnt->m_chunkPos)->removeEntity(pEnt);
		}

		if (hasChunk(cp))
		{
			pEnt->m_bInAChunk = true;
			getChunk(cp)->addEntity(pEnt);
		}
		else
		{
			pEnt->m_bInAChunk = false;
		}
	}

}

int TICKSPERSECOND = 0;
int LASTTICKED = 0;

void Level::tick()
{
	MobSpawner::tick(this, m_difficulty > 0, true);
	m_pChunkSource->tick();

#ifdef ENH_RUN_DAY_NIGHT_CYCLE
	bool skyColorChanged = updateSkyBrightness();

	int64_t time = getTime() + 1;
	_setTime(time); // Bypasses the normally-required update to LevelListeners

	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;

		if (skyColorChanged)
			pListener->skyColorChanged();

		pListener->timeChanged(time);
	}
#endif

	tickPendingTicks(false);
	tickTiles();
}

void Level::tickEntities()
{
	// inlined in the original
	removeAllPendingEntityRemovals();

	for (int i = 0; i<int(m_entities.size()); i++)
	{
		std::shared_ptr<Entity>& pEnt = m_entities[i];

		if (!pEnt->m_bRemoved)
		{
			tick(pEnt);
		}
		else
		{
			if (pEnt->m_bInAChunk && hasChunk(pEnt->m_chunkPos))
				getChunk(pEnt->m_chunkPos)->removeEntity(pEnt);

			m_entities.erase(m_entities.begin() + i);
			i--;

			entityRemoved(pEnt.get());
		}
	}

	for (auto& tileEntity : m_tileEntityList)
	{
		tileEntity->tick();
	}
}

HitResult Level::clip(Vec3 v1, Vec3 v2, bool flag) const
{
	TilePos tp1(v1), tp2(v2);
	int counter = 200;
	while (counter-- >= 0)
	{
		if (tp1 == tp2)
			break;

		float xd = 999.0f;
		float yd = 999.0f;
		float zd = 999.0f;
		if (tp2.x > tp1.x) xd = tp1.x + 1.0f;
		if (tp2.x < tp1.x) xd = tp1.x + 0.0f;
		if (tp2.y > tp1.y) yd = tp1.y + 1.0f;
		if (tp2.y < tp1.y) yd = tp1.y + 0.0f;
		if (tp2.z > tp1.z) zd = tp1.z + 1.0f;
		if (tp2.z < tp1.z) zd = tp1.z + 0.0f;
		float xe = 999.0f;
		float ye = 999.0f;
		float ze = 999.0f;
		float xl = v2.x - v1.x;
		float yl = v2.y - v1.y;
		float zl = v2.z - v1.z;
		if (xd != 999.0f) xe = (xd - v1.x) / xl;
		if (yd != 999.0f) ye = (yd - v1.y) / yl;
		if (zd != 999.0f) ze = (zd - v1.z) / zl;
		int hitSide = 0;
		if (xe >= ye || xe >= ze)
		{
			if (ye >= ze)
			{
				hitSide = tp2.z <= tp1.z ? Facing::SOUTH : Facing::NORTH;
				v1.x = v1.x + (xl * ze);
				v1.y = v1.y + (yl * ze);
				v1.z = zd;
			}
			else
			{
				hitSide = (tp2.y <= tp1.y) ? Facing::UP : Facing::DOWN;
				v1.x = v1.x + (xl * ye);
				v1.y = yd;
				v1.z = v1.z + (zl * ye);
			}
		}
		else
		{
			hitSide = tp2.x <= tp1.x ? Facing::EAST : Facing::WEST;
			v1.x = xd;
			v1.y = v1.y + (yl * xe);
			v1.z = v1.z + (zl * xe);
		}

		Vec3 hitVec(v1);

		// Correct the hit positions for each vector
		hitVec.x = Mth::floor(v1.x);
		tp1.x = (int)hitVec.x;
		if (hitSide == Facing::EAST)
		{
			tp1.x--;
			hitVec.x += 1.0;
		}


		hitVec.y = Mth::floor(v1.y);
		tp1.y = (int)hitVec.y;
		if (hitSide == Facing::UP)
		{
			tp1.y--;
			hitVec.y += 1.0;
		}


		hitVec.z = Mth::floor(v1.z);
		tp1.z = (int)hitVec.z;
		if (hitSide == Facing::SOUTH)
		{
			tp1.z--;
			hitVec.z += 1.0;
		}

		TileID tile = getTile(tp1);
		int    data = getData(tp1);
		Tile* pTile = Tile::tiles[tile];

		if (tile > 0 && pTile->mayPick(data, flag))
		{
			HitResult hr = pTile->clip(this, tp1, v1, v2);
			if (hr.isHit())
				return hr;
		}
	}

	return HitResult();
}

HitResult Level::clip(const Vec3& a, const Vec3& b) const
{
	return clip(a, b, false);
}

void Level::addToTickNextTick(const TilePos& tilePos, int d, int delay)
{
	TickNextTickData tntd(tilePos, d);
	if (m_bInstantTicking)
	{
		// @NOTE: Don't know why this check wasn't just placed at the beginning.
		if (!hasChunksAt(tilePos, 8))
			return;

		TileID tile = getTile(tntd.field_4);
		if (tile > 0 && tile == tntd.field_10)
			Tile::tiles[tntd.field_10]->tick(this, tntd.field_4, &m_random);
	}
	else
	{
		if (!hasChunksAt(tilePos, 8))
			return;

		if (d > 0)
			tntd.setDelay(delay + getTime());

		m_pendingTicks.insert(tntd);
	}
}

void Level::takePicture(std::shared_ptr<TripodCamera> pCamera, Entity* pOwner)
{
	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->takePicture(pCamera, pOwner);
	}
}

void Level::addParticle(const std::string& name, const Vec3& pos, const Vec3& dir)
{
	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->addParticle(name, pos, dir);
	}
}

void Level::playSound(Entity* entity, const std::string& name, float volume, float pitch)
{
	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->playSound(name, Vec3(entity->m_pos.x, entity->m_pos.y - entity->m_heightOffset, entity->m_pos.z), volume, pitch);
	}
}

void Level::playSound(const Vec3& pos, const std::string& name, float a, float b)
{
	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->playSound(name, pos, a, b);
	}
}

void Level::animateTick(const TilePos& pos)
{
	Random random;

	for (int i = 0; i < 1000; i++)
	{
		TilePos aPos(pos.x + m_random.nextInt(16) - m_random.nextInt(16),
					 pos.y + m_random.nextInt(16) - m_random.nextInt(16),
					 pos.z + m_random.nextInt(16) - m_random.nextInt(16));
		TileID tile = getTile(aPos);
		if (tile > 0)
			Tile::tiles[tile]->animateTick(this, aPos, &random);
	}
}

float Level::getSeenPercent(Vec3 vec, AABB aabb) const
{
	int raysTotal = 0, raysSeen = 0;

	float aabbSizeX = aabb.max.x - aabb.min.x;
	float aabbSizeY = aabb.max.y - aabb.min.y;
	float aabbSizeZ = aabb.max.z - aabb.min.z;

	// This shoots a bunch of rays from a point and checks if the rays hit something. Stuupiiiddd
	for (float xi = 0.0f; xi <= 1.0f; xi += 1.0f / (1.0f + 2 * aabbSizeX))
	{
		for (float yi = 0.0f; yi <= 1.0f; yi += 1.0f / (1.0f + 2 * aabbSizeY))
		{
			for (float zi = 0.0f; zi <= 1.0f; zi += 1.0f / (1.0f + 2 * aabbSizeZ))
			{
				Vec3 xvec;
				xvec.x = aabb.min.x + xi * aabbSizeX;
				xvec.y = aabb.min.y + yi * aabbSizeY;
				xvec.z = aabb.min.z + zi * aabbSizeZ;

				HitResult hr = clip(xvec, vec);
				if (hr.m_hitType == HitResult::NONE)
					raysSeen++;

				raysTotal++;
			}
		}
	}

	return float (raysSeen) / float (raysTotal);
}

void Level::explode(Entity* entity, const Vec3& pos, float power)
{
	explode(entity, pos, power, false);
}

void Level::explode(Entity* entity, const Vec3& pos, float power, bool bIsFiery)
{
	Explosion expl(this, entity, pos, power);
	expl.setFiery(bIsFiery);
	expl.explode();
	expl.addParticles();
}

void Level::addEntities(const std::vector<std::shared_ptr<Entity>>& entities)
{
	m_entities.insert(m_entities.end(), entities.begin(), entities.end());

	for (std::vector<std::shared_ptr<Entity>>::iterator it = m_entities.begin(); it != m_entities.end(); it++)
	{
		entityAdded((*it).get());
	}
}

// @UNUSED
void Level::ensureAdded(std::shared_ptr<Entity> entity)
{
	ChunkPos chunkPos(Mth::floor(entity->m_pos.x / 16.0f),
		              Mth::floor(entity->m_pos.z / 16.0f));

	// force the chunk to be resolved
	ChunkPos cp;
	for (cp.x = chunkPos.x - 2; cp.x <= chunkPos.x + 2; cp.x++)
		for (cp.z = chunkPos.z - 2; cp.z <= chunkPos.z + 2; cp.z++)
			getChunk(cp);

	EntityVector::iterator result = std::find(m_entities.begin(), m_entities.end(), entity);
	if (result == m_entities.end())
		m_entities.push_back(entity);
}

bool Level::extinguishFire(Player* player, const TilePos& pos, Facing::Name face)
{
	TilePos p(pos.relative(face));

	if (getTile(p) == Tile::fire->m_ID)
		return setTile(p, TILE_AIR);

	return false;
}

int Level::findPath(Path* path, Entity* ent, Entity* target, float f) const
{
	TilePos tp(ent->m_pos);
	Region reg(this, tp - int(f + 16), tp + int(f + 16));

	m_pPathFinder->setLevel(&reg);
	return m_pPathFinder->findPath(*path, ent, target, f);

	// wtf?
	// return 1;
}

int Level::findPath(Path* path, Entity* ent, const TilePos& pos, float f) const
{
	TilePos tp(ent->m_pos);
	Region reg(this, tp - int(f + 8), tp + int(f + 8));
	
	m_pPathFinder->setLevel(&reg);
	return m_pPathFinder->findPath(*path, ent, pos, f);

	// wtf?
	// return 1;
}

int Level::getLightDepth(const TilePos& pos) const
{
	return getChunk(pos)->getHeightmap(pos);
}

float Level::getStarBrightness(float f) const
{
	float ca = Mth::cos(getSunAngle(f));
	float cb = 1.0f - (0.75f + 2 * ca);

	if (cb < 0.0f)
		cb = 0.0f;
	if (cb > 1.0f)
		cb = 1.0f;

	return cb * cb * 0.5f;
}

float Level::getSunAngle(float f) const
{
	return (float(M_PI) * getTimeOfDay(f)) * 2;
}
