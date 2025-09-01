#include "Minecart.hpp"
#include "Player.hpp"
#include "world/level/Level.hpp"
#include <world/tile/RailTile.hpp>

Minecart::Minecart(Level* level) : Entity(level), SimpleContainer(27, "gui.chestCart")
{
    m_pType = EntityType::minecart;
    m_lPos = Vec3::ZERO;
    m_lRot = Vec2::ZERO;
    m_lPosD = Vec3::ZERO;
    m_lSteps = 0;
    m_xPush = 0;
    m_zPush = 0;
    m_fuel = 0;
    m_type = 0;
    m_damage = 0;
    m_hurtTime = 0;
    m_hurtDir = 1;
    m_bFlipped = false;
    m_bBlocksBuilding = true;
    setSize(0.98f, 0.7f);
    m_heightOffset = m_bbHeight / 2.0f;
    m_bMakeStepSound = false;
}

Minecart::Minecart(Level* level, const Vec3& pos, int type) : Minecart(level)
{
	setPos(Vec3(pos.x, pos.y + m_heightOffset, pos.z));
	m_vel = Vec3::ZERO;
	m_oPos = Vec3::ZERO;
	m_type = type;
}

const AABB* Minecart::getCollideBox() const
{
    return nullptr;
}

AABB* Minecart::getCollideAgainstBox(Entity* ent) const
{
    return &ent->m_hitbox;
}

bool Minecart::interact(Player* player)
{
    if (m_type == 0)
    {
        if (m_pRider && m_pRider->isPlayer() && m_pRider.get() != player)
            return true;

        if (!m_pLevel->m_bIsOnline) 
            player->ride(shared_from_this());
    }
    else if (m_type == 1) 
    {
        if (!m_pLevel->m_bIsOnline) 
            player->openContainer(this);
    }
    else if (m_type == 2)
    {
        auto var2 = player->m_pInventory->getSelected();
        if (var2 && var2->m_itemID == Item::coal->m_itemID) 
        {
            if (--var2->m_count == 0)
                player->m_pInventory->setItem(player->m_pInventory->m_selected, nullptr);

            m_fuel += 1200;
        }

        m_xPush = m_pos.x - player->m_pos.x;
        m_zPush = m_pos.z - player->m_pos.z;
    }

    return true;
}

