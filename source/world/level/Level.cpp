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
#include "EntityTracker.hpp"
#include "world/tile/LiquidTile.hpp"
#include "world/entity/MobSpawner.hpp"
#include "world/entity/LightningBolt.hpp"
#include "network/RakNetInstance.hpp"


void Level::init(Dimension* pDimension)
{
	if (m_pMinecraftServer)
		m_bIsNew = m_pMinecraftServer->m_bIsNew;

	if (pDimension)
		m_pDimension = pDimension;
	else
		m_pDimension = Dimension::getNew(getLevelData().m_LocalPlayerData->getInt("Dimension"));

	auto& limit = getLevelData().getLimit(0);
	auto& netherLimit = getLevelData().getLimit(-1);

	if (limit != DimensionLimit::ZERO && netherLimit != DimensionLimit::ZERO)
	{
		m_netherTravelRatio = Mth::min(8, int(std::ceil((limit.m_maxPos.x - limit.m_minPos.x) / float(netherLimit.m_maxPos.x - netherLimit.m_minPos.x))));
	}
	else
		m_netherTravelRatio = 8;

	m_pDimension->init(this);
	m_pChunkSource = createChunkSource();

	if (m_bIsNew)
		setInitialSpawn();
	updateSkyBrightness();
}

Level::Level()
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
	m_pMinecraftServer = nullptr;
	m_randValue = Random().nextInt();
	m_addend = 1013904223;
	m_bUpdateLights = true;
	m_maxRecurse = 0;
	m_bIsNew = false;
	m_bAllPlayersSleeping = false;
	m_bAnyPlayersSleeping = false;
	m_bUpdatingTileEntities = false;
	//@Note: Using a way higher value for now, used in newer versions
	m_saveInterval = 6000;
	m_delayUntilNextMoodSound = m_random.nextInt(12000);
	m_oThunderLevel = 0;
	m_thunderLevel = 0;
	m_oRainLevel = 0;
	m_rainLevel = 0;
	m_flashTime = 0;
	m_pPathFinder = new PathFinder();
	m_pConnection = nullptr;
}

Level::Level(MinecraftServer* server, Dimension* pDimension) : Level()
{
	m_pMinecraftServer = server;
	if (m_pMinecraftServer)
		m_pDataStorage = server->m_pDataStorage;
	init(pDimension);
}

Level::~Level()
{
	SAFE_DELETE(m_pChunkSource);
	SAFE_DELETE(m_pDimension);
	SAFE_DELETE(m_pPathFinder);
	m_entities.clear();
	m_players.clear();
}

bool Level::isValidPos(const ChunkPos& pos) const
{
	return getLevelData().isValidPos(m_pDimension->m_ID, pos);
}

bool Level::hasFakeChunks() const
{
	return getLevelData().getLimit(m_pDimension->m_ID).m_bFakeChunks;
}

ChunkSource* Level::createChunkSource()
{
	if (m_pMinecraftServer)
	{
		return new ChunkCache(this, m_pDimension->createStorage(), m_pDimension->createRandomLevelSource());
	}

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

	y = 1.0F - y;
	y = (y * (1.0 - (getRainLevel(f) * 5.0F) / 16.0));
	y = (y * (1.0 - (getThunderLevel(f) * 5.0F) / 16.0));
	y = 1.0F - y;

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
	if (pos.x < C_MIN_X || pos.z < C_MIN_Z || pos.x >= C_MAX_X || pos.z >= C_MAX_Z || pos.y < C_MIN_Y || pos.y >= C_MAX_Y)
		// there's nothing out there!
		return 0;

	LevelChunk* pChunk = getChunk(pos);
	return pChunk->getTile(pos);
}

int Level::getData(const TilePos& pos) const
{
	if (pos.x < C_MIN_X || pos.z < C_MIN_Z || pos.x >= C_MAX_X || pos.z > C_MAX_Z || pos.y < C_MIN_Y || pos.y >= C_MAX_Y)
		// there's nothing out there!
		return 0;

	LevelChunk* pChunk = getChunk(pos);
	return pChunk->getData(pos);
}

int Level::getBrightness(const LightLayer& ll, const TilePos& pos) const
{
	TilePos fixedPos(pos.x, Mth::clamp(pos.y, 0, 127), pos.z);
	if (fixedPos.x < C_MIN_X || fixedPos.z < C_MIN_Z || fixedPos.x >= C_MAX_X || fixedPos.z > C_MAX_Z || fixedPos.y < C_MIN_Y || fixedPos.y >= C_MAX_Y)
		// there's nothing out there!
		return ll.m_x;

	if (!hasChunk(fixedPos))
		return 0;

	LevelChunk* pChunk = getChunk(fixedPos);
	return pChunk->getBrightness(ll, fixedPos);
}

float Level::getBrightness(const TilePos& pos) const
{
	return m_pDimension->m_brightnessRamp[getRawBrightness(pos)];
}

int Level::getTileRawBrightness(const TilePos& pos) const
{
	if (pos.y < 0) return 0;
	TilePos fixedPos(pos.x, Mth::min(pos.y, 127), pos.z);
	return getChunk(fixedPos)->getRawBrightness(fixedPos, 0);
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
	if (b && (getTile(pos) == Tile::stoneSlabHalf->m_ID || getTile(pos) == Tile::farmland->m_ID || getTile(pos) == Tile::stairsStone->m_ID || getTile(pos) == Tile::stairsWood->m_ID))
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


	TilePos fixedPos(pos.x, Mth::min(pos.y, 127), pos.z);

	LevelChunk* pChunk = getChunk(fixedPos);
	return pChunk->getRawBrightness(fixedPos, m_skyDarken);
}

