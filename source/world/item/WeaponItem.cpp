#include "WeaponItem.hpp"
#include "world/entity/Mob.hpp"
#include "world/entity/Player.hpp"

WeaponItem::WeaponItem(int id, Tier& tier) : Item(id),  m_tier(tier), m_attackDamage(4 + tier.m_damage)
{
	m_maxStackSize = 1;
	m_maxDamage = tier.m_uses;
}

float WeaponItem::getDestroySpeed(ItemInstance * instance, const Tile * tile)
{
	return tile->m_ID == Tile::web->m_ID ? 15.0f : 1.5f;
}

bool WeaponItem::hurtEnemy(ItemInstance* instance, Mob* mob, Player* player)
{
	instance->hurtAndBreak(1, mob);
	return true;
}

bool WeaponItem::mineBlock(ItemInstance * instance, int tile, const TilePos& pos, Facing::Name face, Player* player)
{
	instance->hurtAndBreak(2, player);
	return true;
}

int WeaponItem::getAttackDamage(Entity* entity)
{
	return m_attackDamage;
}

bool WeaponItem::isHandEquipped()
{
	return true;
}