void Minecart::tick()
{
    if (m_hurtTime > 0) 
        --m_hurtTime;

    if (m_damage > 0)
        --m_damage;

    real var7;
    if (m_pLevel->m_bIsOnline && m_lSteps > 0) 
    {
        //@NOTE: this statement doesn't really make sense
        if (m_lSteps > 0) {
            Vec3 newPos = m_pos + (m_lPos - m_pos) / m_lSteps;

            for (var7 = m_lRot.y - m_rot.y; var7 < -180.0; var7 += 360.0) {
            }

            while (var7 >= 180.0) {
                var7 -= 360.0;
            }

            m_rot.y = m_rot.y + var7 / m_lSteps;
            m_rot.x = m_rot.x + (m_lRot.x - m_rot.x) / m_lSteps;
            --m_lSteps;
            setPos(newPos);
            setRot(m_rot);
        }
        else {
            setPos(m_pos);
            setRot(m_rot);
        }

    }
    else 
    {
        m_oPos = m_pos;
        m_vel.y -= 0.04;
        TilePos tp(m_pos);
        if (RailTile::isRail(m_pLevel->getTile(tp.below())))
            --tp.y;
     

        real var4 = 0.4;
        bool canPush = false;
        var7 = 0.0078125;
        int tile = m_pLevel->getTile(tp);
        if (RailTile::isRail(tile))
        {
            Vec3* var9 = getPos(m_pos);
            int data = m_pLevel->getData(tp);
            m_pos.y = tp.y;

            bool var49 = false;
            bool var13 = false;
            if (RailTile::isPoweredRail(Tile::tiles[tile])) 
            {
                var49 = (data & 8) != 0;
                var13 = !var49;
            }

            if (RailTile::isPowered(Tile::tiles[tile]))
                data &= 7;


            if (data >= 2 && data <= 5)
                m_pos.y = tp.y + 1;

            if (data == 2)
                m_vel.x -= var7;

            if (data == 3)
                m_vel.x += var7;

            if (data == 4)
                m_vel.z += var7;

            if (data == 5)
                m_vel.z -= var7;

            auto var11 = EXITS[data];
            real var12 = var11[1][0] - var11[0][0];
            real var14 = var11[1][2] - var11[0][2];
            real var16 = Mth::sqrt(var12 * var12 + var14 * var14);
            real var18 = m_vel.x * var12 + m_vel.z * var14;
            if (var18 < 0.0) {
                var12 = -var12;
                var14 = -var14;
            }

            real var20 = Mth::sqrt(m_vel.x * m_vel.x + m_vel.z * m_vel.z);
            m_vel.x = var20 * var12 / var16;
            m_vel.z = var20 * var14 / var16;
            real var22;
            if (var13) 
            {
                var22 = Mth::sqrt(m_vel.x * m_vel.x + m_vel.z * m_vel.z);
                if (var22 < 0.03)
                {
                    m_vel *= 0;
                }
                else
                {
                    m_vel.x *= 0.5;
                    m_vel.y *= 0.0;
                    m_vel.z *= 0.5;
                }
            }
            var22 = 0.0;
            real var24 = tp.x + 0.5 + var11[0][0] * 0.5;
            real var26 = tp.z + 0.5 + var11[0][2] * 0.5;
            real var28 = tp.x + 0.5 + var11[1][0] * 0.5;
            real var30 = tp.z + 0.5 + var11[1][2] * 0.5;
            var12 = var28 - var24;
            var14 = var30 - var26;
            real var32;
            real var34;
            real var36;
            if (var12 == 0.0) {
                m_pos.x = tp.x + 0.5;
                var22 = m_pos.z - tp.z;
            }
            else if (var14 == 0.0) {
                m_pos.z = tp.z + 0.5;
                var22 = m_pos.x - tp.x;
            }
            else {
                var32 = m_pos.x - var24;
                var34 = m_pos.z - var26;
                var36 = (var32 * var12 + var34 * var14) * 2.0;
                var22 = var36;
            }

            m_pos.x = var24 + var12 * var22;
            m_pos.z = var26 + var14 * var22;
            setPos(Vec3(m_pos.x, m_pos.y + m_heightOffset, m_pos.z));
            var32 = m_vel.x;
            var34 = m_vel.z;
            if (m_pRider) 
            {
                var32 *= 0.75;
                var34 *= 0.75;
            }

            if (var32 < -var4)
                var32 = -var4;

            if (var32 > var4)
                var32 = var4;

            if (var34 < -var4)
                var34 = -var4;

            if (var34 > var4)
                var34 = var4;

            move(Vec3(var32, 0.0, var34));
            if (var11[0][1] != 0 && Mth::floor(m_pos.x) - tp.x == var11[0][0] && Mth::floor(m_pos.z) - tp.z == var11[0][2]) {
                setPos(Vec3(m_pos.x, m_pos.y + var11[0][1], m_pos.z));
            }
            else if (var11[1][1] != 0 && Mth::floor(m_pos.x) - tp.x == var11[1][0] && Mth::floor(m_pos.z) - tp.z == var11[1][2]) {
                setPos(Vec3(m_pos.x, m_pos.y + var11[1][1], m_pos.z));
            }

            if (m_pRider) 
            {
                m_vel.x *= 0.997;
                m_vel.y *= 0.0;
                m_vel.z *= 0.997;
            }
            else 
            {
                if (m_type == 2) 
                {
                    var36 = Mth::sqrt(m_xPush * m_xPush + m_zPush * m_zPush);
                    if (var36 > 0.01) 
                    {
                        canPush = true;
                        m_xPush /= var36;
                        m_zPush /= var36;
                        real var38 = 0.04;
                        m_vel.x *= 0.8;
                        m_vel.y *= 0.0;
                        m_vel.z *= 0.8;
                        m_vel.x += m_xPush * var38;
                        m_vel.z += m_zPush * var38;
                    }
                    else 
                    {
                        m_vel.x *= 0.9;
                        m_vel.y *= 0.0;
                        m_vel.z *= 0.9;
                    }
                }

                m_vel.x *= 0.96F;
                m_vel.y *= 0.0;
                m_vel.z *= 0.96F;
            }

            if (var49) 
            {
                var36 = Mth::sqrt(m_vel.x * m_vel.x + m_vel.z * m_vel.z);
                if (var36 > 0.01) 
                {
                    const constexpr real var44 = 0.06;
                    m_vel.x += m_vel.x / var36 * var44;
                    m_vel.z += m_vel.z / var36 * var44;
                }
                else if (data == 1)
                {
                    if (m_pLevel->isNormalTile(tp.west()))
                    {
                        m_vel.x = 0.02;
                    }
                    else if (m_pLevel->isNormalTile(tp.east()))
                    {
                        m_vel.x = -0.02;
                    }
                }
                else if (data == 0)
                {
                    if (m_pLevel->isNormalTile(tp.north()))
                    {
                        m_vel.z = 0.02;
                    }
                    else if (m_pLevel->isNormalTile(tp.south()))
                    {
                        m_vel.z = -0.02;
                    }
                }
            }

            Vec3* var46 = getPos(m_pos);
            if (var46 && var9) 
            {
                real var37 = (var9->y - var46->y) * 0.05;
                var20 = Mth::sqrt(m_vel.x * m_vel.x + m_vel.z * m_vel.z);
                if (var20 > 0.0) 
                {
                    m_vel.x = m_vel.x / var20 * (var20 + var37);
                    m_vel.z = m_vel.z / var20 * (var20 + var37);
                }

                setPos(Vec3(m_pos.x, var46->y, m_pos.z));
     
            }
            SAFE_DELETE(var9);
            SAFE_DELETE(var46);

            int var47 = Mth::floor(m_pos.x);
            int var48 = Mth::floor(m_pos.z);
            if (var47 != tp.x || var48 != tp.z)
            {
                var20 = Mth::sqrt(m_vel.x * m_vel.x + m_vel.z * m_vel.z);
                m_vel.x = var20 * (var47 - tp.x);
                m_vel.z = var20 * (var48 - tp.z);
            }

            if (m_type == 2)
            {
                real var39 = Mth::sqrt(m_xPush * m_xPush + m_zPush * m_zPush);
                if (var39 > 0.01 && m_vel.x * m_vel.x + m_vel.z * m_vel.z > 0.001) {
                    m_xPush /= var39;
                    m_zPush /= var39;
                    if (m_xPush * m_vel.x + m_zPush * m_vel.z < 0.0) {
                        m_xPush = 0.0;
                        m_zPush = 0.0;
                    }
                    else {
                        m_xPush = m_vel.x;
                        m_zPush = m_vel.z;
                    }
                }
            }
        }
        else {
            if (m_vel.x < -var4)
                m_vel.x = -var4;

            if (m_vel.x > var4)
                m_vel.x = var4;

            if (m_vel.z < -var4)
                m_vel.z = -var4;

            if (m_vel.z > var4)
                m_vel.z = var4;

            if (m_bOnGround)
            {
                m_vel.x *= 0.5;
                m_vel.y *= 0.5;
                m_vel.z *= 0.5;
            }

            move(m_vel);
            if (!m_bOnGround) 
            {
                m_vel.x *= 0.95;
                m_vel.y *= 0.95;
                m_vel.z *= 0.95;
            }
        }

        m_rot.x = 0.0F;
        real var43 = m_oPos.x - m_pos.x;
        real var44 = m_oPos.z - m_pos.z;
        if (var43 * var43 + var44 * var44 > 0.001)
        {
            m_rot.y = Mth::atan2(var44, var43) * 180.0 / M_PI;
            if (m_bFlipped)
                m_rot.y += 180.0F;
        }

        float var13;
        for (var13 = m_rot.y - m_rotPrev.y; var13 >= 180.0; var13 -= 360.0) {
        }

        while (var13 < -180.0)
            var13 += 360.0;

        if (var13 < -170.0 || var13 >= 170.0) 
        {
            m_rot.y += 180.0F;
            m_bFlipped = !m_bFlipped;
        }

        setRot(m_rot);
        AABB hit = m_hitbox;
        hit.grow(0.2, 0.0, 0.2);
        auto var15 = m_pLevel->getEntities(this, hit);
        if (var15.size() > 0) 
        {
            for (auto& var17 : var15)
            {
                if (var17 != m_pRider && var17->isPushable() && var17->getCategory().contains(EntityCategories::MINECART))
                    var17->push(this);
            }
        }

        if (m_pRider && m_pRider->m_bRemoved) 
            m_pRider = nullptr;

        if (canPush && m_random.nextInt(4) == 0) 
        {
            --m_fuel;
            if (m_fuel < 0)
                m_xPush = m_zPush = 0.0;

            m_pLevel->addParticle("largesmoke", Vec3(m_pos.x, m_pos.y + 0.8, m_pos.z));
        }

    }
}

