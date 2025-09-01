/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Item.hpp"

#include "CameraItem.hpp"
#include "DoorItem.hpp"
#include "TileItem.hpp"
#include "TilePlanterItem.hpp"
#include "DyeItem.hpp"
#include "BowlFoodItem.hpp"
#include "SaddleItem.hpp"
#include "BucketItem.hpp"
#include "WeaponItem.hpp"
#include "PickaxeItem.hpp"
#include "AxeItem.hpp"
#include "ShovelItem.hpp"
#include "HoeItem.hpp"
#include "ArmorItem.hpp"
#include "FlintAndSteelItem.hpp"
#include "CoalItem.hpp"
#include "BowItem.hpp"
#include "SeedItem.hpp"
#include "MinecartItem.hpp"
#include "BoatItem.hpp"
#include "BedItem.hpp"
#include "RecordItem.hpp"
#include "SignItem.hpp"
#include "EggItem.hpp"
#include "PaintingItem.hpp"
#include "FishingRodItem.hpp"
#include "MapItem.hpp"
#include "ShearsItem.hpp"

#define ITEM(x) ((x) - 256)

#define NEW_ITEM(id) (new Item(ITEM(id)))
#define NEW_X_ITEM(Type, id, ...) (new Type(ITEM(id), __VA_ARGS__))
#define NEW_X_ITEMN(Type, id) (new Type(ITEM(id)))

static bool g_bInittedItems = false;

Item* Item::items[C_MAX_ITEMS];
Random Item::itemRand;

Item::Item(int itemID)
{
	m_bHandEquipped = 0;
	m_itemID = itemID + 256;
	m_bStackedByData = false;
	m_pCraftingRemainingItem = 0;
	m_maxStackSize = 64;
	m_maxDamage = 0;
	m_icon = 0;


	if (Item::items[m_itemID])
	{
		LOG_W("Item conflict id @ %d! Id already used\n", m_itemID);
	}

	Item::items[m_itemID] = this;
}

Item* Item::setIcon(int icon)
{
	m_icon = icon;
	return this;
}

Item* Item::setIcon(int ix, int iy)
{
	return setIcon(ix + 16 * iy);
}

Item* Item::setMaxStackSize(int mss)
{
	m_maxStackSize = mss;
	return this;
}

Item* Item::setCraftingRemainingItem(Item* pItem)
{
	if (m_maxStackSize > 1)
		LOG_W("Max stack size must be 1 for items with crafting results");

	m_pCraftingRemainingItem = pItem;

	return this;
}

Item* Item::setDescriptionId(const std::string& desc)
{
	m_DescriptionID = ICON_DESCRIPTION_PREFIX + desc;
	return this;
}

Item* Item::handEquipped()
{
	//@NOTE: This refers to the in-hand renderer when one is in 3rd person mode.
	// Instead of holding the item nearly parallel to the ground, the item is held
	// perpendicular to the ground. It can be seen in swords (where this is true)
	// vs. compasses (where this is false).
	m_bHandEquipped = true;
	return this;
}

Item::EquipmentSlot Item::getEquipmentSlot() const
{
	return Item::SLOT_NONE;
}

int Item::getDefense() const
{
	return 0;
}

const std::string& Item::getArmorTexture() const
{
	return Util::EMPTY_STRING;
}

const std::string& Item::getStreamingMusic()
{
	return Util::EMPTY_STRING;
}

bool Item::isFood() const
{
	return false;
}

bool Item::isWolfFood() const
{
	return false;
}

bool Item::isComplex() const
{
	return false;
}

Packet* Item::getUpdatePacket(const std::shared_ptr<ItemInstance>&, Level*, const std::shared_ptr<Player>&)
{
	return nullptr;
}