std::shared_ptr<TileEntity> Level::getTileEntity(const TilePos& pos) const 
{
	LevelChunk* pChunk = getChunk(pos);
	return pChunk ? pChunk->getTileEntity(pos) : nullptr;
}

void Level::setTileEntity(const TilePos& pos, std::shared_ptr<TileEntity> tileEntity) 
{
	if (!tileEntity->isRemoved())
	{
		if (m_bUpdatingTileEntities)
		{
			tileEntity->m_pos = pos;
			m_pendingTileEntities.push_back(tileEntity);
		}
		else
		{
			m_tileEntityList.push_back(tileEntity);
			LevelChunk* pChunk = getChunk(pos);
			if (pChunk)
				pChunk->setTileEntity(pos, tileEntity);
		}
	}
}

void Level::removeTileEntity(const TilePos& pos) 
{
	auto old = getTileEntity(pos);

	if (old && m_bUpdatingTileEntities)
	{
		old->setRemoved();
	}
	else
	{
		if (old)
			Util::remove(m_tileEntityList, old);
		LevelChunk* pChunk = getChunk(pos);
		if (pChunk)
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

uint8_t* Level::getBlocksAndData(const TilePos& pos1, int xs, int ys, int zs)
{
	uint8_t* var7 = new uint8_t[xs * ys * zs * 5 / 2];
	int var8 = pos1.x >> 4;
	int var9 = pos1.z >> 4;
	int var10 = pos1.x + xs - 1 >> 4;
	int var11 = pos1.z + zs - 1 >> 4;
	int i = 0;
	int var13 = pos1.y;
	int var14 = pos1.y + ys;
	if (pos1.y < 0) {
		var13 = 0;
	}

	if (var14 > 128) {
		var14 = 128;
	}

	ChunkPos pos;
	for (pos.x = var8; pos.x <= var10; ++pos.x) {
		int var16 = pos1.x - pos.x * 16;
		int var17 = pos1.x + xs - pos.x * 16;
		if (var16 < 0) {
			var16 = 0;
		}

		if (var17 > 16) {
			var17 = 16;
		}

		for (pos.z = var9; pos.z <= var11; ++pos.z) {
			int var19 = pos1.z - pos.z * 16;
			int var20 = pos1.z + zs - pos.z * 16;
			if (var19 < 0) {
				var19 = 0;
			}

			if (var20 > 16) {
				var20 = 16;
			}

			i = getChunk(pos)->getBlocksAndData(var7, var16, var13, var19, var17, var14, var20, i);
		}
	}

	return var7;
}

void Level::setBlocksAndData(const TilePos& pos, int xs, int ys, int zs, uint8_t* data)
{
	int var8 = pos.x >> 4;
	int var9 = pos.z >> 4;
	int var10 = pos.x + xs - 1 >> 4;
	int var11 = pos.z + zs - 1 >> 4;
	int i = 0;
	int var13 = pos.y;
	int var14 = pos.y + ys;
	if (pos.y < 0) {
		var13 = 0;
	}

	if (var14 > 128) {
		var14 = 128;
	}

	ChunkPos cp;
	for (cp.x = var8; cp.x <= var10; ++cp.x) {
		int var16 = pos.x - cp.x * 16;
		int var17 = pos.x + xs - cp.x * 16;
		if (var16 < 0) {
			var16 = 0;
		}

		if (var17 > 16) {
			var17 = 16;
		}

		for (cp.z = var9; cp.z <= var11; ++cp.z) {
			int var19 = pos.z - cp.z * 16;
			int var20 = pos.z + zs - cp.z * 16;
			if (var19 < 0) {
				var19 = 0;
			}

			if (var20 > 16) {
				var20 = 16;
			}

			m_pChunkSource->create(cp);
			i = getChunk(cp)->setBlocksAndData(data, var16, var13, var19, var17, var14, var20, i);
			setTilesDirty(TilePos(cp.x * 16 + var16, var13, cp.z * 16 + var19), TilePos(cp.x * 16 + var17, var14, cp.z * 16 + var20));
		}
	}
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

bool Level::isNormalTile(const TilePos& pos) const
{
	Tile* pTile = Tile::tiles[getTile(pos)];
	if (!pTile) return false;

	return pTile->m_pMaterial->isOpaque() && pTile->isCubeShaped();
}

float Level::getThunderLevel(float f) const
{
	return (m_oThunderLevel + (m_thunderLevel - m_oThunderLevel) * f) * getRainLevel(f);
}

void Level::setThunderLevel(float level)
{
	m_oThunderLevel = m_thunderLevel = Mth::clamp(level, 0.0f, 1.0f);
}

float Level::getRainLevel(float f) const
{
	return m_oRainLevel + (m_rainLevel - m_oRainLevel) * f;
}

void Level::setRainLevel(float level)
{
	m_oRainLevel = m_rainLevel = Mth::clamp(level, 0.0f, 1.0f);
}

bool Level::isThundering() const
{
	return getThunderLevel(1.0f) > 0.9f;
}

bool Level::isRaining() const
{
	return getRainLevel(1.0f) > 0.2f;
}

bool Level::isRainingAt(const TilePos& pos) const
{
	if (!isRaining())
		return false;
	else if (!canSeeSky(pos))
		return false;
	else if (getHeightmap(pos) > pos.y)
		return false;
	else
	{
		Biome* biome = getBiomeSource()->getBiome(pos);
		return biome->m_bHasSnow ? false : biome->m_bHasRain;
	}
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

EntityVector Level::getEntities(const Entity* pEntExclude, const AABB& aabb) const
{
	return getEntitiesOfPredicate([pEntExclude](Entity* pEnt) { return pEnt != pEntExclude; }, aabb);
}

EntityVector Level::getEntitiesOfCategory(EntityCategories::CategoriesMask category, const AABB& aabb) const
{
	return getEntitiesOfPredicate([category](Entity* pEnt) { return pEnt->getCategory().contains(category); }, aabb);
}

EntityVector Level::getEntitiesOfType(EntityType* type, const AABB& aabb) const
{
	return getEntitiesOfPredicate([type](Entity* pEnt) { return pEnt->getType() == type; }, aabb);
}

EntityVector Level::getPlayers(const AABB& aabb) const
{
	return getEntitiesOfType(EntityType::player, aabb);
}

std::shared_ptr<Player> Level::getPlayer(const std::string& name)
{
	for (auto& p : m_players)
		if (p->m_name == name) return p;
	return nullptr;
}

void Level::broadcastAll(Packet* packet)
{
	if (m_pConnection && !m_bIsOnline)
	{
		for (auto& player : m_players)
		{
			m_pConnection->send(player, packet, false);
		}
	}
	delete packet;
}

void Level::broadcastToAllInRange(Player* avoid, const Vec3& pos, real range, Packet* packet)
{
	if (m_pConnection && !m_bIsOnline)
	{
		for (auto& player : m_players)
		{
			if (player.get() != avoid)
			{
				Vec3 diff = pos - player->m_pos;
				if (diff.lengthSqr() < range * range)
					m_pConnection->send(player, packet, false);
			}
		}
	}
	delete packet;
}

void Level::setUpdateLights(bool b)
{
	m_bUpdateLights = b;
}

bool Level::updateLights()
{
	if (m_maxRecurse >= 50)
		return false;

	++m_maxRecurse;

	bool all = false;

	for (int i = 500; !m_lightUpdates.empty();)
	{
		--i;

		if (i <= 0)
		{
			all = true;
			break;
		}

		LightUpdate lu = *(m_lightUpdates.end() - 1);
		m_lightUpdates.pop_back();

		lu.update(this);
	}

	--m_maxRecurse;
	return all;
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
	if (isNormalTile(pos))
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

int Level::getHeightmap(const TilePos& pos) const
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
	if (hasChunkAt(pos))
		getChunkAt(pos)->markUnsaved();

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
		TileID tile = getTile(pos);
		if (Tile::blockUpdate[tile])
			tileUpdated(pos, tile);
		else
			updateNeighborsAt(pos, tile);
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

void Level::entityAdded(const std::shared_ptr<Entity>& pEnt)
{
	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->entityAdded(pEnt);
	}
}

void Level::entityRemoved(const std::shared_ptr<Entity>& pEnt)
{
	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->entityRemoved(pEnt);
	}
}

AABBVector* Level::getCubes(const Entity* pEnt, const AABB& aabb)
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

	AABB grew = aabb;
	grew.grow(0.25);
	auto entities = getEntities(pEnt, grew);
	for (std::vector<std::shared_ptr<Entity>>::iterator it = entities.begin(); it != entities.end(); it++)
	{
		auto& ent = *it;
		auto var13 = ent->getCollideBox();
		if (var13 && var13->intersect(aabb))
			m_aabbs.push_back(*var13);

		var13 = pEnt->getCollideAgainstBox(ent.get());
		if (var13 && var13->intersect(aabb))
			m_aabbs.push_back(*var13);
	}

	auto& limit = getLevelData().getLimit(m_pDimension->m_ID);


	if (limit != DimensionLimit::ZERO)
	{
		AABB minX(C_MIN_X, C_MIN_Z, limit.m_minPos.z * 16, limit.m_minPos.x * 16, C_MAX_Z, limit.m_maxPos.z * 16);
		if (minX.intersect(aabb))
			m_aabbs.push_back(minX);

		AABB minZ(limit.m_minPos.x * 16, C_MIN_Z, C_MIN_Z, limit.m_maxPos.x * 16, C_MAX_Z, limit.m_minPos.z * 16);
		if (minZ.intersect(aabb))
			m_aabbs.push_back(minZ);

		AABB maxX(limit.m_maxPos.x * 16, C_MIN_Z, limit.m_minPos.z * 16, C_MAX_X, C_MAX_Z, limit.m_maxPos.z * 16);
		if (maxX.intersect(aabb))
			m_aabbs.push_back(maxX);

		AABB maxZ(limit.m_minPos.x * 16, C_MIN_Z, limit.m_maxPos.z * 16, limit.m_maxPos.x * 16, C_MAX_Z, C_MAX_Z);
		if (maxZ.intersect(aabb))
			m_aabbs.push_back(maxZ);
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

				if (max.y >= (pos.y + 1 - LiquidTile::getHeight(getData(pos))))
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
	return getLevelData().getSpawn();
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
		if (entity->getCategory().contains(category)) count++;
	}
	return count;
}

void Level::validateSpawn()
{
	if (getLevelData().isFlat())
	{
		getLevelData().setYSpawn(getTopSolidBlock(TilePos(0, 0, 0)) - 1);
		return;
	}

	getLevelData().setYSpawn(C_MAX_Y / 2);

	TilePos spawn(0, 64, 0);

	while (!getTopTile(spawn)) {
		spawn.x += m_random.nextInt(8) - m_random.nextInt(8);
		spawn.z += m_random.nextInt(8) - m_random.nextInt(8);
	}

	getLevelData().setSpawn(spawn);
}

void Level::removeAllPendingEntityRemovals(bool limited)
{
	Util::removeAll(m_entities, m_pendingEntityRemovals);

	for (EntityVector::iterator it = m_pendingEntityRemovals.begin(); it != m_pendingEntityRemovals.end(); it++)
	{
		std::shared_ptr<Entity> ent = *it;
		ent->removed();

		LevelChunk* chunk = getChunk(ent->m_chunkPos);
		if (chunk) chunk->removeEntity(ent);

		entityRemoved(ent);
	}

	m_pendingEntityRemovals.clear();

	if (!limited) 
	{
		for (EntityVector::iterator it = m_entities.begin(); it != m_entities.end(); ) {
			std::shared_ptr<Entity> ent = *it;
			if (ent->m_pRiding) {
				if (!ent->m_pRiding->m_bRemoved && ent->m_pRiding->m_pRider == ent) {
					it++;
					continue;
				}

				ent->m_pRiding->m_pRider = nullptr;
				ent->m_pRiding = nullptr;
			}

			if (ent->m_bRemoved) {
				if (ent->m_bInChunk && hasChunk(ent->m_chunkPos)) {
					getChunk(ent->m_chunkPos)->removeEntity(ent);
				}

				entityRemoved(*it);
				it = m_entities.erase(it);
			}
			else it++;
		}
	}
}

void Level::removeEntities(const EntityVector& vec)
{
	m_pendingEntityRemovals.insert(m_pendingEntityRemovals.end(), vec.begin(), vec.end());
}

bool Level::removeEntity(const std::shared_ptr<Entity>& pEnt)
{
	if (pEnt->m_pRider)
		pEnt->m_pRider->ride(nullptr);

	if (pEnt->m_pRiding)
		pEnt->ride(nullptr);

	pEnt->remove();

	if (pEnt->isPlayer())
	{
		auto p = std::dynamic_pointer_cast<Player>(pEnt);
		return Util::remove(m_players, p);
	}

	return false;
}

void Level::removeEntityImmediately(const std::shared_ptr<Entity>& entity)
{
	entity->remove();
	if (entity->isPlayer())
	{
		Util::remove(m_players, std::dynamic_pointer_cast<Player>(entity));
		updateSleeping();
	}

	ChunkPos& pos = entity->m_chunkPos;
	if (entity->m_bInChunk && hasChunk(pos))
		getChunk(pos)->removeEntity(entity);

	Util::remove(m_entities, entity);
	entityRemoved(entity);
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
		auto p = std::dynamic_pointer_cast<Player>(pEnt);
		m_players.push_back(p);
	}
	getChunk(pEnt->m_pos)->addEntity(pEnt);
	m_entities.push_back(pEnt);
	pEnt->startSynchedData();
	entityAdded(pEnt);

	return true;
}

void Level::loadPlayer(std::shared_ptr<Player> player)
{
	if (!player) return;

	getLevelData().setLoadedPlayerTo(player.get());

	addEntity(player);
}

void Level::prepare()
{
	while (m_pChunkSource->tick());
}

void Level::saveLevelData()
{
	if (m_pMinecraftServer)
		m_pMinecraftServer->saveLevelData();
}

void Level::savePlayerData()
{
	if (m_pMinecraftServer)
		m_pMinecraftServer->savePlayerData();
	m_pDataStorage->save();
}

void Level::saveAllChunks()
{
	m_pChunkSource->saveAll();
}

void Level::save(bool force)
{
	if (m_pChunkSource->shouldSave())
	{
		m_pChunkSource->save(force);
	}
}

void Level::save(bool force, ProgressListener& progress)
{
	if (m_pChunkSource->shouldSave())
	{
		progress.progressStartNoAbort("Saving level");
		saveLevelData();
		progress.progressStage("Saving chunks");
		m_pChunkSource->save(force, progress);
	}
}

void Level::updateSleeping()
{
	m_bAllPlayersSleeping = !m_players.empty();
	m_bAnyPlayersSleeping = false;

	for (auto& p : m_players)
	{
		if (p->isSleeping())
			m_bAnyPlayersSleeping = true;
		else
			m_bAllPlayersSleeping = false;
	}
}

void Level::wakeUpAllPlayers()
{
	resetWeatherCycle();
	m_bAllPlayersSleeping = false;

	for (auto& p : m_players)
		if (p->isSleeping())
			p->wake(false, false, true);
}

bool Level::isAnyPlayersSleeping() const
{
	return m_bAnyPlayersSleeping;
}

bool Level::isAllPlayersSleepingLongEnough()
{
	if (m_bAllPlayersSleeping && !m_bIsOnline) {

		for (auto& p : m_players)
		{
			if (!p->isSleepingLongEnough()) return false;
		}

		return true;
	}
	else {
		return false;
	}
}

void Level::playStreamingMusic(const std::string& record, const TilePos& pos)
{
	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->playStreamingMusic(record, pos);
	}
}

