#include "ThrownEgg.hpp"
#include "world/level/Level.hpp"

//For some reason, Thrown Eggs don't have a mapping in the EntityIO in the original
ThrownEgg::ThrownEgg(Level* level) : Snowball(level)
{
	m_pType = EntityType::thrownEgg;
}

ThrownEgg::ThrownEgg(Level* level, const Vec3& pos) : Snowball(level, pos)
{
    m_pType = EntityType::thrownEgg;
}

ThrownEgg::ThrownEgg(Level* level, std::shared_ptr<Mob> mob) : Snowball(level, mob)
{
    m_pType = EntityType::thrownEgg;
}

void ThrownEgg::onHit()
{
    if (!m_pLevel->m_bIsOnline && m_random.nextInt(8) == 0) {
        int count = 1;
        if (m_random.nextInt(32) == 0) {
            count = 4;
        }

        for (int i = 0; i < count; ++i) {
            auto chicken = EntityType::chicken->newEntity(m_pLevel);
            chicken->moveTo(m_pos, Vec2(m_rot.y, 0.0F));
            m_pLevel->addEntity(chicken);
        }
    }
    Snowball::onHit();
}
