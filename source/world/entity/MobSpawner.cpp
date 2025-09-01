#include "MobSpawner.hpp"
#include "Skeleton.hpp"
#include <world/tile/BedTile.hpp>
#include "Sheep.hpp"

std::unordered_set<ChunkPos> MobSpawner::chunksToPoll;
std::vector<EntityType*> MobSpawner::nightmareEntities;

int MobSpawner::tick(Level* level, bool allowHostile, bool allowFriendly)
{
    if (!allowHostile && !allowFriendly)
        return 0;

    chunksToPoll.clear();

    for (auto& player : level->m_players)
    {
        int cx = Mth::floor(player->m_pos.x / 16.0f);
        int cz = Mth::floor(player->m_pos.z / 16.0f);

        for (int dx = -8; dx <= 8; ++dx) {
            for (int dz = -8; dz <= 8; ++dz) {
                chunksToPoll.insert(ChunkPos(cx + dx, cz + dz));
            }
        }
    }

    int totalSpawned = 0;

    for (auto category : MobCategory::values)
    {
        if ((!category->isFriendly() || allowFriendly) && (category->isFriendly() || allowHostile) &&
            level->countWithCategory(category->getBaseType().getCategoryMask()) <= (category->getMaxInstancesPerChunk() * (int)chunksToPoll.size() / 256))
        {

            for (const ChunkPos& pos : chunksToPoll)
            {
                Biome* biome = level->getBiomeSource()->getBiome(pos);
                auto& spawnList = biome->getMobs(category);
                if (spawnList.empty()) continue;

                EntityType* type = spawnList.begin()->first;

                int spawnWeight = 0;

                for (auto it = spawnList.begin(); it != spawnList.end(); spawnWeight += it->second, ++it)
                {
                }


                int randomRate = level->m_random.nextInt(spawnWeight);

                for (auto it = spawnList.begin(); it != spawnList.end(); ++it)
                {
                    randomRate -= it->second;
                    if (randomRate < 0) {
                        type = it->first;
                        break;
                    }
                }

                if (!type->getCategory().contains(EntityCategories::MOB)) continue;

                int idx = level->m_random.nextInt((int)spawnList.size());
                TilePos tpos = getRandomPosWithin(level, pos.x * 16, pos.z * 16);

                if (level->isNormalTile(tpos)) continue;
                if (level->getMaterial(tpos) != category->getSpawnPositionMaterial()) continue;

                int spawned = 0;
                for (int i = 0; i < 3; ++i)
                {
                    TilePos tp(tpos);

                    if (spawned == -1)
                        break;

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
                                        {
                                            totalSpawned += spawned;
                                            spawned = -1;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                if (spawned != -1)
                    totalSpawned += spawned;
            }
        }
    }

    return totalSpawned;
}

bool MobSpawner::spawnNightmare(Level* level, const std::vector<std::shared_ptr<Player>>& players)
{
    PathFinder path(level);

    for (auto& player : players)
    {
        for (int tries = 0; tries < 20; ++tries) {
            TilePos tp(player->m_pos);
            tp.x += level->m_random.nextInt(32) - level->m_random.nextInt(32);
            tp.z += level->m_random.nextInt(32) - level->m_random.nextInt(32);
            tp.y += level->m_random.nextInt(16) - level->m_random.nextInt(16);
            tp.y = Mth::clamp(tp.y, 1, 128);

            EntityType* type = nightmareEntities[level->m_random.nextInt(nightmareEntities.size())];

            int newY;
            for (newY = tp.y; newY > 2 && !level->isNormalTile(TilePos(tp.x, newY, tp.z)); --newY) {
            }

            while (!isSpawnPositionOk(MobCategory::monster, level, TilePos(tp.x, newY, tp.z)) && newY < tp.y + 16 && newY < 128)
                ++newY;
     
            if (newY < tp.y + 16 && newY < 128) {
                std::shared_ptr<Mob> mob = std::dynamic_pointer_cast<Mob>(type->newEntity(level));
                if (!mob) break;

                mob->moveTo(Vec3(tp.x + 0.5, newY, tp.z + 0.5), Vec2(level->m_random.nextFloat() * 360.0F, 0.0F));

                if (mob && mob->canSpawn())
                {
                    Path p;
                    if (path.findPath(p, mob.get(), player.get(), 32.0F))
                    {
                        Vec3 pos = p.currentPos();
                        if (Mth::abs(pos.x - player->m_pos.x) < 1.5 && Mth::abs(pos.y - player->m_pos.y) < 1.5 && Mth::abs(pos.z - player->m_pos.z) < 1.5)
                        {
                            TilePos playerPos = player->m_pos;
                            TilePos var20 = BedTile::getRespawnTilePos(level, playerPos, 1);
                            if (var20 == playerPos)
                            {
                                var20.y += 1;
                            }

                            mob->moveTo(Vec3(var20.x + 0.5, var20.y, var20.z + 0.5));
                            level->addEntity(mob);
                            finalizeMobSettings(mob.get(), level, mob->m_pos);
                            player->wake(true, false, false);
                            mob->playAmbientSound();
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

void MobSpawner::finalizeMobSettings(Mob* mob, Level* level, const Vec3& vec)
{
    if (mob->getType() == EntityType::spider)
    {
        if (level->m_random.nextInt(100) == 0) {
            auto skeleton = std::make_shared<Skeleton>(level);
            skeleton->moveTo(vec, Vec2(mob->m_rot.y, 0.0f));
            level->addEntity(skeleton);
            skeleton->ride(mob->shared_from_this());
        }
    }
    else
        if (mob->getType() == EntityType::sheep) {
            ((Sheep*)mob)->setColor(Sheep::getSheepColor(level->m_random));
        }
}