void Level::tileEvent(const TilePos& pos, int info, int info2)
{
	int tile = getTile(pos);
	if (tile > 0)
		Tile::tiles[tile]->triggerEvent(this, pos, info, info2);
}

void Level::setSavedData(const std::string& key, std::shared_ptr<SavedData> data)
{
	m_pDataStorage->set(key, data);
}

int Level::getFreeMapId(const std::string& key)
{
	return m_pDataStorage->getFreeMapId(key);
}

void Level::levelEvent(int event, const TilePos& pos, int info)
{
	levelEvent(nullptr, event, pos, info);
}

void Level::levelEvent(Player* player, int event, const TilePos& pos, int info)
{
	for (std::vector<LevelListener*>::iterator it = m_levelListeners.begin(); it != m_levelListeners.end(); it++)
	{
		LevelListener* pListener = *it;
		pListener->levelEvent(player, event, pos, info);
	}
}

void Level::entityEvent(Entity* ent, int8_t event)
{
	if (!m_bIsOnline)
	{
		EntityTracker& tracker = getServer()->getEntityTracker(m_pDimension->m_ID);
		if (tracker.needsBroadcasting())
			tracker.broadcast(ent->shared_from_this(), new EntityEventPacket(ent->m_EntityID, event));
	}
}

void Level::sendLevelInfo(Player* player)
{
	m_pConnection->send(player, new SetTimePacket(getTime()));
	if (isRaining())
		m_pConnection->send(player, new GameEventPacket(1));
}

