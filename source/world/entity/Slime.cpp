#include "world/entity/Slime.hpp"
#include "world/level/Level.hpp"

Slime::Slime(Level* pLevel) : Mob(pLevel), m_size(1), m_jumpDelay(0), m_oSquish(0), m_squish(0)
{
	m_pType = EntityType::slime;
	m_texture = "mob/slime.png";
	m_size = 1 << m_random.nextInt(3);
	m_heightOffset = 0.0f;
	m_jumpDelay = m_random.nextInt(20) + 10;
	setSize(m_size);
}

void Slime::setSize(int size)
{
	m_size = size;
	Mob::setSize(0.6F * size, 0.6F * size);
	m_health = size * size;
	setPos(m_pos);
}

void Slime::addAdditionalSaveData(CompoundIO tag)
{
    Mob::addAdditionalSaveData(tag);
    tag->putInt("Size", m_size - 1);
}

void Slime::readAdditionalSaveData(CompoundIO tag)
{
    Mob::readAdditionalSaveData(tag);
    m_size = tag->getInt("Size") + 1;
}

void Slime::tick()
{
    m_oSquish = m_squish;
    bool var1 = m_bOnGround;
    Mob::tick();
    if (m_bOnGround && !var1)
    {
        for (int var2 = 0; var2 < m_size * 8; ++var2)
        {
            float var3 = m_random.nextFloat() * 3.1415927F * 2.0F;
            float var4 = m_random.nextFloat() * 0.5F + 0.5F;
            float var5 = Mth::sin(var3) * m_size * 0.5F * var4;
            float var6 = Mth::cos(var3) * m_size * 0.5F * var4;
            m_pLevel->addParticle("slime", Vec3(m_pos.x + var5, m_hitbox.min.y, m_pos.z + var6));
        }

        if (m_size > 2)
            m_pLevel->playSound(this, "mob.slime", getSoundVolume(), ((m_random.nextFloat() - m_random.nextFloat()) * 0.2F + 1.0F) / 0.8F);

        m_squish = -0.5F;
    }

    m_squish *= 0.6F;
}

void Slime::updateAi()
{
    auto target = m_pLevel->getNearestPlayer(this, 16.0);
    if (target)
        lookAt(target.get(), 10.0f, 10.0f);
    
    if (m_bOnGround && m_jumpDelay-- <= 0)
    {
        m_jumpDelay = m_random.nextInt(20) + 10;
        if (target)
            m_jumpDelay /= 3;

        m_bJumping = true;
        if (m_size > 1)
            m_pLevel->playSound(this, "mob.slime", getSoundVolume(), ((m_random.nextFloat() - m_random.nextFloat()) * 0.2F + 1.0F) / 0.8F);

        m_squish = 1.0F;
        m_moving.y = 1.0F - m_random.nextFloat() * 2.0F;
        m_moving.x = m_size;
    }
    else
    {
        m_bJumping = false;
        if (m_bOnGround)
            m_moving = Vec2::ZERO;
    }
}

void Slime::remove()
{
    if (m_size > 1 && !m_health) {
        for (int i = 0; i < 4; ++i)
        {
            auto slime = std::make_shared<Slime>(m_pLevel);
            slime->setSize(m_size / 2);
            slime->moveTo(m_pos.add(((i % 2) - 0.5F) * m_size / 4.0F, 0.5, ((i / 2) - 0.5F) * m_size / 4.0F), Vec2(m_random.nextFloat() * 360.0F, 0.0F));
            m_pLevel->addEntity(slime);
        }
    }

    Mob::remove();
}

void Slime::playerTouch(Player* player)
{
    if (m_size > 1 && canSee(player) && distanceTo(player) < 0.6 * m_size && player->hurt(this, m_size))
        m_pLevel->playSound(this, "mob.slimeattack", 1.0F, (m_random.nextFloat() - m_random.nextFloat()) * 0.2F + 1.0F);
}

bool Slime::canSpawn()
{
    return (m_size == 1 || m_pLevel->m_difficulty > 0) && m_random.nextInt(10) == 0 && m_pLevel->getChunk(m_pos)->getRandom(987234911L).nextInt(10) == 0 && m_pos.y < 16.0;
}