real Minecart::getRideHeight()
{
	return -0.3;
}

bool Minecart::hurt(Entity*, int mul)
{
    if (!m_pLevel->m_bIsOnline && !m_bRemoved)
    {
        m_hurtDir = -m_hurtDir;
        m_hurtTime = 10;
        markHurt();
        m_damage += mul * 10;
        if (m_damage > 40)
        {
            spawnAtLocation(Item::minecart->m_itemID, 1, 0.0F);
            if (m_type == 1)
            {
                spawnAtLocation(Tile::chest->m_ID, 1, 0.0F);
            }
            else if (m_type == 2)
            {
                spawnAtLocation(Tile::furnace->m_ID, 1, 0.0F);
            }

            remove();
        }
        return true;
    }
    else
        return true;
}

void Minecart::animateHurt()
{
    m_hurtDir = -m_hurtDir;
    m_hurtTime = 10;
    m_damage += m_damage * 10;
}

void Minecart::remove()
{
    for (int var1 = 0; var1 < getContainerSize(); ++var1) {
        auto var2 = getItem(var1);
        if (var2) {
            float var3 = m_random.nextFloat() * 0.8F + 0.1F;
            float var4 = m_random.nextFloat() * 0.8F + 0.1F;
            float var5 = m_random.nextFloat() * 0.8F + 0.1F;

            while (var2->m_count > 0) {
                int var6 = m_random.nextInt(21) + 10;
                if (var6 > var2->m_count) {
                    var6 = var2->m_count;
                }

                var2->m_count -= var6;
                auto var7 = std::make_shared<ItemEntity>(m_pLevel, Vec3(m_pos.x + var3, m_pos.y + var4, m_pos.z + var5), std::make_shared<ItemInstance>(var2->m_itemID, var6, var2->getAuxValue()));
                float var8 = 0.05F;
                var7->m_vel.x = m_random.nextGaussian() * var8;
                var7->m_vel.y = m_random.nextGaussian() * var8 + 0.2F;
                var7->m_vel.z = m_random.nextGaussian() * var8;
                m_pLevel->addEntity(var7);
            }
        }
    }

    Entity::remove();
}

