#pragma once

#include <unordered_set>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <type_traits>
#include <world/level/levelgen/chunk/ChunkPos.hpp>
#include <world/level/Level.hpp>
#include "Sheep.hpp"

class MobSpawner {
public:
    static constexpr int MIN_SPAWN_DISTANCE = 24;
    static std::unordered_set<ChunkPos> chunksToPoll;

    static TilePos getRandomPosWithin(Level* level, int x, int z) {
        int px = x + level->m_random.nextInt(16);
        int py = level->m_random.nextInt(128);
        int pz = z + level->m_random.nextInt(16);
        return TilePos(px, py, pz);
    }

    static int tick(Level* level, bool allowHostile, bool allowFriendly) {
        if (!allowHostile && !allowFriendly)
            return 0;

        chunksToPoll.clear();

        for (auto& player : level->m_players) {
            int cx = Mth::floor(player->m_pos.x / 16.0f);
            int cz = Mth::floor(player->m_pos.z / 16.0f);

            for (int dx = -8; dx <= 8; ++dx) {
                for (int dz = -8; dz <= 8; ++dz) {
                    chunksToPoll.insert(ChunkPos(cx + dx, cz + dz));
                }
            }
        }

        int totalSpawned = 0;

        for (auto category : MobCategory::values) {
            if ((!category->isFriendly() || allowFriendly) && (category->isFriendly() || allowHostile) &&
                level->countWithCategory(category->getBaseType().getCategoryMask()) <= (category->getMaxInstancesPerChunk() * (int)chunksToPoll.size() / 256)) {

                for (const ChunkPos& pos : chunksToPoll) {
                    Biome* biome = level->getBiomeSource()->getBiome(pos);
                    auto& spawnList = biome->getMobs(category);
                    if (spawnList.empty()) continue;
               
                    EntityType* type = spawnList.begin()->first;

                    int spawnWeight = 0;

                    for (auto it = spawnList.begin(); it != spawnList.end(); spawnWeight += it->second, ++it) {
                    }

              
                    int randomRate = level->m_random.nextInt(spawnWeight);

                    for (auto it = spawnList.begin(); it != spawnList.end(); ++it) {
                        randomRate -= it->second;
                        if (randomRate < 0) {
                            type = it->first;
                            break;
                        }
                        ++it;
                    }

                    if (!type->getCategory().contains(EntityCategories::MOB)) continue;

                    int idx = level->m_random.nextInt((int)spawnList.size());
                    TilePos tpos = getRandomPosWithin(level, pos.x * 16, pos.z * 16);

                    if (level->isSolidTile(tpos)) continue;
                    if (level->getMaterial(tpos) != category->getSpawnPositionMaterial()) continue;

                    int spawned = 0;
                    for (int i = 0; i < 3; ++i) {
                        TilePos tp(tpos);

                        for (int j = 0; j < 4; ++j) {
                            tp.x += level->m_random.nextInt(6) - level->m_random.nextInt(6);
                            tp.y += level->m_random.nextInt(1) - level->m_random.nextInt(1);
                            tp.z += level->m_random.nextInt(6) - level->m_random.nextInt(6);

                            if (isSpawnPositionOk(category, level, tp)) {
                                Vec3 pPos(tp.x + 0.5, tp.y, tp.z + 0.5);

                                if (!level->getNearestPlayer(pPos, 24.0f)) {
                                    Vec3 dPos = pPos - level->getSharedSpawnPos();
                                    if (dPos.lengthSqr() >= 576.0f) {
                                        auto entity = type->newEntity(level);
                                        if (!entity) break;
                                        auto mob = std::dynamic_pointer_cast<Mob>(entity);

                                        mob->moveTo(pPos, Vec2(level->m_random.nextFloat() * 360.0f, 0.0f));
                                        if (mob->canSpawn()) {
                                            ++spawned;
                                            level->addEntity(mob);
                                            finalizeMobSettings(mob.get(), level, pPos);
                                            if (spawned >= mob->getMaxSpawnClusterSize())
                                                break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    totalSpawned += spawned;
                }
            }
        }

        return totalSpawned;
    }

private:
    static bool isSpawnPositionOk(const MobCategory* category, Level* level, const TilePos& pos) {
        if (category->getSpawnPositionMaterial() == Material::water) {
            return level->getMaterial(pos)->isLiquid() && !level->isSolidTile(pos.above());
        } else {
            return level->isSolidTile(pos.below()) && !level->isSolidTile(pos) &&
                   !level->getMaterial(pos)->isLiquid() && !level->isSolidTile(pos.above());
        }
    }

    static void finalizeMobSettings(Mob* mob, Level* level, const Vec3& vec) {
        //if (mob->getType() == *EntityType::spider) {
        //    if (level->m_random.nextInt(100) == 0) {
        //        auto skeleton = std::make_shared<Skeleton>(level);
        //        skeleton->moveTo(vec, Vec2(mob->m_rot.y, 0.0f));
        //        level->addEntity(skeleton);
        //        skeleton->ride(mob);
        //    }
        //} else 
        if (mob->getType() == *EntityType::sheep) {
            ((Sheep*)mob)->setColor(Sheep::getSheepColor(level->m_random));
        }
    }
};


std::unordered_set<ChunkPos> MobSpawner::chunksToPoll;
