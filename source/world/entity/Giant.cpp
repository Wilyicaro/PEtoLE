#include "Giant.hpp"

Giant::Giant(Level* pLevel) : Monster(pLevel)
{
	m_pType = EntityType::giant;
	m_texture = "mob/zombie.png";
	m_health *= 10;
	m_runSpeed = 0.5f;
	m_attackDamage = 50;
	m_heightOffset *= 6;
	setSize(m_bbWidth * 6.0F, m_bbHeight * 6.0F);
}

float Giant::getWalkTargetValue(const TilePos& pos) const
{
	return 0.5f - m_pLevel->getBrightness(pos);
}
