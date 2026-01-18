/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <sstream>
#include "ItemInstance.hpp"
#include "world/tile/Tile.hpp"
#include "world/entity/Player.hpp"

void ItemInstance::_init(int itemID, int count, int auxValue)
{
	m_itemID = itemID;
	m_count = count;
	m_auxValue = auxValue;
	m_popTime = 0;

	//@BUG? Not using the auxValue.  This is problematic in the case of wool and dyes.
}

ItemInstance::ItemInstance()
{
	_init(0, 0, 0);
}

ItemInstance::ItemInstance(std::shared_ptr<CompoundTag> tag) {
	m_count = 0;
	load(tag);
}

ItemInstance::ItemInstance(Item* pItem)
{
	_init(pItem->m_itemID, 1, 0);
}

ItemInstance::ItemInstance(Item* pItem, int amount)
{
	_init(pItem->m_itemID, amount, 0);
}

ItemInstance::ItemInstance(Item* pItem, int amount, int auxValue)
{
	_init(pItem->m_itemID, amount, auxValue);
}

ItemInstance::ItemInstance(Tile* pTile)
{
	_init(pTile->m_ID, 1, 0);
}

ItemInstance::ItemInstance(Tile* pTile, int amount)
{
	_init(pTile->m_ID, amount, 0);
}

ItemInstance::ItemInstance(Tile* pTile, int amount, int auxValue)
{
	_init(pTile->m_ID, amount, auxValue);
}

ItemInstance::ItemInstance(int itemID, int amount, int auxValue)
{
	_init(itemID, amount, auxValue);
}


Item* ItemInstance::getItem() const
{
	return Item::items[m_itemID];
}

std::shared_ptr<ItemInstance> ItemInstance::copy() const
{
	return std::make_shared<ItemInstance>(m_itemID, m_count, m_auxValue);
}

std::shared_ptr<ItemInstance> ItemInstance::copyWithAcount(int count) const
{
	return std::make_shared<ItemInstance>(m_itemID, count, m_auxValue);
}

bool ItemInstance::canDestroySpecial(const Tile* tile)
{
	return getItem()->canDestroySpecial(tile);
}

std::string ItemInstance::getDescriptionId()
{
	return getItem()->getDescriptionId(this);
}

float ItemInstance::getDestroySpeed(const Tile * tile)
{
	return getItem()->getDestroySpeed(this, tile);
}

int ItemInstance::getIcon() const
{
	return getItem()->getIcon(this);
}

int ItemInstance::getMaxDamage() const
{
	return getItem()->getMaxDamage();
}

int ItemInstance::getMaxStackSize() const
{
	return getItem()->getMaxStackSize();
}

void ItemInstance::hurt(int by)
{
	if (!isDamageableItem())
		return;

	m_auxValue += by;
	if (m_auxValue > getMaxDamage())
	{
		m_count--;
		if (m_count < 0)
			m_count = 0;
		m_auxValue = 0;
	}
}

void ItemInstance::hurtAndBreak(int amount, Entity* ent)
{
	if (isDamageableItem())
	{
		m_auxValue += amount;
		if (m_auxValue > getMaxDamage())
		{
			if (ent->isPlayer())
			{
				((Player*)ent)->awardStat(Stats::statItemBreak[m_itemID]);
			}


			--m_count;
			if (m_count < 0)
				m_count = 0;

			m_auxValue = 0;
		}

	}
}

void ItemInstance::hurtEnemy(Mob* mob, Player* player)
{
	if (getItem()->hurtEnemy(this, mob, player))
		(player->awardStat(Stats::statItemUsed[m_itemID]));
}

void ItemInstance::interactEnemy(Mob* mob)
{
	getItem()->interactEnemy(this, mob);
}

bool ItemInstance::isDamageableItem()
{
	return getItem()->getMaxDamage() > 0;
}

bool ItemInstance::isDamaged()
{
	return isDamageableItem() && m_auxValue > 0;
}

bool ItemInstance::isStackable()
{
	return getMaxStackSize() > 1 && (!isDamageableItem() || !isDamaged());
}

bool ItemInstance::isStackedByData()
{
	return getItem()->isStackedByData();
}

bool ItemInstance::matches(const ItemInstance& other) const
{
	return this->getAuxValue() == other.getAuxValue() &&
		this->m_count == other.m_count &&
		this->m_itemID == other.m_itemID;
}

bool ItemInstance::sameItem(const std::shared_ptr<ItemInstance> item) const
{
	return m_itemID == item->m_itemID;
}

bool ItemInstance::matches(const std::shared_ptr<ItemInstance>& a1, const std::shared_ptr<ItemInstance>& a2)
{
	if (a1 == a2 && !a1)
		return true;

	if (!a1 || !a2)
		return false;

	return a1->matches(*a2); // agora com referência
}

int ItemInstance::getAttackDamage(Entity *pEnt)
{
	return getItem()->getAttackDamage(pEnt);
}

void ItemInstance::mineBlock(int tile, const TilePos& pos, Facing::Name face, Player* player)
{
	if (getItem()->mineBlock(this, tile, pos, face, player))
		player->awardStat(Stats::statItemUsed[m_itemID]);
}

std::shared_ptr<ItemInstance> ItemInstance::remove(int count)
{
	m_count -= count;
	return std::make_shared<ItemInstance>(m_itemID, count, m_auxValue);
}

void ItemInstance::setDescriptionId(const std::string& str)
{
}

void ItemInstance::snap(Player*)
{

}

std::string ItemInstance::toString()
{
	std::stringstream ss;
	ss << m_count << "x" << getItem()->getDescriptionId() << "@" << m_auxValue;

	return ss.str();
}

std::shared_ptr<ItemInstance> ItemInstance::use(Level* level, Player* player)
{
	return getItem()->use(shared_from_this(), level, player);
}

bool ItemInstance::useOn(Player* player, Level* level, const TilePos& pos, Facing::Name face)
{
	bool used = getItem()->useOn(this, player, level, pos, face);
	if (used)
		player->awardStat(Stats::statItemUsed[m_itemID]);

	return used;
}

void ItemInstance::onCraftedBy(Player* player, Level* level)
{
	onCraftedBy(player, level, m_count);
}

void ItemInstance::onCraftedBy(Player* player, Level* level, int amount)
{
	player->awardStat(Stats::itemCrafted[m_itemID], amount);
	getItem()->onCraftedBy(shared_from_this(), player, level);
}

bool ItemInstance::isNull() const
{
	if (m_itemID <= 0)
		return true;

	if (m_auxValue != 0)
		return false;
	if (m_count != 0)
		return false;
	if (m_popTime != 0)
		return false;

	return true;
}
