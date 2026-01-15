#include "FishingHook.hpp"
#include "world/level/Level.hpp"

FishingHook::FishingHook(Level* level) : Entity(level)
{
	m_pType = EntityType::fishingHook;
	setSize(0.25f, 0.25f);
    m_heightOffset = 0;
    m_bIgnoreFrustum = true;
}

FishingHook::FishingHook(Level* level, const Vec3& pos) : FishingHook(level)
{
	setPos(pos);
}

FishingHook::FishingHook(Level* level, std::shared_ptr<Player> mob) : Entity(level), m_owner(mob)
{
    m_pType = EntityType::fishingHook;
	setSize(0.25f, 0.25f);
	moveTo(mob->m_pos.add(0, 1.62, 0), mob->m_rot);
	m_pos.x -= (Mth::cos(m_rot.y / 180.0F * M_PI) * 0.16F);
	m_pos.y -= 0.1;
	m_pos.z -= (Mth::sin(m_rot.y / 180.0F * M_PI) * 0.16F);
	setPos(m_pos);
	m_heightOffset = 0.0F;
    m_bIgnoreFrustum = true;
	m_vel.x = (-Mth::sin(m_rot.y / 180.0F * M_PI) * Mth::cos(m_rot.x / 180.0F * M_PI)) * 0.4;
	m_vel.z = (Mth::cos(m_rot.y / 180.0F * M_PI) * Mth::cos(m_rot.x / 180.0F * M_PI)) * 0.4;
	m_vel.y = (-Mth::sin(m_rot.x / 180.0F * M_PI)) * 0.4;
	shoot(m_vel, 1.5F, 1.0F);
}

bool FishingHook::shouldRenderAtSqrDistance(real distSqr) const
{
    real var3 = m_hitbox.getSize() * 4.0;
    var3 *= 64.0;
    return distSqr < var3 * var3;
}

void FishingHook::shoot(const Vec3& vel, float pow, float randomPow)
{
	float var9 = Mth::sqrt(vel.lengthSqr());
	m_vel = vel;
	m_vel.x /= var9;
	m_vel.y /= var9;
	m_vel.z /= var9;
	m_vel.x += m_random.nextGaussian() * 0.0075 * randomPow;
	m_vel.y += m_random.nextGaussian() * 0.0075 * randomPow;
	m_vel.z += m_random.nextGaussian() * 0.0075 * randomPow;
	m_vel *= pow;
    float var7 = Mth::sqrt(m_vel.x * m_vel.x + m_vel.z * m_vel.z);
    m_rotPrev.y = m_rot.y = (Mth::atan2(m_vel.x, m_vel.z) * 180.0 / M_PI);
    m_rotPrev.x = m_rot.x = (Mth::atan2(m_vel.y, var7) * 180.0 / M_PI);
	m_life = 0;
}

void FishingHook::lerpTo(const Vec3& pos, const Vec2& rot, int steps)
{
    m_lPos = pos;
    m_lRot = rot;
    m_lSteps = steps;
    m_vel = m_lPosD;
}

void FishingHook::lerpMotion(const Vec3& vel)
{
    m_lPosD = m_vel = vel;
}

float FishingHook::getShadowHeightOffs() const
{
	return 0.0f;
}

int FishingHook::retrieve()
{
    uint8_t retrieveState = 0;
    if (m_hookedIn)
    {
        Vec3 diff = m_owner->m_pos - m_pos;
        m_hookedIn->m_vel.x += diff.x * 0.1;
        m_hookedIn->m_vel.y += diff.y * 0.1 + Mth::sqrt(diff.length()) * 0.08;
        m_hookedIn->m_vel.z += diff.z * 0.1;
        retrieveState = 3;
    }
    else if (m_nibble > 0)
    {
        auto var13 = std::make_shared<ItemEntity>(m_pLevel, m_pos, std::make_shared<ItemInstance>(Item::rawFish));
        Vec3 diff = m_owner->m_pos - m_pos;
        var13->m_vel.x = diff.x * 0.1;
        var13->m_vel.y = diff.y * 0.1 + Mth::sqrt(diff.length()) * 0.08;
        var13->m_vel.z = diff.z * 0.1;
        m_pLevel->addEntity(var13);
        m_owner->awardStat(Stats::fishCaught, 1);
        retrieveState = 1;
    }

    if (m_bInGround)
        retrieveState = 2;

    remove();
    m_owner->m_fishing = nullptr;
    return retrieveState;
}

