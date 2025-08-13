#include "world/entity/FlyingMob.hpp"

FlyingMob::FlyingMob(Level* pLevel) : Mob(pLevel)
{
}

void FlyingMob::causeFallDamage(float)
{
}

void FlyingMob::travel(const Vec2& vec)
{
    if (isInWater())
    {
        moveRelative(Vec3(vec.y, 0.02, vec.x));
        move(m_vel);
        m_vel *= 0.8;
    }
    else if (isInLava())
    {
        moveRelative(Vec3(vec.y, 0.02, vec.x));
        move(m_vel);
        m_vel *= 0.5;
    }
    else
    {
        float var3 = 0.91F;
        if (m_bOnGround)
        {
            var3 = 0.546F;
            int var4 = m_pLevel->getTile(TilePos(m_pos.x, m_hitbox.min.y - 1, m_pos.z));
            if (var4 > 0)
                var3 = Tile::tiles[var4]->m_friction * 0.91F;
        }

        float var8 = 0.16277136F / (var3 * var3 * var3);
        moveRelative(Vec3(vec.y, m_bOnGround ? 0.1F * var8 : 0.02F, vec.x));
        var3 = 0.91F;
        if (m_bOnGround)
        {
            var3 = 0.546;
            int var5 = m_pLevel->getTile(TilePos(m_pos.x, m_hitbox.min.y - 1, m_pos.z));
            if (var5 > 0)
                var3 = Tile::tiles[var5]->m_friction * 0.91F;
        }

        move(m_vel);
        m_vel *= var3;
    }

    m_walkAnimPos = m_walkAnimSpeed;
    real var10 = m_pos.x - m_oPos.x;
    real var9 = m_pos.z - m_oPos.z;
    float var7 = Mth::sqrt(var10 * var10 + var9 * var9) * 4.0F;
    if (var7 > 1.0F)
        var7 = 1.0F;

    m_walkAnimSpeed += (var7 - m_walkAnimSpeed) * 0.4F;
    m_walkAnimPos += m_walkAnimSpeed;
}

bool FlyingMob::onLadder() const
{
	return false;
}
