/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>
#include "common/Utils.hpp"
#include "world/level/Material.hpp"
#include "ItemInstance.hpp"
#include "world/level/TilePos.hpp"
#include "world/Facing.hpp"

#define C_MAX_ITEMS (C_MAX_TILES * 2)


class ItemInstance; // in case we're included from ItemInstance.hpp

class Level;
class Entity;
class Mob;
class Player;
class Tile;

class Item
{
public: // Sub structures
	enum EquipmentSlot
	{
		SLOT_NONE = -1,
		FEET,
		LEGS,
		CHEST,
		HEAD
	};

	struct Tier
	{
		int   m_level;
		int   m_uses;
		float m_speed;
		int   m_damage;

		Tier(int level, int uses, float speed, int damage) :
			m_level(level),
			m_uses(uses),
			m_speed(speed),
			m_damage(damage)
		{
		}

		static Tier WOOD, STONE, IRON, DIAMOND, GOLD;
	};

	struct ArmorTier
	{
		int m_level;
		std::string m_texture1;
		std::string m_texture2;

		ArmorTier(int level, std::string texture1, std::string texture2) : m_level(level), m_texture1(texture1), m_texture2(texture2)
		{
		}

		const std::string& getTexture(EquipmentSlot slot)
		{
			if (slot == EquipmentSlot::LEGS) return m_texture2;	
			return m_texture1;
		}

		static ArmorTier CLOTH, CHAIN, IRON, DIAMOND, GOLD;
	};

public: // Methods
	Item(int ID);

	//@NOTE: The setters are virtual for whatever reason

	virtual Item* setIcon(int icon);
	virtual Item* setMaxStackSize(int mss);
	virtual Item* setIcon(int ix, int iy);
	virtual int getIcon(const ItemInstance*) const;
	virtual bool useOn(ItemInstance*, Level*, const TilePos& pos, Facing::Name face);
	virtual bool useOn(ItemInstance*, Player*, Level*, const TilePos& pos, Facing::Name face);
	virtual float getDestroySpeed(ItemInstance * instance, const Tile * tile);
	virtual std::shared_ptr<ItemInstance> use(std::shared_ptr<ItemInstance>, Level*, Player*);
	virtual int getMaxStackSize();
	virtual int getLevelDataForAuxValue(int x);
	virtual bool isStackedByData();
	virtual int getMaxDamage();
	virtual void hurtEnemy(ItemInstance*, Mob*);
	virtual void mineBlock(ItemInstance * instance, int tile, const TilePos& pos, Facing::Name face);
	virtual int getAttackDamage(Entity*);
	virtual bool canDestroySpecial(const Tile*);
	virtual void interactEnemy(ItemInstance*, Mob*);
	virtual Item* handEquipped();
	virtual bool isHandEquipped();
	virtual bool isMirroredArt();
	virtual Item* setDescriptionId(const std::string& desc);
	virtual std::string getDescription();
	virtual std::string getDescription(ItemInstance*);
	virtual std::string getDescriptionId();
	virtual std::string getDescriptionId(ItemInstance*);
	virtual Item* setCraftingRemainingItem(Item*);
	virtual Item* getCraftingRemainingItem();
	virtual bool hasCraftingRemainingItem();
	virtual std::string getName();
	virtual void onCrafting(std::shared_ptr<ItemInstance>, Player*, Level*);
	
	// Custom methods
	virtual EquipmentSlot getEquipmentSlot() const;
	virtual int getDefense() const;
	virtual const std::string& getArmorTexture() const;

	static void initItems();
	
public: // Item class fields
	int m_itemID;
	int m_maxStackSize;
	int m_maxDamage;
	int m_icon;
	bool m_bHandEquipped;
	bool m_bStackedByData;
	Item* m_pCraftingRemainingItem;
	std::string m_DescriptionID;

public: // Static declarations
	static std::string ICON_DESCRIPTION_PREFIX;

	// The item array.
	static Item* items[C_MAX_ITEMS];
	static Random itemRand;

	static Item* ironShovel;
	static Item* ironPickaxe;
	static Item* ironAxe;
	static Item* flintAndSteel;
	static Item* apple;
	static Item* bow;
	static Item* arrow;
	static Item* coal;
	static Item* diamond;
	static Item* ironIngot;
	static Item* goldIngot;
	static Item* ironSword;
	static Item* woodSword;
	static Item* woodShovel;
	static Item* woodPickaxe;
	static Item* woodAxe;
	static Item* stoneSword;
	static Item* stoneShovel;
	static Item* stonePickaxe;
	static Item* stoneAxe;
	static Item* diamondSword;
	static Item* diamondShovel;
	static Item* diamondPickaxe;
	static Item* diamondAxe;
	static Item* stick;
	static Item* bowl;
	static Item* mushroomStew;
	static Item* goldSword;
	static Item* goldShovel;
	static Item* goldPickaxe;
	static Item* goldAxe;
	static Item* string;
	static Item* feather;
	static Item* sulphur;
	static Item* woodHoe;
	static Item* stoneHoe;
	static Item* ironHoe;
	static Item* diamondHoe;
	static Item* goldHoe;
	static Item* seeds;
	static Item* wheat;
	static Item* bread;
	static Item* clothHelmet;
	static Item* clothChestplate;
	static Item* clothLeggings;
	static Item* clothBoots;
	static Item* chainHelmet;
	static Item* chainChestplate;
	static Item* chainLeggings;
	static Item* chainBoots;
	static Item* ironHelmet;
	static Item* ironChestplate;
	static Item* ironLeggings;
	static Item* ironBoots;
	static Item* diamondHelmet;
	static Item* diamondChestplate;
	static Item* diamondLeggings;
	static Item* diamondBoots;
	static Item* goldHelmet;
	static Item* goldChestplate;
	static Item* goldLeggings;
	static Item* goldBoots;
	static Item* flint;
	static Item* rawPorkchop;
	static Item* cookedPorkchop;
	static Item* painting;
	static Item* goldApple;
	static Item* sign;
	static Item* woodDoor;
	static Item* emptyBucket;
	static Item* waterBucket;
	static Item* lavaBucket;
	static Item* minecart;
	static Item* saddle;
	static Item* ironDoor;
	static Item* redStone;
	static Item* snowBall;
	static Item* boat;
	static Item* leather;
	static Item* milk;
	static Item* brick;
	static Item* clay;
	static Item* reeds;
	static Item* paper;
	static Item* book;
	static Item* slimeBall;
	static Item* minecart_chest;
	static Item* minecart_furnace;
	static Item* egg;
	static Item* compass;
	static Item* fishingRod;
	static Item* clock;
	static Item* yellowDust;
	static Item* rawFish;
	static Item* cookedFish;
	static Item* dyePowder;
	static Item* bone;
	static Item* sugar;
	static Item* cake;
	static Item* bed;
	static Item* diode;
	static Item* record_01;
	static Item* record_02;
	static Item* camera;
};
