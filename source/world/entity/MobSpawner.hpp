#pragma once

#include <unordered_set>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <type_traits>
#include <world/level/levelgen/chunk/ChunkPos.hpp>
#include <world/level/Level.hpp>
#include "EntityType.hpp"

class MobSpawner {
public:
    static constexpr int MIN_SPAWN_DISTANCE = 24;
    static std::unordered_set<ChunkPos> chunksToPoll;
    static std::vector<EntityType*> nightmareEntities;

    static TilePos getRandomPosWithin(Level* level, int x, int z) {
        int px = x + level->m_random.nextInt(16);
        int py = level->m_random.nextInt(128);
        int pz = z + level->m_random.nextInt(16);
        return TilePos(px, py, pz);
    }

    static int tick(Level* level, bool allowHostile, bool allowFriendly);

    static bool spawnNightmare(Level* level, const std::vector<std::shared_ptr<Player>>& players);

private:
    static bool isSpawnPositionOk(const MobCategory* category, Level* level, const TilePos& pos) {
        if (category->getSpawnPositionMaterial() == Material::water) {
            return level->getMaterial(pos)->isLiquid() && !level->isSolidTile(pos.above());
        } else {
            return level->isSolidTile(pos.below()) && !level->isSolidTile(pos) &&
                   !level->getMaterial(pos)->isLiquid() && !level->isSolidTile(pos.above());
        }
    }

    static void finalizeMobSettings(Mob* mob, Level* level, const Vec3& vec);
};
