#include "DiggerItem.hpp"
#include "world/entity/Mob.hpp"

DiggerItem::DiggerItem(int id, int attackDamageBase, Tier& tier, std::initializer_list<Tile*> tiles) : Item(id),  m_tier(tier), m_tiles(tiles), m_speed(tier.m_speed), m_attackDamage(attackDamageBase + tier.m_damage)
{
	m_maxStackSize = 1;
	m_maxDamage = tier.m_uses;
}

float DiggerItem::getDestroySpeed(ItemInstance * instance, const Tile * tile)
{
	if (std::find(m_tiles.begin(), m_tiles.end(), tile) != m_tiles.end())
	{
		return m_speed;
	}
	return 1.0f;
}

void DiggerItem::hurtEnemy(ItemInstance* instance, Mob* mob)
{
	instance->hurt(2);
}

void DiggerItem::mineBlock(ItemInstance * instance, int tile, const TilePos& pos, Facing::Name face)
{
	instance->hurt(1);
}

int DiggerItem::getAttackDamage(Entity* entity)
{
	return m_attackDamage;
}

bool DiggerItem::isHandEquipped()
{
	return true;
}