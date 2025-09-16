/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <set>
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <math.h>

#include "world/tile/Tile.hpp"
#include "world/entity/Entity.hpp"
#include "world/entity/LocalPlayer.hpp"
#include "world/level/levelgen/chunk/LevelChunk.hpp"
#include "world/level/levelgen/chunk/ChunkSource.hpp"
#include "world/level/storage/LevelStorageSource.hpp"
#include "world/level/storage/LevelSource.hpp"
#include "world/level/storage/LevelData.hpp"
#include "world/level/path/PathFinder.hpp"
#include "Dimension.hpp"
#include "LevelListener.hpp"
#include "TickNextTickData.hpp"
#include "client/renderer/LightUpdate.hpp"

class Dimension;
class Level;
class LevelListener;

typedef std::vector<std::shared_ptr<Entity>> EntityVector;
typedef std::vector<AABB> AABBVector;

class Level : public LevelSource
{
private:
	bool m_bAllPlayersSleeping;
	bool m_bAnyPlayersSleeping;
	int m_netherTravelRatio;
	bool m_bUpdatingTileEntities;

private:
	// @NOTE: LevelListeners do NOT get updated here
	void _setTime(int64_t time) { getLevelData().setTime(time); }

	void resetWeatherCycle();

public:
	virtual void init(Dimension* pDimension = nullptr);
	Level();
	Level(MinecraftServer* pStor, Dimension* pDimension = nullptr);
	~Level();

	bool isValidPos(const ChunkPos& pos) const;

	TileID getTile(const TilePos& pos) const override;
	float getBrightness(const TilePos& pos) const override;
	int getData(const TilePos& pos) const override;
	Material* getMaterial(const TilePos& pos) const override;
	bool isSolidTile(const TilePos& pos) const override;
	bool isNormalTile(const TilePos& pos) const override;

