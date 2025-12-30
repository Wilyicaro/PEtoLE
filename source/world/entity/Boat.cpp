#include "Boat.hpp"
#include "Player.hpp"
#include "world/level/Level.hpp"
#include <world/tile/RailTile.hpp>

Boat::Boat(Level* level) : Entity(level)
{
    m_pType = EntityType::boat;
    m_lPos = Vec3::ZERO;
    m_lRot = Vec2::ZERO;
    m_lPosD = Vec3::ZERO;
    m_lSteps = 0;
    m_damage = 0;
    m_hurtTime = 0;
    m_hurtDir = 1;
    m_bBlocksBuilding = true;
    setSize(1.5f, 0.6f);
    m_heightOffset = m_bbHeight / 2.0f;
    m_bMakeStepSound = false;
}

Boat::Boat(Level* level, const Vec3& pos) : Boat(level)
{
	setPos(Vec3(pos.x, pos.y, pos.z));
	m_vel = Vec3::ZERO;
	m_oPos = Vec3::ZERO;
}


const AABB* Boat::getCollideBox() const
{
    return &m_hitbox;
}

AABB* Boat::getCollideAgainstBox(Entity* ent) const
{
    return &ent->m_hitbox;
}

bool Boat::interact(Player* player)
{
    if (m_pRider && m_pRider->isPlayer() && m_pRider.get() != player)
        return true;

    if (!m_pLevel->m_bIsOnline)
        player->ride(shared_from_this());

    return true;
}

void Boat::tick()
{
    if (m_hurtTime > 0)
        --m_hurtTime;

    if (m_damage > 0)
        --m_damage;


    m_oPos = m_pos;
    real var2 = 0.0;

    for (int var4 = 0; var4 < 5; ++var4)
    {
        real var5 = m_hitbox.min.y + (m_hitbox.max.y - m_hitbox.min.y) * (var4 + 0) / 5 - 0.125;
        real var7 = m_hitbox.min.y + (m_hitbox.max.y - m_hitbox.min.y) * (var4 + 1) / 5 - 0.125;
        AABB var9 = AABB(m_hitbox.min.x, var5, m_hitbox.min.z, m_hitbox.max.x, var5, m_hitbox.max.z);
        if (m_pLevel->containsLiquid(var9, Material::water))
        {
            var2 += 1.0 / 5;
        }
    }

    real var6;
    real var8;
    float var10;
    real var23;
    if (m_pLevel->m_bIsOnline)
    {
        if (m_lSteps > 0) {
            Vec3 newPos = m_pos + (m_lPos - m_pos) / m_lSteps;

            for (var10 = m_lRot.y - m_rot.y; var10 < -180.0; var10 += 360.0) {
            }

            while (var10 >= 180.0)
                var10 -= 360.0;

            m_rot.y = m_rot.y + var10 / m_lSteps;
            m_rot.x = m_rot.x + (m_lRot.x - m_rot.x) / m_lSteps;
            --m_lSteps;
            setPos(newPos);
            setRot(m_rot);
        }
        else
        {
            setPos(m_pos + m_vel);
            if (m_bOnGround) {
                m_vel.x *= 0.5;
                m_vel.y *= 0.5;
                m_vel.z *= 0.5;
            }

            m_vel.x *= 0.99;
            m_vel.y *= 0.95;
            m_vel.z *= 0.99;
        }

    }
    else {
        var23 = var2 * 2.0 - 1.0;
        m_vel.y += 0.04 * var23;
        if (m_pRider) {
            m_vel.x += m_pRider->m_vel.x * 0.2;
            m_vel.z += m_pRider->m_vel.z * 0.2;
        }

        var6 = 0.4;
        if (m_vel.x < -var6)
            m_vel.x = -var6;

        if (m_vel.x > var6)
            m_vel.x = var6;

        if (m_vel.z < -var6)
            m_vel.z = -var6;

        if (m_vel.z > var6)
            m_vel.z = var6;

        if (m_bOnGround)
        {
            m_vel *= 0.5;
        }

        move(m_vel);
        var8 = Mth::sqrt(m_vel.x * m_vel.x + m_vel.z * m_vel.z);
        real var12;
        if (var8 > 0.15) {
            var10 = Mth::cos(m_rot.y * M_PI / 180.0);
            var12 = Mth::sin(m_rot.x * M_PI / 180.0);

            for (int var14 = 0; var14 < 1.0 + var8 * 60.0; ++var14) {
                real var15 = (m_random.nextFloat() * 2.0F - 1.0F);
                real var17 = (m_random.nextInt(2) * 2 - 1) * 0.7;
                real var19;
                real var21;
                if (m_random.nextBoolean()) {
                    var19 = m_pos.x - var10 * var15 * 0.8 + var12 * var17;
                    var21 = m_pos.z - var12 * var15 * 0.8 - var10 * var17;
                    m_pLevel->addParticle("splash", Vec3(var19, m_pos.y - 0.125, var21), m_vel);
                }
                else {
                    var19 = m_pos.x + var10 + var12 * var15 * 0.7;
                    var21 = m_pos.z + var12 - var10 * var15 * 0.7;
                    m_pLevel->addParticle("splash", Vec3(var19, m_pos.y - 0.125, var21), m_vel);
                }
            }
        }

        if (m_bHorizontalCollision && var8 > 0.15) {
            if (!m_pLevel->m_bIsOnline) {
                remove();

                int var24;
                for (var24 = 0; var24 < 3; ++var24) {
                    spawnAtLocation(Tile::wood->m_ID, 1, 0.0F);
                }

                for (var24 = 0; var24 < 2; ++var24) {
                    spawnAtLocation(Item::stick->m_itemID, 1, 0.0F);
                }
            }
        }
        else {
            m_vel.x *= 0.99;
            m_vel.y *= 0.95;
            m_vel.z *= 0.99;
        }

        m_rot.x = 0.0F;
        var10 = m_rot.y;
        var12 = m_oPos.x - m_pos.x;
        real var25 = m_oPos.z - m_pos.z;
        if (var12 * var12 + var25 * var25 > 0.001) {
            var10 = Mth::atan2(var25, var12) * 180.0 / M_PI;
        }

        real var16;
        for (var16 = var10 - m_rot.y; var16 >= 180.0; var16 -= 360.0) {
        }

        while (var16 < -180.0)
            var16 += 360.0;

        if (var16 > 20.0)
            var16 = 20.0;

        if (var16 < -20.0)
            var16 = -20.0;

        m_rot.y = m_rot.y + var16;
        setRot(m_rot);
        AABB hit = m_hitbox;
        hit.grow(0.2, 0.0, 0.2);
        auto var18 = m_pLevel->getEntities(this, hit);
        if (var18.size() > 0) {
            for (auto& var20 : var18) {
                if (var20 != m_pRider && var20->isPushable() && var20->getType() == EntityType::boat) {
                    var20->push(this);
                }
            }
        }

        if (m_pRider && m_pRider->m_bRemoved)
            m_pRider = nullptr;
    }
}