void Item::initItems()
{
	if (g_bInittedItems)
		return;

	g_bInittedItems = true;

	Item::bow = NEW_X_ITEMN(BowItem, ITEM_BOW)
		->setIcon(5, 1)
		->setDescriptionId("bow");

	Item::woodSword = NEW_X_ITEM(WeaponItem, ITEM_SWORD_WOOD, Tier::WOOD)
		->setIcon(0, 4)
		->setDescriptionId("swordWood");

	Item::woodPickaxe = NEW_X_ITEM(PickaxeItem, ITEM_PICKAXE_WOOD, Tier::WOOD)
		->setIcon(0, 6)
		->setDescriptionId("pickaxeWood");

	Item::woodAxe = NEW_X_ITEM(AxeItem, ITEM_HATCHET_WOOD, Tier::WOOD)
		->setIcon(0, 7)
		->setDescriptionId("hatchetWood");

	Item::woodShovel = NEW_X_ITEM(ShovelItem, ITEM_SHOVEL_WOOD, Tier::WOOD)
		->setIcon(0, 5)
		->setDescriptionId("shovelWood");

	Item::woodHoe = NEW_X_ITEM(HoeItem, ITEM_HOE_WOOD, Tier::WOOD)
		->setIcon(0, 8)
		->setDescriptionId("hoeWood");

	Item::stoneSword = NEW_X_ITEM(WeaponItem, ITEM_SWORD_STONE, Tier::STONE)
		->setIcon(1, 4)
		->setDescriptionId("swordStone");

	Item::stonePickaxe = NEW_X_ITEM(PickaxeItem, ITEM_PICKAXE_STONE, Tier::STONE)
		->setIcon(1, 6)
		->setDescriptionId("pickaxeStone");

	Item::stoneAxe = NEW_X_ITEM(AxeItem, ITEM_HATCHET_STONE, Tier::STONE)
		->setIcon(1, 7)
		->setDescriptionId("hatchetStone");

	Item::stoneShovel = NEW_X_ITEM(ShovelItem, ITEM_SHOVEL_STONE, Tier::STONE)
		->setIcon(1, 5)
		->setDescriptionId("shovelStone");

	Item::stoneHoe = NEW_X_ITEM(HoeItem, ITEM_HOE_STONE, Tier::STONE)
		->setIcon(1, 8)
		->setDescriptionId("hoeStone");

	Item::ironSword = NEW_X_ITEM(WeaponItem, ITEM_SWORD_IRON, Tier::IRON)
		->setIcon(2, 4)
		->setDescriptionId("swordIron");

	Item::ironPickaxe = NEW_X_ITEM(PickaxeItem, ITEM_PICKAXE_IRON, Tier::IRON)
		->setIcon(2, 6)
		->setDescriptionId("pickaxeIron");

	Item::ironAxe = NEW_X_ITEM(AxeItem, ITEM_HATCHET_IRON, Tier::IRON)
		->setIcon(2, 7)
		->setDescriptionId("hatchetIron");

	Item::ironShovel= NEW_X_ITEM(ShovelItem, ITEM_SHOVEL_IRON, Tier::IRON)
		->setIcon(2, 5)
		->setDescriptionId("shovelIron");

	Item::ironHoe = NEW_X_ITEM(HoeItem, ITEM_HOE_IRON, Tier::IRON)
		->setIcon(2, 8)
		->setDescriptionId("hoeIron");

	Item::goldSword = NEW_X_ITEM(WeaponItem, ITEM_SWORD_GOLD, Tier::GOLD)
		->setIcon(4, 4)
		->setDescriptionId("swordGold");

	Item::goldPickaxe = NEW_X_ITEM(PickaxeItem, ITEM_PICKAXE_GOLD, Tier::GOLD)
		->setIcon(4, 6)
		->setDescriptionId("pickaxeGold");

	Item::goldAxe = NEW_X_ITEM(AxeItem, ITEM_HATCHET_GOLD, Tier::GOLD)
		->setIcon(4, 7)
		->setDescriptionId("hatchetGold");

	Item::goldShovel = NEW_X_ITEM(ShovelItem, ITEM_SHOVEL_GOLD, Tier::GOLD)
		->setIcon(4, 5)
		->setDescriptionId("shovelGold");

	Item::goldHoe = NEW_X_ITEM(HoeItem, ITEM_HOE_GOLD, Tier::GOLD)
		->setIcon(4, 8)
		->setDescriptionId("hoeGold");

	Item::diamondSword = NEW_X_ITEM(WeaponItem, ITEM_SWORD_DIAMOND, Tier::DIAMOND)
		->setIcon(3, 4)
		->setDescriptionId("swordDiamond");

	Item::diamondPickaxe = NEW_X_ITEM(PickaxeItem, ITEM_PICKAXE_DIAMOND, Tier::DIAMOND)
		->setIcon(3, 6)
		->setDescriptionId("pickaxeDiamond");

	Item::diamondAxe = NEW_X_ITEM(AxeItem, ITEM_HATCHET_DIAMOND, Tier::DIAMOND)
		->setIcon(3, 7)
		->setDescriptionId("hatchetDiamond");

	Item::diamondShovel = NEW_X_ITEM(ShovelItem, ITEM_SHOVEL_DIAMOND, Tier::DIAMOND)
		->setIcon(3, 5)
		->setDescriptionId("shovelDiamond");

	Item::diamondHoe = NEW_X_ITEM(HoeItem, ITEM_HOE_DIAMOND, Tier::DIAMOND)
		->setIcon(3, 8)
		->setDescriptionId("hoeDiamond");

	Item::clothHelmet = NEW_X_ITEM(ArmorItem, ITEM_HELMET_CLOTH, ArmorTier::CLOTH, HEAD)
		->setIcon(0, 0)
		->setDescriptionId("helmetCloth");

	Item::clothChestplate = NEW_X_ITEM(ArmorItem, ITEM_CHESTPLATE_CLOTH, ArmorTier::CLOTH, CHEST)
		->setIcon(0, 1)
		->setDescriptionId("chestplateCloth");

	Item::clothLeggings = NEW_X_ITEM(ArmorItem, ITEM_LEGGINGS_CLOTH, ArmorTier::CLOTH, LEGS)
		->setIcon(0, 2)
		->setDescriptionId("leggingsCloth");

	Item::clothBoots = NEW_X_ITEM(ArmorItem, ITEM_BOOTS_CLOTH, ArmorTier::CLOTH, FEET)
		->setIcon(0, 3)
		->setDescriptionId("bootsCloth");

	Item::chainHelmet = NEW_X_ITEM(ArmorItem, ITEM_HELMET_CHAIN, ArmorTier::CHAIN, HEAD)
		->setIcon(1, 0)
		->setDescriptionId("helmetChain");

	Item::chainChestplate = NEW_X_ITEM(ArmorItem, ITEM_CHESTPLATE_CHAIN, ArmorTier::CHAIN, CHEST)
		->setIcon(1, 1)
		->setDescriptionId("chestplateChain");

	Item::chainLeggings = NEW_X_ITEM(ArmorItem, ITEM_LEGGINGS_CHAIN, ArmorTier::CHAIN, LEGS)
		->setIcon(1, 2)
		->setDescriptionId("leggingsChain");

	Item::chainBoots = NEW_X_ITEM(ArmorItem, ITEM_BOOTS_CHAIN, ArmorTier::CHAIN, FEET)
		->setIcon(1, 3)
		->setDescriptionId("bootsChain");

	Item::ironHelmet = NEW_X_ITEM(ArmorItem, ITEM_HELMET_IRON, ArmorTier::IRON, HEAD)
		->setIcon(2, 0)
		->setDescriptionId("helmetIron");

	Item::ironChestplate = NEW_X_ITEM(ArmorItem, ITEM_CHESTPLATE_IRON, ArmorTier::IRON, CHEST)
		->setIcon(2, 1)
		->setDescriptionId("chestplateIron");

	Item::ironLeggings = NEW_X_ITEM(ArmorItem, ITEM_LEGGINGS_IRON, ArmorTier::IRON, LEGS)
		->setIcon(2, 2)
		->setDescriptionId("leggingsIron");

	Item::ironBoots = NEW_X_ITEM(ArmorItem, ITEM_BOOTS_IRON, ArmorTier::IRON, FEET)
		->setIcon(2, 3)
		->setDescriptionId("bootsIron");

	Item::goldHelmet = NEW_X_ITEM(ArmorItem, ITEM_HELMET_GOLD, ArmorTier::GOLD, HEAD)
		->setIcon(4, 0)
		->setDescriptionId("helmetGold");

	Item::goldChestplate = NEW_X_ITEM(ArmorItem, ITEM_CHESTPLATE_GOLD, ArmorTier::GOLD, CHEST)
		->setIcon(4, 1)
		->setDescriptionId("chestplateGold");

	Item::goldLeggings = NEW_X_ITEM(ArmorItem, ITEM_LEGGINGS_GOLD, ArmorTier::GOLD, LEGS)
		->setIcon(4, 2)
		->setDescriptionId("leggingsGold");

	Item::goldBoots = NEW_X_ITEM(ArmorItem, ITEM_BOOTS_GOLD, ArmorTier::GOLD, FEET)
		->setIcon(4, 3)
		->setDescriptionId("bootsGold");

	Item::diamondHelmet = NEW_X_ITEM(ArmorItem, ITEM_HELMET_DIAMOND, ArmorTier::DIAMOND, HEAD)
		->setIcon(3, 0)
		->setDescriptionId("helmetDiamond");

	Item::diamondChestplate = NEW_X_ITEM(ArmorItem, ITEM_CHESTPLATE_DIAMOND, ArmorTier::DIAMOND, CHEST)
		->setIcon(3, 1)
		->setDescriptionId("chestplateDiamond");

	Item::diamondLeggings = NEW_X_ITEM(ArmorItem, ITEM_LEGGINGS_DIAMOND, ArmorTier::DIAMOND, LEGS)
		->setIcon(3, 2)
		->setDescriptionId("leggingsDiamond");

	Item::diamondBoots = NEW_X_ITEM(ArmorItem, ITEM_BOOTS_DIAMOND, ArmorTier::DIAMOND, FEET)
		->setIcon(3, 3)
		->setDescriptionId("bootsDiamond");

	Item::flintAndSteel = NEW_X_ITEMN(FlintAndSteelItem, ITEM_FLINT_AND_STEEL)
		->setIcon(5, 0)
		->setDescriptionId("flintAndSteel");

	Item::arrow = NEW_ITEM(ITEM_ARROW)
		->setIcon(5, 2)
		->setDescriptionId("arrow");

	Item::coal = NEW_X_ITEMN(CoalItem, ITEM_COAL)
		->setIcon(7, 0)
		->setDescriptionId("coal");

	Item::diamond = NEW_ITEM(ITEM_DIAMOND)
		->setIcon(7, 3)
		->setDescriptionId("diamond");

	Item::ironIngot = NEW_ITEM(ITEM_INGOT_IRON)
		->setIcon(7, 1)
		->setDescriptionId("ingotIron");

	Item::goldIngot = NEW_ITEM(ITEM_INGOT_GOLD)
		->setIcon(7, 2)
		->setDescriptionId("ingotGold");

	Item::stick = NEW_ITEM(ITEM_STICK)
		->setIcon(5, 3)
		->handEquipped()
		->setDescriptionId("stick");

	Item::bowl = NEW_ITEM(ITEM_BOWL)
		->setIcon(7, 4)
		->setDescriptionId("bowl");

	Item::mushroomStew = NEW_X_ITEM(BowlFoodItem, ITEM_STEW_MUSHROOM, 10)
		->setIcon(8, 4)
		->setDescriptionId("mushroomStew");

	Item::string = NEW_ITEM(ITEM_STRING)
		->setIcon(8, 0)
		->setDescriptionId("string");

	Item::feather = NEW_ITEM(ITEM_FEATHER)
		->setIcon(8, 1)
		->setDescriptionId("feather");

	Item::sulphur = NEW_ITEM(ITEM_SULPHUR)
		->setIcon(8, 2)
		->setDescriptionId("sulphur");

	Item::seeds = NEW_X_ITEM(SeedItem, ITEM_SEEDS, TILE_WHEAT)
		->setIcon(9, 0)
		->setDescriptionId("seeds");

	Item::reeds = NEW_ITEM(ITEM_REEDS)
		->setIcon(11, 1)
		->setDescriptionId("reeds");

	Item::wheat = NEW_ITEM(ITEM_WHEAT)
		->setIcon(9, 1)
		->setDescriptionId("wheat");

	Item::bread = NEW_X_ITEM(FoodItem, ITEM_BREAD, 5)
		->setIcon(9, 2)
		->setDescriptionId("bread");

	Item::flint = NEW_ITEM(ITEM_FLINT)
		->setIcon(6, 0)
		->setDescriptionId("flint");

	Item::rawPorkchop = NEW_X_ITEM(FoodItem, ITEM_PORKCHOP_RAW, 3, true)
		->setIcon(7, 5)
		->setDescriptionId("porkchopRaw");

	Item::cookedPorkchop = NEW_X_ITEM(FoodItem, ITEM_PORKCHOP_COOKED, 8, true)
		->setIcon(8, 5)
		->setDescriptionId("porkchopCooked");

	Item::apple = NEW_X_ITEM(FoodItem, ITEM_APPLE, 4)
		->setIcon(10, 0)
		->setDescriptionId("appleGold");

	Item::goldApple = NEW_X_ITEM(FoodItem, ITEM_APPLE_GOLD, 42)
		->setIcon(11, 0)
		->setDescriptionId("appleGold");

	Item::woodDoor = NEW_X_ITEM(DoorItem, ITEM_DOOR_WOOD, Material::wood)
		->setIcon(11, 2)
		->setDescriptionId("doorWood");

	Item::emptyBucket = NEW_X_ITEM(BucketItem, ITEM_BUCKET, TILE_AIR)
		->setIcon(10, 4)
		->setDescriptionId("bucket");

	Item::waterBucket = NEW_X_ITEM(BucketItem, ITEM_BUCKET_WATER, TILE_WATER)
		->setIcon(11, 4)
		->setDescriptionId("bucketWater")
		->setCraftingRemainingItem(emptyBucket);

	Item::lavaBucket = NEW_X_ITEM(BucketItem, ITEM_BUCKET_LAVA, TILE_LAVA)
		->setIcon(12, 4)
		->setDescriptionId("bucketLava")
		->setCraftingRemainingItem(emptyBucket);

	Item::minecart = NEW_X_ITEM(MinecartItem, ITEM_MINECART, 0)
		->setIcon(7, 8)
		->setDescriptionId("minecart");

	Item::chestMinecart = NEW_X_ITEM(MinecartItem, ITEM_MINECART_CHEST, 1)
		->setIcon(7, 9)
		->setDescriptionId("minecartChest");

	Item::furnaceMinecart = NEW_X_ITEM(MinecartItem, ITEM_MINECART_FURNACE, 2)
		->setIcon(7, 10)
		->setDescriptionId("minecartFurnace");

	Item::boat = NEW_X_ITEMN(BoatItem, ITEM_BOAT)
		->setIcon(8, 8)
		->setDescriptionId("boat");

	Item::ironDoor = NEW_X_ITEM(DoorItem, ITEM_DOOR_IRON, Material::metal)
		->setIcon(12, 2)
		->setDescriptionId("doorIron");

	//@TODO RedStoneItem (maybe not)
	Item::redStone = NEW_X_ITEM(TilePlanterItem, ITEM_REDSTONE, TILE_WIRE, false)
		->setIcon(8, 3)
		->setDescriptionId("redstone");

	Item::snowBall = NEW_X_ITEMN(SnowballItem, ITEM_SNOWBALL)
		->setIcon(14, 0)
		->setDescriptionId("snowball");

	Item::saddle = NEW_X_ITEMN(SaddleItem, ITEM_SADDLE)
		->setIcon(8, 6)
		->setDescriptionId("saddle");

	Item::leather = NEW_ITEM(ITEM_LEATHER)
		->setIcon(7, 6)
		->setDescriptionId("leather");

	Item::milk = NEW_X_ITEM(BucketItem, ITEM_BUCKET_MILK, -1)
		->setIcon(13, 4)
		->setDescriptionId("milk")
		->setCraftingRemainingItem(emptyBucket);

	Item::brick = NEW_ITEM(ITEM_BRICK)
		->setIcon(6, 1)
		->setDescriptionId("brick");

	Item::clay = NEW_ITEM(ITEM_CLAY)
		->setIcon(9, 3)
		->setDescriptionId("clay");

	Item::reeds = NEW_X_ITEM(TilePlanterItem, ITEM_REEDS, TILE_REEDS)
		->setIcon(11, 1)
		->setDescriptionId("reeds");

	Item::paper = NEW_ITEM(ITEM_PAPER)
		->setIcon(10, 3)
		->setDescriptionId("paper");

	Item::book = NEW_ITEM(ITEM_BOOK)
		->setIcon(11, 3)
		->setDescriptionId("book");

	Item::slimeBall = NEW_ITEM(ITEM_SLIME_BALL)
		->setIcon(14, 1)
		->setDescriptionId("slimeball");

	Item::egg = NEW_X_ITEMN(EggItem, ITEM_EGG)
		->setIcon(12, 0)
		->setDescriptionId("egg");

	Item::compass = NEW_ITEM(ITEM_COMPASS)
		->setIcon(6, 3)
		->setDescriptionId("compass");

	Item::fishingRod = NEW_X_ITEMN(FishingRodItem, ITEM_FISHING_ROD)
		->setIcon(5, 4)
		->setDescriptionId("fishingRod");

	Item::dyePowder = NEW_X_ITEMN(DyeItem, ITEM_DYE_POWDER)
		->setIcon(14, 4)
		->setDescriptionId("dyePowder");

	Item::clock = NEW_ITEM(ITEM_CLOCK)
		->setIcon(6, 4)
		->setDescriptionId("clock");

	Item::yellowDust = NEW_ITEM(ITEM_YELLOW_DUST)
		->setIcon(9, 4)
		->setDescriptionId("yellowDust");

	Item::rawFish = NEW_X_ITEM(FoodItem, ITEM_FISH_RAW, 2)
		->setIcon(9, 5)
		->setDescriptionId("fishRaw");

	Item::cookedFish = NEW_X_ITEM(FoodItem, ITEM_FISH_COOKED, 5)
		->setIcon(10, 5)
		->setDescriptionId("fishCooked");

	Item::bone = NEW_ITEM(ITEM_BONE)
		->setIcon(12, 1)
		->setDescriptionId("bone")
		->handEquipped();

	Item::sugar = NEW_ITEM(ITEM_SUGAR)
		->setIcon(13, 0)
		->setDescriptionId("sugar")
		->handEquipped(); // weirdly also in JE (certainly they originally just copied the bone and forgot this lol)

	Item::cake = NEW_X_ITEM(TilePlanterItem, ITEM_CAKE, TILE_CAKE)
		->setIcon(13, 1)
		->setMaxStackSize(1)
		->setDescriptionId("cake");

	Item::bed = NEW_X_ITEMN(BedItem, ITEM_BED)
		->setIcon(13, 2)
		->setDescriptionId("bed");

	Item::diode = NEW_X_ITEM(TilePlanterItem, ITEM_DIODE, TILE_REPEATER_OFF)
		->setIcon(6, 5)
		->setDescriptionId("diode");

	Item::cookie = NEW_X_ITEM(FoodItem, ITEM_COOKIE, 1)
		->setMaxStackSize(8)
		->setIcon(12, 5)
		->setDescriptionId("cookie");

	Item::sign = NEW_X_ITEMN(SignItem, ITEM_SIGN)
		->setIcon(10, 2)
		->setDescriptionId("sign");

	Item::painting = NEW_X_ITEMN(PaintingItem, ITEM_PAINTING)
		->setIcon(10, 1)
		->setDescriptionId("painting");

	Item::map = NEW_X_ITEMN(MapItem, ITEM_MAP);
	Item::map
		->setIcon(12, 3)
		->setDescriptionId("map");

#ifdef ENH_b1_7
	Item::shears = NEW_X_ITEMN(ShearsItem, ITEM_SHEARS)
		->setIcon(13, 5)
		->setDescriptionId("shears");
#endif

	Item::record_01 = NEW_X_ITEM(RecordItem, ITEM_RECORD_01, "13")
		->setIcon(0, 15)
		->setDescriptionId("record");

	Item::record_02 = NEW_X_ITEM(RecordItem, ITEM_RECORD_02, "cat")
		->setIcon(1, 15)
		->setDescriptionId("record");
}