	float getThunderLevel(float) const;
	void setThunderLevel(float);
	float getRainLevel(float) const;
	void setRainLevel(float);
	bool isThundering() const;
	bool isRaining() const;
	bool isRainingAt(const TilePos&) const;
	ChunkSource* getChunkSource() const;
	virtual ChunkSource* createChunkSource();
	LevelChunk* getChunk(const ChunkPos& pos) const;
	LevelChunk* getChunkAt(const TilePos& pos) const;
	int getTileRawBrightness(const TilePos& pos) const;
	int getRawBrightness(const TilePos& pos) const;
	int getRawBrightness(const TilePos& pos, bool b) const;
	std::shared_ptr<TileEntity> getTileEntity(const TilePos& pos) const override;
	void setTileEntity(const TilePos& pos, std::shared_ptr<TileEntity> tileEntity);
	void removeTileEntity(const TilePos& pos);
	int getBrightness(const LightLayer&, const TilePos& pos) const;
	void setBrightness(const LightLayer&, const TilePos& pos, int brightness);
	int getSeaLevel() const { return 63; }
	int64_t getSeed() const { return getLevelData().getSeed(); }
	int64_t getTime() const { return getLevelData().getTime(); }
	void setTime(int64_t time);
	GameType getDefaultGameType() { return getLevelData().getGameType(); }
	int getHeightmap(const TilePos& pos) const;
	bool isDay() const;
	bool isSkyLit(const TilePos& pos) const;
	bool isEmptyTile(const TilePos& pos) const;
	bool hasChunkAt(const TilePos& pos) const;
	bool hasChunk(const ChunkPos& pos) const;
	bool hasChunksAt(const TilePos& min, const TilePos& max) const;
	bool hasChunksAt(const TilePos& pos, int rad) const;
	bool updateSkyBrightness();
	float getTimeOfDay(float f) const;
	int getSkyDarken(float f) const;
	void setUpdateLights(bool b);
	bool updateLights();
	void updateLight(const LightLayer&, const TilePos& tilePos1, const TilePos& tilePos2, bool = true);
	void updateLightIfOtherThan(const LightLayer&, const TilePos& pos, int);
	virtual bool setTileAndDataNoUpdate(const TilePos& pos, TileID tile, int data);
	virtual bool setTileNoUpdate(const TilePos& pos, TileID tile);
	virtual bool setDataNoUpdate(const TilePos& pos, int data);
	bool setTileAndData(const TilePos& pos, TileID tile, int data);
	bool setTile(const TilePos& pos, TileID tile);
	bool setData(const TilePos& pos, int data);
	void sendTileUpdated(const TilePos& pos);
	void tileUpdated(const TilePos& pos, TileID tile);
	void tileEntityChanged(const TilePos& pos, std::shared_ptr<TileEntity> tileEntity);
	void updateNeighborsAt(const TilePos& pos, TileID tile);
	void neighborChanged(const TilePos& pos, TileID tile);
	void setTilesDirty(const TilePos& min, const TilePos& max);
	virtual void entityAdded(Entity* pEnt);
	virtual void entityRemoved(Entity* pEnt);
	void lightColumnChanged(int x, int z, int y1, int y2);
	bool containsFireTile(const AABB&);
	bool containsAnyLiquid(const AABB&);
	bool containsLiquid(const AABB&, const Material *pMtl);
	bool containsMaterial(const AABB&, const Material *pMtl);
	bool checkAndHandleWater(const AABB&, const Material* pMtl, Entity* pEnt);
	TilePos getSharedSpawnPos() const;
	void validateSpawn();
	TileID getTopTile(const TilePos& pos) const;
	int getTopTileY(const TilePos& pos) const;
	int getTopSolidBlock(const TilePos& tilePos) const;
	int countWithCategory(EntityCategories::CategoriesMask category);
	void loadPlayer(std::shared_ptr<Player>);
	virtual bool addEntity(std::shared_ptr<Entity>);
	virtual bool removeEntity(std::shared_ptr<Entity>);
	void removeEntities(const EntityVector&);
	void removeAllPendingEntityRemovals(bool limited = true);
	void prepare();
	void saveLevelData();
	void savePlayerData();
	void saveAllChunks();
	void setInitialSpawn();
	void setSpawnPos(const TilePos& pos) { getLevelData().setSpawn(pos); }
	void setSpawnSettings(bool a, bool b) { }
	bool canSeeSky(const TilePos& pos) const;
	Vec3f getSkyColor(Entity* pEnt, float f) const;
	Vec3f getFogColor(float f) const;
	Vec3 getCloudColor(float f) const;
	bool canChunkExist(const ChunkPos& pos);
	bool isUnobstructed(AABB*) const;
	bool mayInteract(Player* player, const TilePos& pos) const;
	bool mayPlace(TileID tid, const TilePos& pos, bool b, Facing::Name face) const;
	void removeListener(LevelListener*);
	void addListener(LevelListener*);
	void tick(std::shared_ptr<Entity> pEnt, bool = true) const;
	virtual void tick();
	virtual bool tickPendingTicks(bool b);
	virtual void tickTiles();
	virtual void tickWeather();
	void tickEntities();
	void addAllPendingTileEntities(const std::unordered_map<ChunkTilePos, std::shared_ptr<TileEntity>>&);
	virtual void addToTickNextTick(const TilePos& tilePos, int, int);
	void takePicture(std::shared_ptr<TripodCamera> pCamera, Entity* pOwner);
	void addParticle(const std::string& name, const Vec3& pos, const Vec3& dir = Vec3::ZERO);
	void playSound(Entity*, const std::string& name, float volume = 1.0f, float pitch = 1.0f);
	void playSound(const Vec3& pos, const std::string& name, float volume = 1.0f, float pitch = 1.0f);
	void animateTick(const TilePos& pos);
	float getSeenPercent(Vec3, AABB) const;
	void explode(Entity*, const Vec3& pos, float power);
	void explode(Entity*, const Vec3& pos, float power, bool bIsFiery);
	void addEntities(const std::vector<std::shared_ptr<Entity>>& entities);
	void ensureAdded(std::shared_ptr<Entity> entity);
	bool extinguishFire(Player* player, const TilePos& pos, Facing::Name face);
	int findPath(Path* path, Entity* ent1, Entity* ent2, float f) const;
	int findPath(Path* path, Entity* ent, const TilePos& pos, float f) const;
	int getLightDepth(const TilePos& pos) const;
	float getStarBrightness(float f) const;
	float getSunAngle(float f) const;
	int getNetherTravelRatio() const;
	void swap(const TilePos& pos1, const TilePos& pos2);
	uint8_t* getBlocksAndData(const TilePos& pos1, int xs, int ys, int zs);
	void setBlocksAndData(const TilePos& pos, int xs, int ys, int zs, uint8_t*);

