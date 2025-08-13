#pragma once

#include "TileEntity.hpp"
#include "world/level/Level.hpp"
#include <memory>
#include <string>
#include <world/entity/EntityIO.hpp>

class MobSpawnerTileEntity : public TileEntity {
public:
    static constexpr int MAX_DIST = 16;

    int spawnDelay = 20;
    std::string entityId = EntityType::pig->getName();
    double spin = 0.0;
    double oSpin = 0.0;

    MobSpawnerTileEntity() {
        m_pType = TileEntityType::mobSpawner;
        spawnDelay = 20;
    }

    std::string getEntityId() const {
        return entityId;
    }

    void setEntityId(const std::string& id) {
        entityId = id;
    }

    bool isNearPlayer() const {
        return m_pLevel && m_pLevel->getNearestPlayer(m_pos.center(), 16.0);
    }

    void tick() override {
        oSpin = spin;
        if (!m_pLevel || !isNearPlayer()) return;

        Vec3 particlePos(
            (float)m_pos.x + m_pLevel->m_random.nextFloat(),
            (float)m_pos.y + m_pLevel->m_random.nextFloat(),
            (float)m_pos.z + m_pLevel->m_random.nextFloat()
        );

        m_pLevel->addParticle("smoke", particlePos);
        m_pLevel->addParticle("flame", particlePos);

        spin += (1000.0f / ((float)spawnDelay + 200.0f));
        while (spin > 360.0) {
            spin -= 360.0;
            oSpin -= 360.0;
        }

        if (spawnDelay == -1)
            delay();

        if (spawnDelay > 0) {
            --spawnDelay;
            return;
        }

        const int maxSpawn = 4;

        for (int i = 0; i < maxSpawn; ++i) {
            std::shared_ptr<Mob> mob = std::dynamic_pointer_cast<Mob>(EntityIO::newEntity(entityId, m_pLevel));
            if (!mob) return;

            AABB box(m_pos.x, m_pos.y, m_pos.z, m_pos.x, m_pos.y, m_pos.z);
            box.grow(8.0, 4.0, 8.0);
            int existing = m_pLevel->getEntitiesOfCategory(mob->getCategory().getCategoryMask(), box).size();

            if (existing >= 6) {
                delay();
                return;
            }

            Vec3 spawnPos(
                m_pos.x + (m_pLevel->m_random.nextDouble() - m_pLevel->m_random.nextDouble()) * 4.0,
                m_pos.y + m_pLevel->m_random.nextInt(3) - 1,
                m_pos.z + (m_pLevel->m_random.nextDouble() - m_pLevel->m_random.nextDouble()) * 4.0
            );

            mob->moveTo(spawnPos, Vec2(m_pLevel->m_random.nextFloat() * 360.0f, 0.0f));

            if (mob->canSpawn()) {
                m_pLevel->addEntity(mob);

                for (int j = 0; j < 20; ++j) {
                    Vec3 particleSpawn(
                        m_pos.x + 0.5 + (m_pLevel->m_random.nextFloat() - 0.5) * 2.0,
                        m_pos.y + 0.5 + (m_pLevel->m_random.nextFloat() - 0.5) * 2.0,
                        m_pos.z + 0.5 + (m_pLevel->m_random.nextFloat() - 0.5) * 2.0
                    );
                    m_pLevel->addParticle("smoke", particleSpawn, Vec3(0, 0, 0));
                    m_pLevel->addParticle("flame", particleSpawn, Vec3(0, 0, 0));
                }

                mob->spawnAnim();
                delay();
            }
        }

        TileEntity::tick();
    }

    void delay() {
        if (m_pLevel)
            spawnDelay = 200 + m_pLevel->m_random.nextInt(600);
    }

    void load(const std::shared_ptr<CompoundTag> tag) override {
        TileEntity::load(tag);
        entityId = tag->getString("EntityId");
        spawnDelay = tag->getShort("Delay");
    }

    void save(std::shared_ptr<CompoundTag> tag) override {
        TileEntity::save(tag);
        tag->putString("EntityId", entityId);
        tag->putShort("Delay", static_cast<short>(spawnDelay));
    }
};