int Item::getIcon(const ItemInstance* pInstance) const
{
	return m_icon;
}

bool Item::useOn(ItemInstance* instance, Level* level, const TilePos& pos, Facing::Name face)
{
	return false;
}

bool Item::useOn(ItemInstance* instance, Player* player, Level* level, const TilePos& pos, Facing::Name face)
{
	return false;
}

float Item::getDestroySpeed(ItemInstance* instance, const Tile* tile)
{
	return 1.0f;
}

std::shared_ptr<ItemInstance> Item::use(std::shared_ptr<ItemInstance> instance, Level* level, Player* player)
{
	return instance;
}

int Item::getMaxStackSize()
{
	return m_maxStackSize;
}

int Item::getLevelDataForAuxValue(int x)
{
	return 0;
}

bool Item::isStackedByData()
{
	return m_bStackedByData;
}

int Item::getMaxDamage()
{
	return m_maxDamage;
}

void Item::hurtEnemy(ItemInstance* instance, Mob* mob)
{

}

void Item::mineBlock(ItemInstance* instance, int tile, const TilePos& pos, Facing::Name face, Player*)
{

}

int Item::getAttackDamage(Entity* ent)
{
	return 1;
}

bool Item::canDestroySpecial(const Tile*)
{
	return false;
}

void Item::interactEnemy(ItemInstance* instance, Mob* mob)
{

}

