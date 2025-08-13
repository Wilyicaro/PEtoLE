#include "world/entity/Ghast.hpp"
#include "projectile/Fireball.hpp"

Ghast::Ghast(Level* pLevel) : FlyingMob(pLevel), m_floatDuration(0), m_retargetTime(0), m_oCharge(0), m_charge(0)
{
	m_pType = EntityType::ghast;
	m_texture = "mob/ghast.png";
	setSize(4.0f, 4.0f);
	m_bFireImmune = true;
}

void Ghast::updateAi()
{
    if (!m_pLevel->m_difficulty)
        remove();

    m_oCharge = m_charge;
    Vec3 diff = m_targetPos - m_pos;
    real dist = diff.length();
    if (dist < 1.0 || dist > 60.0)
    {
        m_targetPos.x = m_pos.x + (m_random.nextFloat() * 2.0F - 1.0F) * 16.0F;
        m_targetPos.y = m_pos.y + (m_random.nextFloat() * 2.0F - 1.0F) * 16.0F;
        m_targetPos.z = m_pos.z + (m_random.nextFloat() * 2.0F - 1.0F) * 16.0F;
    }
    

    if (m_floatDuration-- <= 0) {
        m_floatDuration += m_random.nextInt(5) + 2;
        if (canReach(m_targetPos, dist))
            m_vel += diff / dist * 0.1;
        else
            m_targetPos = m_pos;
    }

    if (m_target && m_target->m_bRemoved)
        m_target = nullptr;

    if (m_target || m_retargetTime-- <= 0)
    {
        m_target = m_pLevel->getNearestPlayer(this, 100.0);
        if (m_target)
            m_retargetTime = 20;
    }

    const real var9 = 64.0;
    if (m_target && m_target->distanceToSqr(this) < var9 * var9) {
        Vec3 firePos(
            m_target->m_pos.x - m_pos.x,
            m_target->m_hitbox.min.y + (m_target->m_bbHeight / 2.0F) - (m_pos.y + (m_bbHeight / 2.0F)),
            m_target->m_pos.z - m_pos.z);
        m_yBodyRot = m_rot.y = -(Mth::atan2(firePos.x, firePos.z)) * 180.0F / M_PI;
        if (canSee(m_target.get()))
        {
            if (m_charge == 10)
                m_pLevel->playSound(this, "mob.ghast.charge", getSoundVolume(), (m_random.nextFloat() - m_random.nextFloat()) * 0.2F + 1.0F);
            
            ++m_charge;
            if (m_charge == 20)
            {
                m_pLevel->playSound(this, "mob.ghast.fireball", getSoundVolume(), (m_random.nextFloat() - m_random.nextFloat()) * 0.2F + 1.0F);
                auto fireball = std::make_shared<Fireball>(m_pLevel, std::dynamic_pointer_cast<Mob>(shared_from_this()), firePos);
                const real var18 = 4.0;
                Vec3 var20 = getViewVector(1.0F);
                fireball->m_pos.x = m_pos.x + var20.x * var18;
                fireball->m_pos.y = m_pos.y + (m_bbHeight / 2.0F) + 0.5;
                fireball->m_pos.z = m_pos.z + var20.z * var18;
                m_pLevel->addEntity(fireball);
                m_charge = -40;
            }
        }
        else if (m_charge > 0)
            --m_charge;
    }
    else
    {
        m_yBodyRot = m_rot.y = -((float)Mth::atan2(m_vel.x, m_vel.z)) * 180.0F / 3.1415927F;
        if (m_charge > 0)
            --m_charge;
    }

    m_texture = m_charge > 10 ? "mob/ghast_fire.png" : "mob/ghast.png";
}

bool Ghast::canSpawn()
{
    return m_random.nextInt(20) == 0 && FlyingMob::canSpawn() && m_pLevel->m_difficulty > 0;
}

bool Ghast::canReach(const Vec3& pos, real div)
{
    Vec3 diff = (pos - m_pos) / div;
    AABB var15 = m_hitbox;

    for (int var16 = 1; var16 < div; ++var16)
    {
        var15.move(diff);
        if (m_pLevel->getCubes(this, var15)->size() > 0)
            return false;
    }

    return true;
}