void Level::setInitialSpawn()
{
	if (getLevelData().isFlat()) return;

	TilePos spawn(0, 64, 0);

	const DimensionLimit& limit = getLevelData().getLimit(m_pDimension->m_ID);

	if (limit != DimensionLimit::ZERO && (limit.m_minPos.x >= 0 || limit.m_minPos.z >= 0))
	{
		if (limit.m_minPos.x >= 0)
			spawn.x = (limit.m_maxPos.x - limit.m_minPos.x) / 2 * 16;
		if (limit.m_minPos.z >= 0)
			spawn.z = (limit.m_maxPos.z - limit.m_minPos.z) / 2 * 16;

		spawn.y = getTopSolidBlock(spawn) - 1;
		getLevelData().setSpawn(spawn);
		return;
	}

	m_bCalculatingInitialSpawn = true;

	for (; !m_pDimension->isValidSpawn(spawn); spawn.z += m_random.nextInt(64) - m_random.nextInt(64)) {
		spawn.x += m_random.nextInt(64) - m_random.nextInt(64);

		if (limit != DimensionLimit::ZERO && !isValidPos(spawn))
		{
			spawn = getLevelData().getSpawn();
			spawn.y = getTopSolidBlock(spawn) - 1;
			break;
		}
	}

	getLevelData().setSpawn(spawn);

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

	float rain = getRainLevel(f);

	if (rain > 0)
	{
		float m = (sky.x * 0.3F + sky.y * 0.59F + sky.z * 0.11F) * 0.6F;
		float light = 1.0F - rain * (12.0F / 16.0F);
		sky.x = sky.x * light + m * (1.0F - light);
		sky.y = sky.y * light + m * (1.0F - light);
		sky.z = sky.z * light + m * (1.0F - light);
	}

	float thunder = getThunderLevel(f);

	if (thunder > 0)
	{
		float m = (sky.x * 0.3F + sky.y * 0.59F + sky.z * 0.11F) * 0.2F;
		float light = 1.0F - thunder * (12.0F / 16.0F);
		sky.x = sky.x * light + m * (1.0F - light);
		sky.y = sky.y * light + m * (1.0F - light);
		sky.z = sky.z * light + m * (1.0F - light);
	}

	if (m_flashTime > 0)
	{
		float flash = m_flashTime - f;
		if (flash > 1.0F)
			flash = 1.0F;

		flash *= 0.45F;
		sky.x = sky.x * (1.0F - flash) + 0.8F * flash;
		sky.y = sky.y * (1.0F - flash) + 0.8F * flash;
		sky.z = sky.z * (1.0F - flash) + 1.0F * flash;
	}
	return sky;
}