bool Item::isHandEquipped()
{
	return m_bHandEquipped;
}

bool Item::isMirroredArt()
{
	return false;
}

std::string Item::getDescription()
{
	return m_DescriptionID;
}

std::string Item::getDescription(ItemInstance* inst)
{
	return m_DescriptionID;
}

std::string Item::getDescriptionId()
{
	return m_DescriptionID;
}

std::string Item::getDescriptionId(ItemInstance* inst)
{
	return m_DescriptionID;
}

Item* Item::getCraftingRemainingItem()
{
	return m_pCraftingRemainingItem;
}

bool Item::hasCraftingRemainingItem()
{
	return m_pCraftingRemainingItem != 0;
}

std::string Item::getName()
{
	return getDescriptionId() + ".name";
}

void Item::onCraftedBy(const std::shared_ptr<ItemInstance>&, Player*, Level*)
{
}

void Item::inventoryTick(const std::shared_ptr<ItemInstance>&, Level*, Entity*, int, bool)
{
}

Item::Tier
	Item::Tier::WOOD   (0, 59,   2.0f,  0),
	Item::Tier::STONE  (1, 131,  4.0f,  1),
	Item::Tier::IRON   (2, 250,  6.0f,  2),
	Item::Tier::DIAMOND(3, 1561, 8.0f,  3),
	Item::Tier::GOLD   (0, 32,   12.0f, 0);

