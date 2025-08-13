#include "Squid.hpp"
#include "common/Utils.hpp"
#include "world/entity/Player.hpp"

Squid::Squid(Level* pLevel) : WaterAnimal(pLevel)
{
	m_pType = EntityType::squid;
	m_texture = "mob/squid.png";
	setSize(0.95f, 0.95f);
    m_tentacleSpeed = 1.0F / (m_random.nextFloat() + 1.0F) * 0.2F;
}

void Squid::dropDeathLoot()
{
    int itemId = getDeathLoot();
    if (itemId > 0)
    {
        int desiredItemCount = m_random.nextInt(3) + 1;
        for (int itemCount = 0; itemCount < desiredItemCount; itemCount++)
        {
            spawnAtLocation(itemId, 1);
        }
    }
}

bool Squid::wasInWater()
{
	AABB hit = m_hitbox;
	hit.grow(0.0, -0.6, 0.0);
	return m_pLevel->checkAndHandleWater(hit, Material::water, this);
}

void Squid::aiStep()
{
    WaterAnimal::aiStep();
    m_xBodyRotO = m_xBodyRot;
    m_zBodyRotO = m_zBodyRot;
    m_oldTentacleMovement = m_tentacleMovement;
    m_oldTentacleAngle = m_tentacleAngle;
    m_tentacleMovement += m_tentacleSpeed;
    if (m_tentacleMovement > 2 * M_PI) {
        m_tentacleMovement -= 2 * M_PI;
        if (m_random.nextInt(10) == 0) {
            m_tentacleSpeed = 1.0F / (m_random.nextFloat() + 1.0F) * 0.2F;
        }
    }

    if (wasInWater())
    {
        float var1;
        if (m_tentacleMovement < M_PI)
        {
            var1 = m_tentacleMovement / M_PI;
            m_tentacleAngle = Mth::sin(var1 * var1 * M_PI) * M_PI * 0.25F;
            if (var1 > 0.75)
            {
                m_speed = 1.0F;
                m_rotateSpeed = 1.0F;
            }
            else
                m_rotateSpeed *= 0.8F;
        }
        else
        {
            m_tentacleAngle = 0.0F;
            m_speed *= 0.9F;
            m_rotateSpeed *= 0.99F;
        }

        if (!m_bInterpolateOnly)
        {
            m_vel.x = (m_tPos.x * m_speed);
            m_vel.y = (m_tPos.y * m_speed);
            m_vel.z = (m_tPos.z * m_speed);
        }

        var1 = Mth::sqrt(m_vel.x * m_vel.x + m_vel.z * m_vel.z);
        m_yBodyRot += (-(Mth::atan2(m_vel.x, m_vel.z)) * 180.0F / M_PI - m_yBodyRot) * 0.1F;
        m_rot.y = m_yBodyRot;
        m_zBodyRot += M_PI * m_rotateSpeed * 1.5F;
        m_xBodyRot += (-(Mth::atan2(var1, m_vel.y)) * 180.0F / M_PI - m_xBodyRot) * 0.1F;
    }
    else
    {
        m_tentacleAngle = Mth::abs(Mth::sin(m_tentacleMovement)) * M_PI * 0.25F;
        if (!m_bInterpolateOnly)
        {
            m_vel.x = 0.0;
            m_vel.y -= 0.08;
            m_vel.y *= 0.98;
            m_vel.z = 0.0;
        }

        m_xBodyRot = (m_xBodyRot + (-90.0F - m_xBodyRot) * 0.02);
    }
}

void Squid::travel(const Vec2&)
{
    move(m_vel);
}

void Squid::updateAi()
{
    if (m_random.nextInt(50) == 0 || !m_bWasInWater || m_tPos == Vec3f::ZERO)
    {
        float var1 = m_random.nextFloat() * M_PI * 2.0F;
        m_tPos.x = Mth::cos(var1) * 0.2F;
        m_tPos.y = -0.1F + m_random.nextFloat() * 0.2F;
        m_tPos.z = Mth::sin(var1) * 0.2F;
    }
    checkDespawn();
}
