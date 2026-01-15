#pragma once

#include "Item.hpp"

#include <initializer_list>

class DiggerItem : public Item
{
public:
	DiggerItem(int id, int attackDamageBase, Tier& tier, std::initializer_list<Tile*> tiles);

	virtual float getDestroySpeed(ItemInstance * instance, const Tile * tile) override;
	virtual bool hurtEnemy(ItemInstance* instance, Mob* mob, Player* player) override;
	virtual bool mineBlock(ItemInstance * instance, int tile, const TilePos& pos, Facing::Name face, Player *) override;
	virtual int getAttackDamage(Entity* entity) override;
	virtual bool isHandEquipped() override;

private:
	float m_speed;
	int m_attackDamage;

protected:
	Tier& m_tier;

	std::vector<Tile*> m_tiles;
};
