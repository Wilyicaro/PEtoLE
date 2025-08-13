#include "Fireball.hpp"
#include "world/level/Level.hpp"

//For some reason, Fireballs also don't have a mapping in the EntityIO in the original
Fireball::Fireball(Level* level) : Snowball(level)
{
	m_pType = EntityType::fireball;
    setSize(1.0f, 1.0f);
}

Fireball::Fireball(Level* level, std::shared_ptr<Mob> mob, const Vec3& vel) : Snowball(level)
{
    m_pType = EntityType::fireball;
    setSize(1.0f, 1.0f);
    m_owner = mob;
    moveTo(mob->m_pos, mob->m_rot);
    setPos(m_pos);
    m_heightOffset = 0.0F;
    m_vel = Vec3::ZERO;
    Vec3 v(vel);
    v.x += m_random.nextGaussian() * 0.4;
    v.y += m_random.nextGaussian() * 0.4;
    v.z += m_random.nextGaussian() * 0.4;
    m_power = v / v.length() * 0.1; 
}

void Fireball::tick()
{
    m_fireTicks = 10;
    Snowball::tick();
}

void Fireball::onHit()
{
    if (!m_pLevel->m_bIsOnline)
        m_pLevel->explode(nullptr, m_pos, 1.0F, true);
}

bool Fireball::isPickable() const
{
    return true;
}

float Fireball::getPickRadius() const
{
    return 1.0f;
}

bool Fireball::hurt(Entity* ent, int dmg)
{
    markHurt();
    if (ent)
    {
        Vec3 ang = ent->getLookAngle();
        if (!(ang == Vec3::ZERO))
        {
            m_vel = ang;
            m_power = m_vel * 0.1;
        }
        return true;
    }
    else
        return false;
}