Vec3f Level::getFogColor(float f) const
{
	return m_pDimension->getFogColor(getTimeOfDay(f), f);
}

Vec3 Level::getCloudColor(float f) const
{
	Vec3 result(1, 1, 1);

	float fTODCosAng = Mth::cos(getSunAngle(f));

	float mult = 2 * fTODCosAng + 0.5f;
	if (mult < 0.0f)
		mult = 0.0f;
	if (mult > 1.0f)
		mult = 1.0f;

	float rain = getRainLevel(f);

	if (rain > 0)
	{
		float m = (result.x * 0.3F + result.y * 0.59F + result.z * 0.11F) * 0.6F;
		float light = 1.0F - rain * 0.95f;
		result.x = result.x * light + m * (1.0F - light);
		result.y = result.y * light + m * (1.0F - light);
		result.z = result.z * light + m * (1.0F - light);
	}


	result.x *= mult * 0.9f + 0.1f;
	result.y *= mult * 0.9f + 0.1f;
	result.z *= mult * 0.85f + 0.15f;

	float thunder = getThunderLevel(f);

	if (thunder > 0)
	{
		float m = (result.x * 0.3F + result.y * 0.59F + result.z * 0.11F) * 0.2F;
		float light = 1.0F - thunder * 0.95f;
		result.x = result.x * light + m * (1.0F - light);
		result.y = result.y * light + m * (1.0F - light);
		result.z = result.z * light + m * (1.0F - light);
	}

	return result;
}