void FishingHook::tick()
{
	Entity::tick();
    if (m_lSteps > 0) {
        Vec3 diff = m_pos + (m_lPos - m_pos) / m_lSteps;

        real var7;
        for (var7 = m_lRot.y - m_rot.y; var7 < -180.0; var7 += 360.0) {
        }

        while (var7 >= 180.0) {
            var7 -= 360.0;
        }

        m_rot.y = (m_rot.y + var7 / m_lSteps);
        m_rot.x = (m_rot.x + (m_lRot.x - m_rot.x) / m_lSteps);
        --m_lSteps;
        setPos(diff);
        setRot(m_rot);
    }
    else
    {
        if (!m_pLevel->m_bIsOnline)
        {
            auto var1 = m_owner->getSelectedItem();
            if (m_owner->m_bRemoved || !m_owner->isAlive() || !var1 || var1->getItem() != Item::fishingRod || distanceToSqr(m_owner.get()) > 1024.0)
            {
                remove();
                m_owner->m_fishing = nullptr;
                return;
            }

            if (m_hookedIn)
            {
                if (!m_hookedIn->m_bRemoved)
                {
                    m_pos.x = m_hookedIn->m_pos.x;
                    m_pos.y = m_hookedIn->m_hitbox.min.y + m_hookedIn->m_bbHeight * 0.8;
                    m_pos.z = m_hookedIn->m_pos.z;
                    return;
                }

                m_hookedIn = nullptr;
            }
        }

        if (m_shakeTime > 0)
            --m_shakeTime;

        if (m_bInGround)
        {
            if (m_pLevel->getTile(m_tilePos) == m_lastTile)
            {
                ++m_life;
                if (m_life == 1200)
                    remove();

                return;
            }

            m_bInGround = false;
            m_vel.x *= m_random.nextFloat() * 0.2F;
            m_vel.y *= m_random.nextFloat() * 0.2F;
            m_vel.z *= m_random.nextFloat() * 0.2F;
            m_life = 0;
            m_flightTime = 0;
        }
        else
        {
            ++m_flightTime;
        }

        HitResult var3 = m_pLevel->clip(m_pos, m_pos + m_vel);
        Vec3 var2 = m_pos + m_vel;
        if (var3.isHit())
            var2 = var3.m_hitPos;

        std::shared_ptr<Entity> var4 = nullptr;
        AABB checkBox = m_hitbox;
        checkBox.expand(m_vel.x, m_vel.y, m_vel.z);
        checkBox.grow(1.0, 1.0, 1.0);
        auto entities = m_pLevel->getEntities(this, checkBox);
        real var6 = 0.0;

        for (auto& var9 : entities)
        {
            if (var9->isPickable() && (var9 != m_owner || m_flightTime >= 5))
            {
                AABB var11 = var9->m_hitbox;
                var11.grow(0.3, 0.3, 0.3);
                HitResult var12 = var11.clip(m_pos, var2);
                if (var12.isHit())
                {
                    real var13 = m_pos.distanceTo(var12.m_hitPos);
                    if (var13 < var6 || var6 == 0.0)
                    {
                        var4 = var9;
                        var6 = var13;
                    }
                }
            }
        }

        if (var4)
            var3 = HitResult(var4);

        float var17;
        if (var3.isHit())
        {
            if (var3.m_pEnt)
            {
                if (var3.m_pEnt->hurt(m_owner.get(), 0))
                    m_hookedIn = var3.m_pEnt;
            }
            else
            {
                m_bInGround = true;
            }
        }

        if (!m_bInGround)
        {
            move(m_vel);
            var17 = Mth::sqrt(m_vel.x * m_vel.x + m_vel.z * m_vel.z);
            m_rot.y = (Mth::atan2(m_vel.x, m_vel.z) * 180.0 / M_PI);

            for (m_rot.x = (Mth::atan2(m_vel.y, var17) * 180.0 / M_PI); m_rot.x - m_rotPrev.x < -180.0F; m_rotPrev.x -= 360.0F) {
            }

            while (m_rot.x - m_rotPrev.x >= 180.0F)
                m_rotPrev.x += 360.0F;

            while (m_rot.y - m_rotPrev.y < -180.0F)
                m_rotPrev.y -= 360.0F;

            while (m_rot.y - m_rotPrev.y >= 180.0F)
                m_rotPrev.y += 360.0F;

            m_rot.x = m_rotPrev.x + (m_rot.x - m_rotPrev.x) * 0.2F;
            m_rot.y = m_rotPrev.y + (m_rot.y - m_rotPrev.y) * 0.2F;
            float var25 = 0.92F;
            if (m_bOnGround || m_bHorizontalCollision) {
                var25 = 0.5F;
            }

            real waterLevel = 0.0;

            for (int var28 = 0; var28 < 5; ++var28) {
                real var14 = m_hitbox.min.y + (m_hitbox.max.y - m_hitbox.min.y) * (var28 + 0) / 5 - 0.125 + 0.125;
                real var16 = m_hitbox.min.y + (m_hitbox.max.y - m_hitbox.min.y) * (var28 + 1) / 5 - 0.125 + 0.125;
                AABB var18 = AABB(m_hitbox.min.x, var14, m_hitbox.min.z, m_hitbox.max.x, var16, m_hitbox.max.z);
                if (m_pLevel->containsLiquid(var18, Material::water))
                    waterLevel += 1.0 / 5;
            }

            if (waterLevel > 0.0)
            {
                if (m_nibble > 0)
                    --m_nibble;
                else if (m_random.nextInt(500) == 0)
                {
                    m_nibble = m_random.nextInt(30) + 10;
                    m_vel.y -= 0.2;
                    m_pLevel->playSound(this, "random.splash", 0.25F, 1.0F + (m_random.nextFloat() - m_random.nextFloat()) * 0.4F);
                    float var29 = Mth::floor(m_hitbox.min.y);

                    float var15;
                    int var30;
                    float var31;
                    for (var30 = 0; var30 < 1.0F + m_bbWidth * 20.0F; ++var30) {
                        var15 = (m_random.nextFloat() * 2.0F - 1.0F) * m_bbWidth;
                        var31 = (m_random.nextFloat() * 2.0F - 1.0F) * m_bbWidth;
                        m_pLevel->addParticle("bubble", Vec3(m_pos.x + var15, (var29 + 1.0F), m_pos.z + var31), Vec3(m_vel.x, m_vel.y - (m_random.nextFloat() * 0.2F), m_vel.z));
                    }

                    for (var30 = 0; var30 < 1.0F + m_bbWidth * 20.0F; ++var30) {
                        var15 = (m_random.nextFloat() * 2.0F - 1.0F) * m_bbWidth;
                        var31 = (m_random.nextFloat() * 2.0F - 1.0F) * m_bbWidth;
                        m_pLevel->addParticle("splash", Vec3(m_pos.x + var15, (var29 + 1.0F), m_pos.z + var31), m_vel);
                    }
                }
            }

            if (m_nibble > 0)
                m_vel.y -= (m_random.nextFloat() * m_random.nextFloat() * m_random.nextFloat()) * 0.2;

            m_vel.y += 0.04 * (waterLevel * 2.0 - 1.0);
            if (waterLevel > 0.0)
            {
                var25 = (var25 * 0.9);
                m_vel.y *= 0.8;
            }
            m_vel *= var25;
            setPos(m_pos);
        }
    }
}

void FishingHook::addAdditionalSaveData(CompoundIO tag) 
{
    tag->putShort("xTile", m_tilePos.x);
    tag->putShort("yTile", m_tilePos.y);
    tag->putShort("zTile", m_tilePos.z);
    tag->putByte("inTile", m_lastTile);
    tag->putByte("shake", m_shakeTime);
    tag->putBoolean("inGround", m_bInGround);
}

void FishingHook::readAdditionalSaveData(CompoundIO tag) {
    m_tilePos.x = tag->getShort("xTile");
    m_tilePos.y = tag->getShort("yTile");
    m_tilePos.z = tag->getShort("zTile");
    m_lastTile = tag->getByte("inTile") & 255;
    m_shakeTime = tag->getByte("shake") & 255;
    m_bInGround = tag->getBoolean("inGround");
}