#include "world/level/Level.hpp"
#include "LightningBolt.hpp"


LightningBolt::LightningBolt(Level* level) : Entity(level)
{
	m_pType = EntityType::lightningBolt;
	m_life = 2;
	m_seed = m_random.nextLong();
	m_flashes = m_random.nextInt(3) + 1;
	if (level->m_difficulty >= 2 && level->hasChunksAt(m_pos, 10)) {
		TilePos tp(m_pos);
		if (level->getTile(tp) == 0 && Tile::fire->mayPlace(level, tp))
			level->setTile(tp, Tile::fire->m_ID);

		for (int i = 0; i < 4; ++i) {
			tp.x = Mth::floor(m_pos.x) + m_random.nextInt(3) - 1;
			tp.y = Mth::floor(m_pos.y) + m_random.nextInt(3) - 1;
			tp.z = Mth::floor(m_pos.z) + m_random.nextInt(3) - 1;
			if (level->getTile(tp) == 0 && Tile::fire->mayPlace(level, tp))
				level->setTile(tp, Tile::fire->m_ID);
		}
	}
}

LightningBolt::LightningBolt(Level* level, const Vec3& pos) : LightningBolt(level)
{
	moveTo(pos);
}

void LightningBolt::tick()
{
	Entity::tick();
	if (m_life == 2)
	{
		m_pLevel->playSound(m_pos, "ambient.weather.thunder", 10000.0F, 0.8F + m_random.nextFloat() * 0.2F);
		m_pLevel->playSound(m_pos, "random.explode", 2.0F, 0.5F + m_random.nextFloat() * 0.2F);
	}

	--m_life;
	if (m_life < 0)
	{
		if (m_flashes == 0)
			remove();
		else if (m_life < -m_random.nextInt(10))
		{
			--m_flashes;
			m_life = 1;
			m_seed = m_random.nextLong();
			if (m_pLevel->hasChunksAt(m_pos, 10))
			{
				TilePos tp(m_pos);
				if (m_pLevel->getTile(tp) == 0 && Tile::fire->mayPlace(m_pLevel, tp))
					m_pLevel->setTile(tp, Tile::fire->m_ID);
			}
		}
	}

	if (m_life >= 0) {
		real var6 = 3.0;
		auto var7 = m_pLevel->getEntities(this, AABB(m_pos.x - var6, m_pos.y - var6, m_pos.z - var6, m_pos.x + var6, m_pos.y + 6.0 + var6, m_pos.z + var6));

		for (auto& ent : var7) {
			ent->thunderHit(this);
		}

		m_pLevel->m_flashTime = 2;
	}
}

bool LightningBolt::shouldRender(Vec3& camPos) const
{
	return m_life >= 0;
}