bool Level::canChunkExist(const ChunkPos& pos) {

    for (const auto& player : m_players) {

        int dx = Mth::abs(pos.x - player->m_chunkPos.x);
        int dz = Mth::abs(pos.z - player->m_chunkPos.z);

        if (dx <= m_viewDistance && dz <= m_viewDistance)
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

bool Level::tickPendingTicks(bool b)
{
	//@Note: Replaced 65536 with 1000, as this is more accurate to the original code
	int size = Mth::min(m_pendingTicks.size(), 1000);

	for (int i = 0; i < size; i++)
	{
		const TickNextTickData& t = *m_pendingTicks.begin();
		if (!b && t.m_delay > getLevelData().getTime())
			break;

		if (hasChunksAt(t.m_pos - 8, t.m_pos + 8))
		{
			TileID tile = getTile(t.m_pos);
			if (tile == t.m_tileId && tile > 0)
				Tile::tiles[tile]->tick(this, t.m_pos, &m_random);
		}

		m_pendingTicks.erase(m_pendingTicks.begin());
	}

	return !m_pendingTicks.empty();
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

	if (m_delayUntilNextMoodSound > 0)
		m_delayUntilNextMoodSound--;

	for (std::unordered_set<ChunkPos>::iterator it = m_chunksToUpdate.begin(); it != m_chunksToUpdate.end(); it++)
	{
		ChunkPos pos = *it;
		TilePos tPos(pos);
		LevelChunk* pChunk = getChunk(pos);

		if (m_delayUntilNextMoodSound == 0)
		{
			m_randValue = m_randValue * 3 + m_addend;
			int rand = m_randValue >> 2;
			TilePos tp(tPos.x + rand & 15, rand >> 16 & 127, tPos.z + (rand >> 8 & 15));

			TileID tile = pChunk->getTile(tp);
			if (tile == TILE_AIR && getTileRawBrightness(tp) <= m_random.nextInt(8) && getBrightness(LightLayer::Sky, tp) <= 0)
			{
				Vec3 soundPos = tp.center();
				auto nearest = getNearestPlayer(soundPos, 8.0);
				if (nearest && nearest->distanceToSqr(soundPos) > 4.0)
				{
					playSound(soundPos, "ambient.cave.cave", 0.7F, 0.8F + m_random.nextFloat() * 0.2F);
					m_delayUntilNextMoodSound = m_random.nextInt(12000) + 6000;
				}
			}
		}

		if (m_random.nextInt(100000) == 0 && isRaining() && isThundering())
		{
			m_randValue = m_randValue * 3 + 1013904223;
			int rand = m_randValue >> 2;
			TilePos tp(tPos.x + (rand & 15), 0, tPos.z + ((rand >> 8) & 15));
			tp.y = getTopSolidBlock(tp);
			if (isRainingAt(tp))
				addEntity(std::make_shared<LightningBolt>(this, tp));
		}

		if (m_random.nextInt(16) == 0)
		{
			m_randValue = m_randValue * 3 + 1013904223;
			int rand = m_randValue >> 2;
			TilePos tp(tPos.x + (rand & 15), 0, tPos.z + (rand >> 8 & 15));
			tp.y = getTopSolidBlock(tp);
			if (getBiomeSource()->getBiome(tp)->m_bHasSnow && tp.y >= 0 && tp.y < 128 && getBrightness(LightLayer::Block, tp) < 10)
			{
				TileID belowTop = pChunk->getTile(tp.below());
				TileID top = pChunk->getTile(tp);
				if (isRaining() && top == 0 && Tile::topSnow->mayPlace(this, tp) && belowTop != 0 && belowTop != Tile::ice->m_ID && Tile::tiles[belowTop]->m_pMaterial->isSolid())
					setTile(tp, Tile::topSnow->m_ID);

				if (belowTop == Tile::calmWater->m_ID && pChunk->getData(tp.below()) == 0)
					setTile(tp.below(), Tile::ice->m_ID);
			}
		}

		TilePos tilePos(tPos);
		for (int i = 0; i < 80; i++)
		{
			m_randValue = m_randValue * 3 + m_addend;
			int rand = m_randValue >> 2;

			tilePos.x = tPos.x + rand & 15;
			tilePos.y = tPos.y + (rand >> 16) & 127;
			tilePos.z = tPos.z + (rand >> 8) & 15;

			TileID tile = pChunk->getTile(tilePos);
			if (Tile::shouldTick[tile])
				Tile::tiles[tile]->tick(this, tilePos + pos, &m_random);
		}
	}
}

void Level::tickWeather()
{
	if (m_flashTime > 0)
		--m_flashTime;

	if (!m_bIsOnline && !m_pDimension->m_bHasCeiling)
	{
		bool wasRaining = isRaining();
		int thunder = getLevelData().getThunderTime();
		if (thunder <= 0)
		{
			if (getLevelData().isThundering())
				getLevelData().setThunderTime(m_random.nextInt(12000) + 3600);
			else
				getLevelData().setThunderTime(m_random.nextInt(168000) + 12000);
		}
		else
		{
			--thunder;
			getLevelData().setThunderTime(thunder);
			if (thunder <= 0)
				getLevelData().setThundering(!getLevelData().isThundering());
		}

		int rain = getLevelData().getRainTime();
		if (rain <= 0)
		{
			if (getLevelData().isRaining())
				getLevelData().setRainTime(m_random.nextInt(12000) + 12000);
			else
				getLevelData().setRainTime(m_random.nextInt(168000) + 12000);
		}
		else {
			--rain;
			getLevelData().setRainTime(rain);
			if (rain <= 0)
				getLevelData().setRaining(!getLevelData().isRaining());
		}

		m_oRainLevel = m_rainLevel;
		if (getLevelData().isRaining())
			m_rainLevel += 0.01f;
		else
			m_rainLevel -= 0.01f;

		m_rainLevel = Mth::clamp(m_rainLevel, 0.0f, 1.0f);

		m_oThunderLevel = m_thunderLevel;
		if (getLevelData().isThundering())
			m_thunderLevel += 0.1f;
		else
			m_thunderLevel -= 0.1f;

		m_thunderLevel = Mth::clamp(m_thunderLevel, 0.0f, 1.0f);

		if (wasRaining != isRaining())
			broadcastAll(new GameEventPacket(isRaining() ? 1 : 2));
	}
}

void Level::resetWeatherCycle()
{
	getLevelData().setRainTime(0);
	getLevelData().setRaining(false);
	getLevelData().setThunderTime(0);
	getLevelData().setThundering(false);
}

void Level::tick(std::shared_ptr<Entity> pEnt, bool b) const
{
	TilePos tilePos(pEnt->m_pos);


	if (b && !hasChunksAt(TilePos(tilePos.x - 32, 0, tilePos.z - 32), TilePos(tilePos.x + 32, 128, tilePos.z + 32))) return;
	

	pEnt->m_posPrev = pEnt->m_pos;
	pEnt->m_rotPrev = pEnt->m_rot;

	if (b && pEnt->m_bInChunk)
	{
		if (pEnt->m_pRiding) pEnt->rideTick();
		else pEnt->tick();
	}

	if (std::isnan(pEnt->m_pos.x) || std::isinf(pEnt->m_pos.x)) {
		pEnt->m_pos.x = pEnt->m_posPrev.x;
		LOG_I("something went wrong x");
	}

	if (std::isnan(pEnt->m_pos.y) || std::isinf(pEnt->m_pos.y)) {
		pEnt->m_pos.y = pEnt->m_posPrev.y;
		LOG_I("something went wrong y");
	}

	if (std::isnan(pEnt->m_pos.z) || std::isinf(pEnt->m_pos.z)) {
		pEnt->m_pos.z = pEnt->m_posPrev.z;
		LOG_I("something went wrong z");
	}

	if (std::isnan(pEnt->m_rot.x) || std::isinf(pEnt->m_rot.x)) {
		pEnt->m_rot.x = pEnt->m_rotPrev.x;
	}

	if (std::isnan(pEnt->m_rot.y) || std::isinf(pEnt->m_rot.y)) {
		pEnt->m_rot.y = pEnt->m_rotPrev.y;
	}

	ChunkPos cp(pEnt->m_pos);

	if (!pEnt->m_bInChunk || cp != pEnt->m_chunkPos)
	{
		// move the entity to the new chunk
		if (pEnt->m_bInChunk && hasChunk(pEnt->m_chunkPos))
		{
			getChunk(pEnt->m_chunkPos)->removeEntity(pEnt);
		}

		if (hasChunk(cp))
		{
			pEnt->m_bInChunk = true;
			getChunk(cp)->addEntity(pEnt);
		}
		else
		{
			pEnt->m_bInChunk = false;
		}
	}
	else if (pEnt->m_bInChunk)
	{
		if (pEnt->m_chunkPosY != ChunkPos::ToChunkCoordinate(pEnt->m_pos.y))
		{
			getChunk(cp)->updateEntity(pEnt);
		}
	}

	if (b && pEnt->m_bInChunk && pEnt->m_pRider)
	{
		if (!pEnt->m_pRider->m_bRemoved && pEnt->m_pRider->m_pRiding == pEnt)
			tick(pEnt->m_pRider);
		else
		{
			pEnt->m_pRider->m_pRiding = nullptr;
			pEnt->m_pRider = nullptr;
		}
	}

}

int TICKSPERSECOND = 0;
int LASTTICKED = 0;

void Level::tick()
{
	if (!m_bIsOnline && getTime() % 20 == 0)
	{
		broadcastAll(new SetTimePacket(getTime()));
	}

	tickWeather();
	if (isAllPlayersSleepingLongEnough())
	{
		bool var4 = false;

		if (m_difficulty >= 1)
			var4 = MobSpawner::spawnNightmare(this, m_players);

		if (!var4)
		{
			int64_t time = getLevelData().getTime() + 24000L;
			getLevelData().setTime(time - time % 24000L);
			wakeUpAllPlayers();
		}
	}

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

	if (time % m_saveInterval == 0L)
		save(false);
	

	tickPendingTicks(false);
	tickTiles();
}

void Level::tickEntities()
{
	// inlined in the original
	removeAllPendingEntityRemovals(true);

	for (int i = 0; i < int(m_entities.size()); i++)
	{
		std::shared_ptr<Entity>& pEnt = m_entities[i];

		if (pEnt->m_pRiding) {
			if (!pEnt->m_pRiding->m_bRemoved && pEnt->m_pRiding->m_pRider == pEnt) {
				i++;
				continue;
			}

			pEnt->m_pRiding->m_pRider = nullptr;
			pEnt->m_pRiding = nullptr;
		}

		if (!pEnt->m_bRemoved)
		{
			tick(pEnt);
		}
		else
		{
			if (pEnt->m_bInChunk && hasChunk(pEnt->m_chunkPos))
				getChunk(pEnt->m_chunkPos)->removeEntity(pEnt);

			entityRemoved(pEnt);

			m_entities.erase(m_entities.begin() + i);
			i--;
		}
	}

	m_bUpdatingTileEntities = true;
	for (auto it = m_tileEntityList.begin(); it != m_tileEntityList.end(); )
	{
		auto& tileEntity = *it;
		if (!tileEntity->isRemoved())
		{
			tileEntity->tick();
			it++;
		}
		else
		{
			LevelChunk* pChunk = getChunk(tileEntity->m_pos);
			if (pChunk)
				pChunk->removeTileEntity(tileEntity->m_pos);

			it = m_tileEntityList.erase(it);
		}

	}
	m_bUpdatingTileEntities = false;

	if (!m_pendingTileEntities.empty())
	{
		for (auto& tileEntity : m_pendingTileEntities)
		{
			if (tileEntity->isRemoved()) continue;
			if (std::find(m_tileEntityList.begin(), m_tileEntityList.end(), tileEntity) == m_tileEntityList.end())
				m_tileEntityList.push_back(tileEntity);

			LevelChunk* pChunk = getChunk(tileEntity->m_pos);
			if (pChunk)
				pChunk->setTileEntity(tileEntity->m_pos, tileEntity);

			sendTileUpdated(tileEntity->m_pos);
		}
		m_pendingTileEntities.clear();
	}
}

void Level::addAllPendingTileEntities(const std::unordered_map<ChunkTilePos, std::shared_ptr<TileEntity>>& tileEntities)
{
	if (m_bUpdatingTileEntities)
		for (auto& it : tileEntities)
			m_pendingTileEntities.push_back(it.second);
	else
		for (auto& it : tileEntities)
			m_tileEntityList.push_back(it.second);
}

HitResult Level::clip(Vec3 v1, Vec3 v2, bool flag, bool flag1) const
{
	TilePos tp1(v1), tp2(v2);

	TileID tile = getTile(tp1);
	int    data = getData(tp1);
	Tile* pTile = Tile::tiles[tile];

	if (pTile && isValidPos(tp1) && (!flag1 || pTile->getAABB(this, tp1)) && tile > 0 && pTile->mayPick(data, flag))
	{
		HitResult hr = pTile->clip(this, tp1, v1, v2);
		if (hr.isHit())
			return hr;
	}

	int counter = 200;
	while (counter-- >= 0)
	{
		if (tp1 == tp2)
			break;

		real xd = 999.0f;
		real yd = 999.0f;
		real zd = 999.0f;
		if (tp2.x > tp1.x) xd = tp1.x + 1.0f;
		if (tp2.x < tp1.x) xd = tp1.x + 0.0f;
		if (tp2.y > tp1.y) yd = tp1.y + 1.0f;
		if (tp2.y < tp1.y) yd = tp1.y + 0.0f;
		if (tp2.z > tp1.z) zd = tp1.z + 1.0f;
		if (tp2.z < tp1.z) zd = tp1.z + 0.0f;
		real xe = 999.0f;
		real ye = 999.0f;
		real ze = 999.0f;
		real xl = v2.x - v1.x;
		real yl = v2.y - v1.y;
		real zl = v2.z - v1.z;
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

		tile = getTile(tp1);
		data = getData(tp1);
		pTile = Tile::tiles[tile];
		if (pTile && isValidPos(tp1) && (!flag1 || pTile->getAABB(this, tp1)) && tile > 0 && pTile->mayPick(data, flag))
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
	return clip(a, b, false, false);
}

void Level::addToTickNextTick(const TilePos& tilePos, int d, int delay)
{
	if (!hasChunksAt(tilePos, 8))
		return;
	TickNextTickData tntd(tilePos, d);
	if (m_bInstantTicking)
	{
		TileID tile = getTile(tntd.m_pos);
		if (tile > 0 && tile == tntd.m_tileId)
			Tile::tiles[tntd.m_tileId]->tick(this, tntd.m_pos, &m_random);
	}
	else
	{
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
		pListener->playSound(name, Vec3(entity->m_pos.x, entity->m_pos.y, entity->m_pos.z), volume, pitch);
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
	broadcastToAllInRange(nullptr, pos, 64.0, new ExplodePacket(pos, power, expl.m_toBlow));
}

void Level::addEntities(const std::vector<std::shared_ptr<Entity>>& entities)
{
	m_entities.insert(m_entities.end(), entities.begin(), entities.end());

	for (auto& e : entities)
	{
		entityAdded(e);
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

int Level::getNetherTravelRatio() const
{
	return m_netherTravelRatio;
}