real Boat::getRideHeight()
{
	return m_heightOffset - 0.3;
}

void Boat::positionRider() {
    if (m_pRider)
    {
        m_pRider->setPos(Vec3(m_pos.x + Mth::cos(m_rot.y * M_PI / 180.0) * 0.4, m_pos.y + getRideHeight() + m_pRider->getRidingHeight(), m_pos.z + Mth::sin(m_rot.y * M_PI / 180.0) * 0.4));
    }
}

bool Boat::hurt(Entity*, int mul)
{
    m_hurtDir = -m_hurtDir;
    m_hurtTime = 10;
    markHurt();
    if (m_damage < 40)
        m_damage += mul * 10;

    if (!m_pLevel->m_bIsOnline && !m_bRemoved) 
    {
        if (m_damage > 40) 
        {
            for (int i = 0; i < 3; ++i) {
                spawnAtLocation(Tile::wood->m_ID, 1, 0.0F);
            }

            for (int i = 0; i < 2; ++i) {
                spawnAtLocation(Item::stick->m_itemID, 1, 0.0F);
            }

            remove();
        }
        return true;
    }
    else
        return true;
}

void Boat::animateHurt()
{
    m_hurtDir = -m_hurtDir;
    m_hurtTime = 10;
    if (m_damage < 40)
        m_damage += m_damage * 10;
}

void Boat::addAdditionalSaveData(CompoundIO tag)
{
}

void Boat::readAdditionalSaveData(CompoundIO tag)
{
}

void Boat::lerpTo(const Vec3& pos, const Vec2& rot, int steps)
{
    m_lPos = pos;
    m_lRot = rot;
    m_lSteps = steps + 4;
    m_vel = m_lPosD;
}

void Boat::lerpMotion(const Vec3& motion) 
{
    m_lPosD = m_vel = motion;
}