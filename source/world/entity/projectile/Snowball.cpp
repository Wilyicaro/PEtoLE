#include "Snowball.hpp"
#include "world/level/Level.hpp"

Snowball::Snowball(Level* level) : Entity(level)
{
	m_pType = EntityType::snowball;
	setSize(0.25f, 0.25f);
}

Snowball::Snowball(Level* level, const Vec3& pos) : Entity(level)
{
    m_pType = EntityType::snowball;
	setSize(0.25f, 0.25f);
	setPos(pos);
	m_heightOffset = 0;
}

Snowball::Snowball(Level* level, std::shared_ptr<Mob> mob) : Entity(level), m_owner(mob)
{
    m_pType = EntityType::snowball;
	setSize(0.25f, 0.25f);
	moveTo(mob->m_pos.add(0, mob->getHeadHeight(), 0), mob->m_rot);
	m_pos.x -= (Mth::cos(m_rot.y / 180.0F * M_PI) * 0.16F);
	m_pos.y -= 0.1;
	m_pos.z -= (Mth::sin(m_rot.y / 180.0F * M_PI) * 0.16F);
	setPos(m_pos);
	m_heightOffset = 0.0F;
    m_vel.x = (-Mth::sin(m_rot.y / 180.0F * M_PI) * Mth::cos(m_rot.x / 180.0F * M_PI)) * 0.4;
    m_vel.z = (Mth::cos(m_rot.y / 180.0F * M_PI) * Mth::cos(m_rot.x / 180.0F * M_PI)) * 0.4;
    m_vel.y = (-Mth::sin(m_rot.x / 180.0F * M_PI)) * 0.4;
	shoot(m_vel, 1.5F, 1.0F);
}

bool Snowball::shouldRenderAtSqrDistance(real distSqr) const
{
    real var3 = m_hitbox.getSize() * 4.0;
    var3 *= 64.0;
    return distSqr < var3 * var3;
}

void Snowball::shoot(const Vec3& vel, float pow, float randomPow)
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

void Snowball::lerpMotion(const Vec3& vel)
{
	m_vel.x = vel.x;
	m_vel.y = vel.y;
	m_vel.z = vel.z;
	if (m_rotPrev.x == 0.0F && m_rotPrev.y == 0.0F) {
		float var7 = Mth::sqrt(vel.x * vel.x + vel.z * vel.z);
		m_rotPrev.y = m_rot.y = (Mth::atan2(vel.x, vel.z) * 180.0 / M_PI);
		m_rotPrev.x = m_rot.x = (Mth::atan2(vel.y, var7) * 180.0 / M_PI);
        moveTo(m_pos, m_rot);
        m_life = 0;
	}
}

float Snowball::getShadowHeightOffs() const
{
	return 0.0f;
}

void Snowball::onHit()
{
    for (int i = 0; i < 8; ++i)
        m_pLevel->addParticle("snowballpoof", m_pos);
}

void Snowball::tick()
{
    Entity::tick();
    if (m_shakeTime > 0)
        --m_shakeTime;

    if (m_bInGround)
    {
        int var15 = m_pLevel->getTile(m_tilePos);
        if (var15 == m_lastTile) {
            ++m_life;
            if (m_life == 1200) {
                remove();
            }

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

    if (!m_pLevel->m_bIsOnline)
    {
        for (auto& var9 : entities)
        {
            if (var9->isPickable() && (var9 != m_owner || m_flightTime >= 5))
            {
                float var10 = 0.3F;
                AABB var11 = var9->m_hitbox;
                var11.grow(var10, var10, var10);
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
    }

    float var17;
    if (var3.isHit())
    {
        if (var3.m_pEnt && var3.m_pEnt->hurt(m_owner.get(), 0))
        {
        }

        onHit();

        remove();
    }

    m_pos += m_vel;
    var17 = Mth::sqrt(m_vel.x * m_vel.x + m_vel.z * m_vel.z);
    m_rot.y = (float)(Mth::atan2(m_vel.x, m_vel.z) * 180.0 / M_PI);

    for (m_rot.x = (float)(Mth::atan2(m_vel.y, var17) * 180.0 / M_PI); m_rot.x - m_rotPrev.x < -180.0F; m_rotPrev.x -= 360.0F) {
    }

    while (m_rot.x - m_rotPrev.x >= 180.0F)
        m_rotPrev.x += 360.0F;

    while (m_rot.y - m_rotPrev.y < -180.0F)
        m_rotPrev.y -= 360.0F;

    while (m_rot.y - m_rotPrev.y >= 180.0F)
        m_rotPrev.y += 360.0F;

    m_rot.x = m_rotPrev.x + (m_rot.x - m_rotPrev.x) * 0.2F;
    m_rot.y = m_rotPrev.y + (m_rot.y - m_rotPrev.y) * 0.2F;
    float var18 = 0.99F;
    if (wasInWater())
    {
        for (int var19 = 0; var19 < 4; ++var19)
            m_pLevel->addParticle("bubble", m_pos - (m_vel * 0.25), m_vel);

        var18 = 0.8F;
    }

    if (!(m_power == Vec3::ZERO))
        m_vel += m_power;
    m_vel *= var18;
    m_vel.y -= 0.03;
    setPos(m_pos);
}

void Snowball::addAdditionalSaveData(std::shared_ptr<CompoundTag> tag) 
{
    tag->putShort("xTile", m_tilePos.x);
    tag->putShort("yTile", m_tilePos.y);
    tag->putShort("zTile", m_tilePos.z);
    tag->putByte("inTile", m_lastTile);
    tag->putByte("shake", m_shakeTime);
    tag->putBoolean("inGround", m_bInGround);
}

void Snowball::readAdditionalSaveData(std::shared_ptr<CompoundTag> tag) {
    m_tilePos.x = tag->getShort("xTile");
    m_tilePos.y = tag->getShort("yTile");
    m_tilePos.z = tag->getShort("zTile");
    m_lastTile = tag->getByte("inTile") & 255;
    m_shakeTime = tag->getByte("shake") & 255;
    m_bInGround = tag->getBoolean("inGround");
}