	HitResult clip(const Vec3& a, const Vec3& b) const;
	HitResult clip(Vec3 a, Vec3 b, bool c, bool = false) const;
	std::shared_ptr<Entity> getEntity(int id) const;
	const EntityVector* getAllEntities() const;
	template<typename Predicate>
	EntityVector getEntitiesOfPredicate(Predicate predicate, const AABB& aabb) const
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
				pChunk->getEntities(predicate, aabb, entities);
			}
		}

		return entities;
	}
	EntityVector getEntities(const Entity* pAvoid, const AABB&) const;
	EntityVector getEntitiesOfCategory(EntityCategories::CategoriesMask category, const AABB&) const;
	EntityVector getEntitiesOfType(EntityType*, const AABB&) const;
	EntityVector getPlayers(const AABB&) const;
	std::shared_ptr<Player> getPlayer(const std::string&);
	BiomeSource* getBiomeSource() const override;
	MinecraftServer* getServer() const { return m_pMinecraftServer; }
	virtual LevelData& getLevelData() const { return getServer()->m_levelData; }
	AABBVector* getCubes(const Entity* pEnt, const AABB& aabb);
	std::vector<LightUpdate>* getLightsToUpdate();
	std::shared_ptr<Player> getNearestPlayer(const Entity*, float) const;
	std::shared_ptr<Player> getNearestPlayer(const Vec3& pos, float) const;

	// unused redstone stuff
	int getSignal(const TilePos& pos, Facing::Name face) const;
	int getDirectSignal(const TilePos& pos, Facing::Name face) const;
	bool hasDirectSignal(const TilePos& pos) const;
	bool hasNeighborSignal(const TilePos& pos) const;

	void save(bool force = true);
	void save(bool force, ProgressListener&);

	void updateSleeping();
	void wakeUpAllPlayers();
	bool isAnyPlayersSleeping() const;
	bool isAllPlayersSleepingLongEnough();

	void playStreamingMusic(const std::string&, const TilePos&);
	void tileEvent(const TilePos&, int info, int info2);


	template <typename T>
	std::shared_ptr<T> getSavedData(const std::string& key)
	{
		return m_pDataStorage->computeIfAbsent<T>(key);
	}

	void setSavedData(const std::string& key, std::shared_ptr<SavedData> data);

	int getFreeMapId(const std::string& key);

	void levelEvent(int event, const TilePos&, int info);
	void levelEvent(Player*, int event, const TilePos&, int info);
	void entityEvent(Entity*, int event);

protected:
	int m_randValue;
	int m_addend;
	int m_saveInterval;
	int m_delayUntilNextMoodSound;
	float m_oRainLevel;
	float m_rainLevel;
	float m_oThunderLevel;
	float m_thunderLevel;

public:
	AABBVector m_aabbs;
	bool m_bInstantTicking;
	bool m_bIsOnline; // if the level is controlled externally by a server.
	bool m_bPostProcessing;
	EntityVector m_entities;
	std::vector<std::shared_ptr<Player>> m_players;
	int m_skyDarken;
	int m_flashTime;
	bool m_noNeighborUpdate;
	Dimension* m_pDimension;
    int m_difficulty; // @TODO: Difficulty enum
	Random m_random;
	bool m_bCalculatingInitialSpawn;
	std::vector<LevelListener*> m_levelListeners;
	ChunkSource* m_pChunkSource;
	MinecraftServer* m_pMinecraftServer;
	DimensionDataStorage* m_pDataStorage;
	EntityVector m_pendingEntityRemovals;
	std::set<TickNextTickData> m_pendingTicks;
	std::set<ChunkPos> m_chunksToUpdate;
	std::vector<LightUpdate> m_lightUpdates;
	std::vector<std::shared_ptr<TileEntity>> m_tileEntityList;
	std::vector<std::shared_ptr<TileEntity>> m_pendingTileEntities;
	bool m_bUpdateLights;
	int m_maxRecurse;
	bool m_bIsNew;
	int field_B10;
	int viewDistance = 10;
	PathFinder* m_pPathFinder;
};

