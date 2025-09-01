/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "common/Utils.hpp"
#include "common/CompoundTag.hpp"
#include "Item.hpp"
#include "world/level/TilePos.hpp"
#include "world/Facing.hpp"

class Item; // in case we're included from Item.hpp
class Tile;
class Level;
class Entity;
class Mob;
class Player;

class ItemInstance : public std::enable_shared_from_this<ItemInstance>
{
private:
    void _init(int itemID, int amount, int auxValue);
    
public:
	ItemInstance();
	ItemInstance(std::shared_ptr<CompoundTag> tag);
	ItemInstance(Item*);
	ItemInstance(Item*, int amount);
	ItemInstance(Item*, int amount, int auxValue);
	ItemInstance(Tile*);
	ItemInstance(Tile*, int amount);
	ItemInstance(Tile*, int amount, int auxValue);
	ItemInstance(int itemID, int amount, int auxValue);

    int getAuxValue() const { return m_auxValue; }
    void setAuxValue(int auxValue) { m_auxValue = auxValue; } // Technically doesn't exist in b1.2_02
    int getDamageValue() const { return m_auxValue; }

	bool canDestroySpecial(const Tile*);
	std::string getDescriptionId();
	float getDestroySpeed(const Tile * tile);
	int getIcon() const;
	int getMaxDamage() const;
	int getMaxStackSize() const;
	void hurt(int by);
	void hurtAndBreak(int, Entity*);
	void hurtEnemy(Mob*);
	void interactEnemy(Mob*);
	bool isDamageableItem();
	bool isDamaged();
	bool isStackable();
	bool isStackedByData();
	bool matches(const ItemInstance&) const;
	bool sameItem(const std::shared_ptr<ItemInstance>) const;
	void mineBlock(int tile, const TilePos& pos, Facing::Name face, Player*);
	std::shared_ptr<ItemInstance> remove(int amt);
	void setDescriptionId(const std::string&);
	void snap(Player*);
	std::string toString();
	std::shared_ptr<ItemInstance> use(Level*, Player*);
	bool useOn(Player*, Level*, const TilePos& pos, Facing::Name face);
	void onCraftedBy(Player*, Level*);
	void onCraftedBy(Player*, Level*, int amount);

	Item* getItem() const;
	std::shared_ptr<ItemInstance> copy() const;

	static bool matches(const std::shared_ptr<ItemInstance>& a1, const std::shared_ptr<ItemInstance>& a2);

	// v0.2.0
	int getAttackDamage(Entity *pEnt);
	bool isNull() const;
	void load(CompoundIO tag) 
	{
		_init(tag->getShort("id"), tag->getByte("Count"), tag->getShort("Damage"));
	}
	std::shared_ptr<CompoundTag> save(CompoundIO tag)
	{
		tag->putShort("id", m_itemID);
		tag->putByte("Count", m_count);
		tag->putShort("Damage", getDamageValue());
		return tag;
	}

public:
	int m_count;
	int m_popTime;
	int m_itemID;
private:
    int m_auxValue;
};

