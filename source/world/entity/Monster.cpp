#include "world/entity/Monster.hpp"

Monster::Monster(Level* pLevel) : PathfinderMob(pLevel)
{
	m_health = 20;
	m_attackDamage = 2;
}

void Monster::aiStep()
{
	float brightness = getBrightness(1.0f);

	if (brightness > 0.5f)
	{
		m_noActionTime += 2;
	}

	PathfinderMob::aiStep();
}

void Monster::tick()
{
	PathfinderMob::tick();

	// Remove any monsters when in peaceful mode
	if (m_pLevel->m_difficulty == 0)
	{
		remove();
	}
}

std::shared_ptr<Entity> Monster::findAttackTarget()
{
	auto player = m_pLevel->getNearestPlayer(this, 16.0f);

	if (player && canSee(player.get()))
	{
		return player;
	}

	return nullptr;
}

bool Monster::hurt(Entity* pCulprit, int damage)
{
	if (PathfinderMob::hurt(pCulprit, damage))
	{
		if (pCulprit != this)
		{
			m_pAttackTarget = pCulprit ? pCulprit->shared_from_this() : nullptr;
		}
		return true;
	}

	return false;
}

void Monster::checkHurtTarget(Entity* pEnt, float f)
{
	if (m_attackTime <= 0 && f < 2.0f && pEnt->m_hitbox.max.y > m_hitbox.min.y && pEnt->m_hitbox.min.y < m_hitbox.max.y)
	{
		m_attackTime = 20;
		pEnt->hurt(this, m_attackDamage);
	}
}

float Monster::getWalkTargetValue(const TilePos& pos) const
{
	return 0.5f - m_pLevel->getBrightness(pos);
}

bool Monster::canSpawn()
{
	TilePos pos(m_pos.x, m_hitbox.min.y, m_pos.z);
	int wh = m_random.nextInt(30);
	if (m_pLevel->getBrightness(LightLayer::Sky, pos) > m_random.nextInt(32))
	{
		return false;
	}
	else
		return m_pLevel->getRawBrightness(pos) <= m_random.nextInt(8) && PathfinderMob::canSpawn();
}