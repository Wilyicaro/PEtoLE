#pragma once

#include "Item.hpp"

class WeaponItem : public Item
{
public:
	WeaponItem(int id, Tier& tier);

	virtual float getDestroySpeed(ItemInstance * instance, const Tile * tile) override;
	virtual bool hurtEnemy(ItemInstance* instance, Mob* mob, Player* player) override;
	virtual bool mineBlock(ItemInstance * instance, int tile, const TilePos& pos, Facing::Name face, Player *) override;
	virtual int getAttackDamage(Entity* entity) override;
	virtual bool isHandEquipped() override;

private:
	int m_attackDamage;

protected:
	Tier& m_tier;
};