Vec3* Minecart::getPosOffs(const Vec3& pos, real var7)
{
    TilePos tp(pos);
    if (RailTile::isRail(m_pLevel->getTile(tp.below())))
        --tp.y;

    int tile = m_pLevel->getTile(tp);

    if (RailTile::isRail(tile))
    {
        Vec3 newPos = pos;
        int var12 = m_pLevel->getData(tp);
        newPos.y = tp.y;

        if (RailTile::isPowered(Tile::tiles[tile]))
            var12 &= 7;

        if (var12 >= 2 && var12 <= 5)
            newPos.y = tp.y;

        auto var13 = EXITS[var12];
        real var14 = (var13[1][0] - var13[0][0]);
        real var16 = (var13[1][2] - var13[0][2]);
        real var18 = Mth::sqrt(var14 * var14 + var16 * var16);
        var14 /= var18;
        var16 /= var18;
        newPos.x += var14 * var7;
        newPos.z += var16 * var7;
        if (var13[0][1] != 0 && Mth::floor(pos.x) - tp.x == var13[0][0] && Mth::floor(newPos.z) - tp.z == var13[0][2])
            newPos.y += var13[0][1];
        else if (var13[1][1] != 0 && Mth::floor(pos.x) - tp.x == var13[1][0] && Mth::floor(newPos.z) - tp.z == var13[1][2])
            newPos.y += var13[1][1];

        return getPos(newPos);
    }
    else
    {
        return nullptr;
    }
}