Item::ArmorTier
	Item::ArmorTier::CLOTH(0, "armor/cloth_1.png", "armor/cloth_2.png"),
	Item::ArmorTier::CHAIN(1, "armor/chain_1.png", "armor/chain_2.png"),
	Item::ArmorTier::IRON(2, "armor/iron_1.png", "armor/iron_2.png"),
	Item::ArmorTier::DIAMOND(3, "armor/diamond_1.png", "armor/diamond_2.png"),
	Item::ArmorTier::GOLD(1, "armor/gold_1.png", "armor/gold_2.png");

std::string Item::ICON_DESCRIPTION_PREFIX = "item.";

Item* Item::ironShovel;
Item* Item::ironPickaxe;
Item* Item::ironAxe;
Item* Item::flintAndSteel;
Item* Item::apple;
Item* Item::bow;
Item* Item::arrow;
Item* Item::coal;
Item* Item::diamond;
Item* Item::ironIngot;
Item* Item::goldIngot;
Item* Item::ironSword;
Item* Item::woodSword;
Item* Item::woodShovel;
Item* Item::woodPickaxe;
Item* Item::woodAxe;
Item* Item::stoneSword;
Item* Item::stoneShovel;
Item* Item::stonePickaxe;
Item* Item::stoneAxe;
Item* Item::diamondSword;
Item* Item::diamondShovel;
Item* Item::diamondPickaxe;
Item* Item::diamondAxe;
Item* Item::stick;
Item* Item::bowl;
Item* Item::mushroomStew;
Item* Item::goldSword;
Item* Item::goldShovel;
Item* Item::goldPickaxe;
Item* Item::goldAxe;
Item* Item::string;
Item* Item::feather;
Item* Item::sulphur;
Item* Item::woodHoe;
Item* Item::stoneHoe;
Item* Item::ironHoe;
Item* Item::diamondHoe;
Item* Item::goldHoe;
Item* Item::seeds;
Item* Item::wheat;
Item* Item::bread;
Item* Item::clothHelmet;
Item* Item::clothChestplate;
Item* Item::clothLeggings;
Item* Item::clothBoots;
Item* Item::chainHelmet;
Item* Item::chainChestplate;
Item* Item::chainLeggings;
Item* Item::chainBoots;
Item* Item::ironHelmet;
Item* Item::ironChestplate;
Item* Item::ironLeggings;
Item* Item::ironBoots;
Item* Item::diamondHelmet;
Item* Item::diamondChestplate;
Item* Item::diamondLeggings;
Item* Item::diamondBoots;
Item* Item::goldHelmet;
Item* Item::goldChestplate;
Item* Item::goldLeggings;
Item* Item::goldBoots;
Item* Item::flint;
Item* Item::rawPorkchop;
Item* Item::cookedPorkchop;
Item* Item::painting;
Item* Item::goldApple;
Item* Item::sign;
Item* Item::woodDoor;
Item* Item::emptyBucket;
Item* Item::waterBucket;
Item* Item::lavaBucket;
Item* Item::minecart;
Item* Item::saddle;
Item* Item::ironDoor;
Item* Item::redStone;
Item* Item::snowBall;
Item* Item::boat;
Item* Item::leather;
Item* Item::milk;
Item* Item::brick;
Item* Item::clay;
Item* Item::reeds;
Item* Item::paper;
Item* Item::book;
Item* Item::slimeBall;
Item* Item::chestMinecart;
Item* Item::furnaceMinecart;
Item* Item::egg;
Item* Item::compass;
Item* Item::fishingRod;
Item* Item::clock;
Item* Item::yellowDust;
Item* Item::rawFish;
Item* Item::cookedFish;
Item* Item::dyePowder;
Item* Item::bone;
Item* Item::sugar;
Item* Item::cake;
Item* Item::bed;
Item* Item::diode;
Item* Item::cookie;
MapItem* Item::map;
#ifdef ENH_b1_7
Item* Item::shears;
#endif
Item* Item::record_01;
Item* Item::record_02;