void Minecart::addAdditionalSaveData(CompoundIO tag)
{
    tag->putInt("Type", m_type);
    if (m_type == 2) 
    {
        tag->putDouble("PushX", m_xPush);
        tag->putDouble("PushZ", m_zPush);
        tag->putShort("Fuel", m_fuel);
    } else if (m_type == 1) 
        SimpleContainer::save(tag);
}

void Minecart::readAdditionalSaveData(CompoundIO tag)
{
    m_type = tag->getInt("Type");
    if (m_type == 2) 
    {
        m_xPush = tag->getDouble("PushX");
        m_zPush = tag->getDouble("PushZ");
        m_fuel = tag->getShort("Fuel");
    }
    else if (m_type == 1)
        SimpleContainer::load(tag);
}

float Minecart::getLootContent()
{
    int var1 = 0;

    for (int var2 = 0; var2 < getContainerSize(); ++var2) {
        if (getItem(var2)) {
            ++var1;
        }
    }

    return (float)var1 / getContainerSize();
}

Vec3* Minecart::getPos(const Vec3& vec)
{
    TilePos tp(vec);
    if (RailTile::isRail(m_pLevel->getTile(tp.below())))
        --tp.y;

    int tile = m_pLevel->getTile(tp);
    if (RailTile::isRail(tile))
    {
        Vec3* newPos = new Vec3(vec);
        int var10 = m_pLevel->getData(tp);
        newPos->y = tp.y;

        if (RailTile::isPowered(Tile::tiles[tile]))
            var10 &= 7;
  

        if (var10 >= 2 && var10 <= 5)
            newPos->y = (tp.y + 1);

        auto var11 = EXITS[var10];
        real var12 = 0.0;
        real var14 = tp.x + 0.5 + var11[0][0] * 0.5;
        real var16 = tp.y + 0.5 + var11[0][1] * 0.5;
        real var18 = tp.z + 0.5 + var11[0][2] * 0.5;
        real var20 = tp.x + 0.5 + var11[1][0] * 0.5;
        real var22 = tp.y + 0.5 + var11[1][1] * 0.5;
        real var24 = tp.z + 0.5 + var11[1][2] * 0.5;
        real var26 = var20 - var14;
        real var28 = (var22 - var16) * 2.0;
        real var30 = var24 - var18;
        if (var26 == 0.0) 
        {
            newPos->x = tp.x + 0.5;
            var12 = newPos->z - tp.z;
        }
        else if (var30 == 0.0) 
        {
            newPos->z = tp.z + 0.5;
            var12 = newPos->x - tp.x;
        }
        else 
        {
            real var32 = newPos->x - var14;
            real var34 = newPos->z - var18;
            real var36 = (var32 * var26 + var34 * var30) * 2.0;
            var12 = var36;
        }

        newPos->x = var14 + var26 * var12;
        newPos->y = var16 + var28 * var12;
        newPos->z = var18 + var30 * var12;
        if (var28 < 0.0)
            ++newPos->y;

        if (var28 > 0.0)
            newPos->y += 0.5;

        return newPos;
    }
    else {
        return nullptr;
    }
}

void Minecart::push(Entity* ent)
{
    if (!m_pLevel->m_bIsOnline && ent != m_pRider.get())
    {
        if (ent->getCategory().contains(EntityCategories::MOB) && !ent->isPlayer() && m_type == 0 && m_vel.x * m_vel.x + m_vel.z * m_vel.z > 0.01 && !m_pRider && !m_pRiding)
            ent->ride(shared_from_this());

        real var2 = ent->m_pos.x - m_pos.x;
        real var4 = ent->m_pos.z - m_pos.z;
        real var6 = var2 * var2 + var4 * var4;
        if (var6 >= 9.999999747378752E-5)
        {
            var6 = Mth::sqrt(var6);
            var2 /= var6;
            var4 /= var6;
            real var8 = 1.0 / var6;
            if (var8 > 1.0)
                var8 = 1.0;

            var2 *= var8;
            var4 *= var8;
            var2 *= 0.1;
            var4 *= 0.1;
            var2 *= 1.0F - m_pushThrough;
            var4 *= 1.0F - m_pushThrough;
            var2 *= 0.5;
            var4 *= 0.5;
            if (ent->getCategory().contains(EntityCategories::MINECART)) {
                Minecart* minecart = (Minecart*)ent;
                real xDiff = ent->m_pos.x - m_pos.x;
                real zDiff = ent->m_pos.z - m_pos.z;

                real var14 = xDiff * minecart->m_vel.z + zDiff * minecart->m_oPos.x;
                var14 *= var14;
                if (var14 > 5.0)
                    return;
           
                real var10 = ent->m_vel.x + m_vel.x;
                real var12 = ent->m_vel.z + m_vel.z;
                if (minecart->m_type == 2 && m_type != 2)
                {
                    m_vel.x *= 0.2;
                    m_vel.z *= 0.2;
                    Entity::push(Vec3(ent->m_vel.x - var2, 0.0, ent->m_vel.z - var4));
                    ent->m_vel.x *= 0.7;
                    ent->m_vel.z *= 0.7;
                }
                else if (minecart->m_type != 2 && m_type == 2)
                {
                    ent->m_vel.x *= 0.2;
                    ent->m_vel.z *= 0.2;
                    ent->push(Vec3(m_vel.x + var2, 0.0, m_vel.z + var4));
                    m_vel.x *= 0.7;
                    m_vel.z *= 0.7;
                }
                else
                {
                    var10 /= 2.0;
                    var12 /= 2.0;
                    m_vel.x *= 0.2;
                    m_vel.z *= 0.2;
                    Entity::push(Vec3(var10 - var2, 0.0, var12 - var4));
                    ent->m_vel.x *= 0.2;
                    ent->m_vel.z *= 0.2;
                    ent->push(Vec3(var10 + var2, 0.0, var12 + var4));
                }
            }
            else
            {
                Entity::push(Vec3(-var2, 0.0, -var4));
                ent->push(Vec3(var2 / 4.0, 0.0, var4 / 4.0));
            }
        }
    }
}


void Minecart::lerpTo(const Vec3& pos, const Vec2& rot, int steps)
{
    m_lPos = pos;
    m_lRot = rot;
    m_lSteps = steps + 2;
    m_vel = m_lPosD;
}

void Minecart::lerpMotion(const Vec3& motion) 
{
    m_lPosD = m_vel = motion;
}

bool Minecart::stillValid(Player* var1) 
{
    return !m_bRemoved && !(var1->distanceToSqr(this) > 64.0);
}
