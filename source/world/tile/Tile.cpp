/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "world/level/Level.hpp"
#include "world/item/TileItem.hpp"
#include "world/item/ClothItem.hpp"
#include "world/item/AuxTileItem.hpp"
#include "world/item/PistonItem.hpp"
#include "world/item/SlabItem.hpp"
#include "world/entity/ItemEntity.hpp"

// Include tile definitions here
#include "SandStoneTile.hpp"
#include "SandTile.hpp"
#include "HalfTransparentTile.hpp"
#include "GlassTile.hpp"
#include "GravelTile.hpp"
#include "StoneTile.hpp"
#include "DirtTile.hpp"
#include "GrassTile.hpp"
#include "IceTile.hpp"
#include "ClayTile.hpp"
#include "FarmTile.hpp"
#include "StoneSlabTile.hpp"
#include "ClothTile.hpp"
#include "Bush.hpp"
#include "Sapling.hpp"
#include "TopSnowTile.hpp"
#include "TreeTile.hpp"
#include "TransparentTile.hpp"
#include "LeafTile.hpp"
#include "OreTile.hpp"
#include "RedStoneOreTile.hpp"
#include "ReedTile.hpp"
#include "InvisibleTile.hpp"
#include "LiquidTileDynamic.hpp"
#include "LiquidTileStatic.hpp"
#include "ObsidianTile.hpp"
#include "MetalTile.hpp"
#include "StairTile.hpp"
#include "TorchTile.hpp"
#include "FireTile.hpp"
#include "LadderTile.hpp"
#include "TntTile.hpp"
#include "DoorTile.hpp"
#include "SpongeTile.hpp"
#include "BookshelfTile.hpp"
#include "RedStoneDustTile.hpp"
#include "CraftingTableTile.hpp"
#include "FurnaceTile.hpp"
#include "TallGrass.hpp"
#include "DeadBush.hpp"
#include "CactusTile.hpp"
#include "ChestTile.hpp"
#include "PumpkinTile.hpp"
#include "SoulSandTile.hpp"
#include "GlowstoneTile.hpp"
#include "FenceTile.hpp"
#include "BedTile.hpp"
#include "CropsTile.hpp"
#include "Web.hpp"
#include "SnowTile.hpp"
#include "SignTile.hpp"
#include "LeverTile.hpp"
#include "PressurePlateTile.hpp"
#include "RailTile.hpp"
#include "DetectorRailTile.hpp"
#include "ButtonTile.hpp"
#include "MobSpawnerTile.hpp"
#include "RedstoneTorchTile.hpp"
#include "CakeTile.hpp"
#include "DispenserTile.hpp"
#include "MusicTile.hpp"
#include "RecordPlayerTile.hpp"
#include "TrapDoorTile.hpp"
#include "PortalTile.hpp"
#include "RepeaterTile.hpp"
#include "Mushroom.hpp"
#include "PistonBaseTile.hpp"
#include "PistonHeadTile.hpp"
#include "MovingPistonTile.hpp"

std::string Tile::TILE_DESCRIPTION_PREFIX = "tile.";

Tile* Tile::tiles        [C_MAX_TILES];
int   Tile::lightBlock   [C_MAX_TILES];
int   Tile::lightEmission[C_MAX_TILES];
bool  Tile::shouldTick   [C_MAX_TILES];
bool  Tile::solid        [C_MAX_TILES];
bool  Tile::translucent  [C_MAX_TILES];
bool  Tile::isEntityTile [C_MAX_TILES];
bool  Tile::blockUpdate  [C_MAX_TILES];


void Tile::_init()
{
	m_TextureFrame = 1;
	m_pSound = nullptr;
	m_gravity = 1.0f;
	m_friction = 0.6f;
	m_hardness = 0.0f;
	m_blastResistance = 0.0f;
	m_descriptionID = "";
}

void Tile::_init(int ID, Material* pMaterial, int texture)
{
	_init();

	m_ID = ID;
	m_TextureFrame = texture;
	m_pMaterial = pMaterial;
	m_pSound = &SOUND_NORMAL;
	m_aabb = m_aabbReturned = AABB(0, 0, 0, 1, 1, 1);

	if (tiles[m_ID])
		// @BUG: Printing &tiles[m_ID], but probably supposed to print tiles[m_ID]
		LOG_W("Slot %d is already occupied by %p when adding %p", m_ID, &tiles[m_ID], this);
}

Tile::~Tile()
{
}

bool Tile::isCubeShaped() const
{
	return true;
}

std::string Tile::getDescriptionId() const
{
	return m_descriptionID;
}

std::string Tile::getName() const
{
	return getDescriptionId();
}

Tile* Tile::setSoundType(const Tile::SoundType& st)
{
	m_pSound = &st;
	return this;
}

Tile* Tile::setDescriptionId(const std::string& x)
{
	m_descriptionID = TILE_DESCRIPTION_PREFIX + x;
	return this;
}

Tile* Tile::setDestroyTime(float time)
{
	m_hardness = time;

	if (m_blastResistance < 5 * time)
		m_blastResistance = 5 * time;

	return this;
}

Tile* Tile::setExplodeable(float power)
{
	m_blastResistance = power;
	return this;
}

Tile* Tile::setLightBlock(int x)
{
	lightBlock[m_ID] = x;
	return this;
}

Tile* Tile::setLightEmission(float x)
{
	lightEmission[m_ID] = int(15 * x);
	return this;
}

Tile* Tile::setTicking(bool bTick)
{
	shouldTick[m_ID] = bTick;
	return this;
}

Tile* Tile::setBlockUpdate()
{
	blockUpdate[m_ID] = true;
	return this;
}

Tile* Tile::setShape(float a, float b, float c, float d, float e, float f)
{
	m_aabb = AABB(a, b, c, d, e, f);

	return this;
}

Tile* Tile::init()
{
	setShape(m_aabb.min.x, m_aabb.min.y, m_aabb.min.z, m_aabb.max.x, m_aabb.max.y, m_aabb.max.z);

	tiles[m_ID] = this;

	solid[m_ID] = isSolidRender();
	lightBlock[m_ID] = isSolidRender() ? 255 : 0;
	translucent[m_ID] = m_pMaterial->blocksLight();
	isEntityTile[m_ID] = hasTileEntity();


	return this;
}

int Tile::getRenderShape() const
{
	return 0;
}

void Tile::updateDefaultShape()
{
	
}

int Tile::getTexture(Facing::Name face) const
{
	return m_TextureFrame;
}

int Tile::getTexture(Facing::Name face, int data) const
{
	return getTexture(face);
}

int Tile::getTickDelay() const
{
	return 10;
}

bool Tile::isSignalSource() const
{
	return false;
}

int Tile::getRenderLayer() const
{
	return LAYER_OPAQUE;
}

bool Tile::isSolidRender() const
{
	return true;
}

bool Tile::mayPick() const
{
	return true;
}


bool Tile::hasTileEntity() const
{
	return false;
}

bool Tile::mayPick(int x, bool y) const
{
	return mayPick();
}

bool Tile::mayPlace(const Level* level, const TilePos& pos, Facing::Name face) const
{
	return mayPlace(level, pos);
}

int Tile::getResource(int x, Random* pRandom) const
{
	return m_ID;
}

int Tile::getResourceCount(Random* pRandom) const
{
	return 1;
}

int Tile::getSpawnResourcesAuxValue(int x) const
{
	return 0;
}

std::shared_ptr<TileEntity> Tile::newTileEntity()
{
	return nullptr;
}

PushReaction Tile::getPistonPushReaction()
{
	return m_pMaterial->getPushReaction();
}

void Tile::initTiles()
{
	Tile::stone = (new StoneTile(TILE_STONE, TEXTURE_STONE, Material::stone))
		->init()
		->setDestroyTime(1.5f)
		->setExplodeable(10.0f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("stone");

	Tile::grass = (new GrassTile(TILE_GRASS, Material::grass))
		->init()
		->setDestroyTime(0.6f)
		->setSoundType(Tile::SOUND_GRASS)
		->setDescriptionId("grass");

	Tile::dirt = (new DirtTile(TILE_DIRT, TEXTURE_DIRT, Material::dirt))
		->init()
		->setDestroyTime(0.5f)
		->setSoundType(Tile::SOUND_GRAVEL)
		->setDescriptionId("dirt");

	Tile::cobblestone = (new Tile(TILE_COBBLESTONE, TEXTURE_COBBLESTONE, Material::stone))
		->init()
		->setDestroyTime(2.0f)
		->setExplodeable(10.0f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("cobblestone");

	Tile::wood = (new Tile(TILE_WOOD, TEXTURE_PLANKS, Material::wood))
		->init()
		->setDestroyTime(2.0f)
		->setExplodeable(5.0f)
		->setSoundType(Tile::SOUND_WOOD)
		->setDescriptionId("wood")
		->setBlockUpdate();

	Tile::bedrock = (new Tile(TILE_BEDROCK, TEXTURE_BEDROCK, Material::stone))
		->init()
		->setDestroyTime(-1.0f)
		->setExplodeable(6000000.f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("bedrock");

	Tile::water = (new LiquidTileDynamic(TILE_WATER, Material::water))
		->init()
		->setDestroyTime(100.0f)
		->setLightBlock(3)
		->setDescriptionId("water")
		->setBlockUpdate();

	Tile::calmWater = (new LiquidTileStatic(TILE_WATER_CALM, Material::water))
		->init()
		->setDestroyTime(100.0f)
		->setLightBlock(3)
		->setDescriptionId("water")
		->setBlockUpdate();

	Tile::lava = (new LiquidTileDynamic(TILE_LAVA, Material::lava))
		->init()
		->setDestroyTime(0.0f)
		->setLightEmission(1.0f)
		->setLightBlock(255)
		->setDescriptionId("lava")
		->setBlockUpdate();

	Tile::calmLava = (new LiquidTileStatic(TILE_LAVA_CALM, Material::lava))
		->init()
		->setDestroyTime(100.0f)
		->setLightEmission(1.0f)
		->setLightBlock(255)
		->setDescriptionId("lava")
		->setBlockUpdate();

	Tile::sand = (new SandTile(TILE_SAND, TEXTURE_SAND, Material::sand))
		->init()
		->setDestroyTime(0.5f)
		->setSoundType(Tile::SOUND_SAND)
		->setDescriptionId("sand");

	Tile::gravel = (new GravelTile(TILE_GRAVEL, TEXTURE_GRAVEL, Material::sand))
		->init()
		->setDestroyTime(0.6f)
		->setSoundType(Tile::SOUND_GRAVEL)
		->setDescriptionId("gravel");

	Tile::goldOre = (new OreTile(TILE_ORE_GOLD, TEXTURE_ORE_GOLD))
		->init()
		->setDestroyTime(3.0f)
		->setExplodeable(5.0f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("oreGold");

	Tile::ironOre = (new OreTile(TILE_ORE_IRON, TEXTURE_ORE_IRON))
		->init()
		->setDestroyTime(3.0f)
		->setExplodeable(5.0f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("oreIron");

	Tile::coalOre = (new OreTile(TILE_ORE_COAL, TEXTURE_ORE_COAL))
		->init()
		->setDestroyTime(3.0f)
		->setExplodeable(5.0f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("oreCoal");

	Tile::treeTrunk = (new TreeTile(TILE_TREE_TRUNK))
		->init()
		->setDestroyTime(2.0f)
		->setSoundType(Tile::SOUND_WOOD)
		->setDescriptionId("log")
		->setBlockUpdate();

	Tile::leaves = (new LeafTile(TILE_LEAVES))
		->init()
		->setDestroyTime(0.2f)
		->setLightBlock(true)
		->setSoundType(Tile::SOUND_GRASS)
		->setDescriptionId("leaves")
		->setBlockUpdate();

	Tile::glass = (new GlassTile(TILE_GLASS, TEXTURE_GLASS, Material::glass))
		->init()
		->setDestroyTime(0.3f)
		->setSoundType(Tile::SOUND_GLASS)
		->setDescriptionId("glass");

	Tile::lapisOre = (new OreTile(TILE_ORE_LAPIS, TEXTURE_ORE_LAPIS))
		->init()
		->setDestroyTime(3.0f)
		->setExplodeable(5.0f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("oreLapis");

	Tile::lapisBlock = (new MetalTile(TILE_BLOCK_LAPIS, TEXTURE_LAPIS, Material::metal))
		->init()
		->setDestroyTime(3.0f)
		->setExplodeable(10.0f)
		->setSoundType(Tile::SOUND_METAL)
		->setDescriptionId("blockLapis");

	Tile::sandStone = (new SandStoneTile(TILE_SANDSTONE, TEXTURE_SANDSTONE_SIDE, Material::stone))
		->init()
		->setSoundType(Tile::SOUND_STONE)
		->setDestroyTime(0.8f)
		->setDescriptionId("sandStone");

	Tile::tallGrass = (new TallGrass(TILE_TALL_GRASS, TEXTURE_TALL_GRASS))
		->init()
		->setSoundType(Tile::SOUND_GRASS)
		->setDestroyTime(0.0f)
		->setDescriptionId("tallGrass");

	Tile::deadBush = (new DeadBush(TILE_DEAD_BUSH, TEXTURE_DEAD_BUSH))
		->init()
		->setSoundType(Tile::SOUND_GRASS)
		->setDestroyTime(0.0f)
		->setDescriptionId("deadBush")
		->setBlockUpdate();

	Tile::cloth = (new ClothTile(TILE_CLOTH))
		->init()
		->setDestroyTime(0.8f)
		->setSoundType(Tile::SOUND_CLOTH)
		->setDescriptionId("cloth")
		->setBlockUpdate();

	Tile::flower = (new Bush(TILE_FLOWER, TEXTURE_FLOWER))
		->init()
		->setDestroyTime(0.0f)
		->setSoundType(Tile::SOUND_GRASS)
		->setDescriptionId("flower");

	Tile::rose = (new Bush(TILE_ROSE, TEXTURE_ROSE))
		->init()
		->setDestroyTime(0.0f)
		->setSoundType(Tile::SOUND_GRASS)
		->setDescriptionId("rose");

	Tile::mushroom1 = (new Mushroom(TILE_MUSHROOM_1, TEXTURE_MUSHROOM_BROWN))
		->init()
		->setDestroyTime(0.0f)
		->setSoundType(Tile::SOUND_GRASS)
		->setLightEmission(0.125f)
		->setDescriptionId("mushroom");

	Tile::mushroom2 = (new Mushroom(TILE_MUSHROOM_2, TEXTURE_MUSHROOM_RED))
		->init()
		->setDestroyTime(0.0f)
		->setSoundType(Tile::SOUND_GRASS)
		->setDescriptionId("mushroom");

	Tile::goldBlock = (new MetalTile(TILE_BLOCK_GOLD, TEXTURE_GOLD, Material::metal))
		->init()
		->setDestroyTime(3.0f)
		->setExplodeable(10.0f)
		->setSoundType(Tile::SOUND_METAL)
		->setDescriptionId("blockGold");

	Tile::ironBlock = (new MetalTile(TILE_BLOCK_IRON, TEXTURE_IRON, Material::metal))
		->init()
		->setDestroyTime(5.0f)
		->setExplodeable(10.0f)
		->setSoundType(Tile::SOUND_METAL)
		->setDescriptionId("blockIron");

	Tile::stoneSlab = (new StoneSlabTile(TILE_STONESLAB_FULL, true))
		->init()
		->setDestroyTime(2.0f)
		->setExplodeable(10.0f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("stoneSlab");

	Tile::stoneSlabHalf = (new StoneSlabTile(TILE_STONESLAB_HALF, false))
		->init()
		->setDestroyTime(2.0f)
		->setExplodeable(10.0f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("stoneSlab");

	Tile::brick = (new Tile(TILE_BRICKS, TEXTURE_BRICKS, Material::stone))
		->init()
		->setDestroyTime(2.0f)
		->setExplodeable(10.0f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("brick");

	Tile::tnt = (new TntTile(TILE_TNT, TEXTURE_TNT_SIDE))
		->init()
		->setDestroyTime(0.0f)
		->setSoundType(Tile::SOUND_GRASS)
		->setDescriptionId("tnt");

	Tile::bookshelf = (new BookshelfTile(TILE_BOOKSHELF, TEXTURE_BOOKSHELF, Material::wood))
		->init()
		->setDestroyTime(1.5f)
		->setSoundType(Tile::SOUND_WOOD)
		->setDescriptionId("bookshelf");

	Tile::mossStone = (new Tile(TILE_MOSS_STONE, TEXTURE_MOSSY_STONE, Material::stone))
		->init()
		->setDestroyTime(2.0f)
		->setExplodeable(10.0f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("stoneMoss");

	Tile::obsidian = (new ObsidianTile(TILE_OBSIDIAN, TEXTURE_OBSIDIAN, Material::stone))
		->init()
		->setDestroyTime(10.0f)
		->setExplodeable(2000.0f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("obsidian");

	Tile::torch = (new TorchTile(TILE_TORCH, TEXTURE_TORCH_LIT, Material::decoration))
		->init()
		->setDestroyTime(0.0f)
		->setLightEmission(15.0f / 16.0f)
		->setSoundType(Tile::SOUND_WOOD)
		->setDescriptionId("torch")
		->setBlockUpdate();

	Tile::stairsWood = (new StairTile(TILE_STAIRS_WOOD, Tile::wood))
		->init()
		->setDescriptionId("stairsWood")
		->setBlockUpdate();

	Tile::diamondOre = (new OreTile(TILE_ORE_DIAMOND, TEXTURE_ORE_DIAMOND))
		->init()
		->setDestroyTime(3.0f)
		->setExplodeable(5.0f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("oreDiamond");

	Tile::diamondBlock = (new MetalTile(TILE_BLOCK_DIAMOND, TEXTURE_DIAMOND, Material::metal))
		->init()
		->setDestroyTime(5.0f)
		->setExplodeable(10.0f)
		->setSoundType(Tile::SOUND_METAL)
		->setDescriptionId("blockDiamond");

	Tile::farmland = (new FarmTile(TILE_FARMLAND, Material::dirt))
		->init()
		->setDestroyTime(0.6f)
		->setSoundType(Tile::SOUND_GRAVEL)
		->setDescriptionId("farmland");

	Tile::doorWood = (new DoorTile(TILE_DOOR_WOOD, Material::wood))
		->init()
		->setDestroyTime(3.0f)
		->setSoundType(Tile::SOUND_WOOD)
		->setDescriptionId("doorWood")
		->setBlockUpdate();

	Tile::ladder = (new LadderTile(TILE_LADDER, TEXTURE_LADDER))
		->init()
		->setDestroyTime(0.4f)
		->setSoundType(Tile::SOUND_WOOD)
		->setDescriptionId("ladder")
		->setBlockUpdate();

	Tile::rail = (new RailTile(TILE_RAIL, TEXTURE_RAIL, false))
		->init()
		->setDestroyTime(0.7f)
		->setSoundType(Tile::SOUND_METAL)
		->setDescriptionId("rail")
		->setBlockUpdate();

	Tile::poweredRail = (new RailTile(TILE_RAIL_POWERED, TEXTURE_POWERED_RAIL, true))
		->init()
		->setDestroyTime(0.7f)
		->setSoundType(Tile::SOUND_METAL)
		->setDescriptionId("goldenRail")
		->setBlockUpdate();

	Tile::detectorRail = (new DetectorRailTile(TILE_RAIL_ACTIVATOR, TEXTURE_DETECTOR_RAIL))
		->init()
		->setDestroyTime(0.7f)
		->setSoundType(Tile::SOUND_METAL)
		->setDescriptionId("detectorRail")
		->setBlockUpdate();

	Tile::stonePressurePlate = (new PressurePlateTile(TILE_PLATE_STONE, TEXTURE_STONE, PressurePlateTile::mobs))
		->init()
		->setDestroyTime(0.5f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("pressurePlate")
		->setBlockUpdate();

	Tile::woodPressurePlate = (new PressurePlateTile(TILE_PLATE_WOOD, TEXTURE_PLANKS, PressurePlateTile::players))
		->init()
		->setDestroyTime(0.5f)
		->setSoundType(Tile::SOUND_WOOD)
		->setDescriptionId("pressurePlate")
		->setBlockUpdate();

	Tile::lever = (new LeverTile(TILE_LEVER, TEXTURE_LEVER))
		->init()
		->setDestroyTime(0.5f)
		->setSoundType(Tile::SOUND_WOOD)
		->setDescriptionId("lever")
		->setBlockUpdate();

	Tile::stairsStone = (new StairTile(TILE_STAIRS_STONE, Tile::cobblestone))
		->init()
		->setDescriptionId("stairsStone")
		->setBlockUpdate();

	Tile::sign = (new SignTile(TILE_SIGN, false))
		->init()
		->setDestroyTime(1.0f)
		->setSoundType(Tile::SOUND_WOOD)
		->setDescriptionId("sign")
		->setBlockUpdate();

	Tile::wallSign = (new SignTile(TILE_SIGN_WALL, true))
		->init()
		->setDestroyTime(1.0f)
		->setSoundType(Tile::SOUND_WOOD)
		->setDescriptionId("sign")
		->setBlockUpdate();

	Tile::doorIron = (new DoorTile(TILE_DOOR_IRON, Material::metal))
		->init()
		->setDestroyTime(5.0f)
		->setSoundType(Tile::SOUND_METAL)
		->setDescriptionId("doorIron")
		->setBlockUpdate();

	Tile::redstoneOre = (new RedStoneOreTile(TILE_ORE_REDSTONE, TEXTURE_ORE_RED_STONE, false))
		->init()
		->setDestroyTime(3.0f)
		->setExplodeable(5.0f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("oreRedstone")
		->setBlockUpdate();

	Tile::redstoneOreLit = (new RedStoneOreTile(TILE_ORE_REDSTONE_LIT, TEXTURE_ORE_RED_STONE, true))
		->init()
		->setDestroyTime(3.0f)
		->setLightEmission(0.625f)
		->setExplodeable(5.0f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("oreRedstone")
		->setBlockUpdate();

	Tile::redstoneTorch = (new RedstoneTorchTile(TILE_NOT_GATE_OFF, TEXTURE_TORCH_RED_STONE_OFF, Material::decoration, false))
		->init()
		->setDestroyTime(0.0f)
		->setSoundType(Tile::SOUND_WOOD)
		->setDescriptionId("notGate")
		->setBlockUpdate();

	Tile::redstoneTorchLit = (new RedstoneTorchTile(TILE_NOT_GATE_ON, TEXTURE_TORCH_RED_STONE, Material::decoration, true))
		->init()
		->setDestroyTime(0.0f)
		->setLightEmission(0.5f)
		->setSoundType(Tile::SOUND_WOOD)
		->setDescriptionId("notGate")
		->setBlockUpdate();

	Tile::button = (new ButtonTile(TILE_BUTTON_STONE, TEXTURE_STONE))
		->init()
		->setDestroyTime(0.5f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("button")
		->setBlockUpdate();

	Tile::topSnow = (new TopSnowTile(TILE_TOPSNOW, TEXTURE_SNOW, Material::topSnow))
		->init()
		->setDestroyTime(0.1f)
		->setSoundType(Tile::SOUND_CLOTH)
		->setDescriptionId("snow");
	
	Tile::ice = (new IceTile(TILE_ICE, TEXTURE_ICE, Material::ice))
		->init()
		->setDestroyTime(0.5f)
		->setLightBlock(3)
		->setSoundType(Tile::SOUND_GLASS)
		->setDescriptionId("ice");

	Tile::snow = (new SnowTile(TILE_SNOW, TEXTURE_SNOW))
		->init()
		->setDestroyTime(0.2f)
		->setSoundType(Tile::SOUND_CLOTH)
		->setDescriptionId("snow");

	Tile::clay = (new ClayTile(TILE_CLAY, TEXTURE_CLAY, Material::clay))
		->init()
		->setDestroyTime(0.6f)
		->setSoundType(Tile::SOUND_GRAVEL)
		->setDescriptionId("clay");

	Tile::cactus = (new CactusTile(TILE_CACTUS, TEXTURE_CACTUS_SIDE))
		->init()
		->setDestroyTime(0.4f)
		->setSoundType(Tile::SOUND_CLOTH)
		->setDescriptionId("cactus");

	Tile::reeds = (new ReedTile(TILE_REEDS))
		->init()
		->setDestroyTime(0.0f)
		->setSoundType(Tile::SOUND_GRASS)
		->setDescriptionId("reeds");

	Tile::fence = (new FenceTile(TILE_FENCE, TEXTURE_PLANKS))
		->init()
		->setDestroyTime(2.0f)
		->setExplodeable(5.0f)
		->setSoundType(Tile::SOUND_WOOD)
		->setDescriptionId("fence")
		->setBlockUpdate();

	Tile::pumpkin = (new PumpkinTile(TILE_PUMPKIN, false))
		->init()
		->setDestroyTime(1.0f)
		->setSoundType(Tile::SOUND_WOOD)
		->setDescriptionId("pumpkin")
		->setBlockUpdate();

	Tile::netherrack = (new Tile(TILE_NETHERRACK, TEXTURE_HELLROCK, Material::stone))
		->init()
		->setDestroyTime(0.4f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("hellRock");

	Tile::soulSand = (new SoulSandTile(TILE_SOUL_SAND, TEXTURE_SOULSAND))
		->init()
		->setDestroyTime(0.5f)
		->setSoundType(Tile::SOUND_SAND)
		->setDescriptionId("hellSand");

	Tile::glowstone = (new GlowstoneTile(TILE_GLOWSTONE, TEXTURE_GLOWSTONE, Material::stone))
		->init()
		->setDestroyTime(0.3f)
		->setLightEmission(1.0f)
		->setSoundType(Tile::SOUND_GLASS)
		->setDescriptionId("lightGem");

	Tile::pumpkinLantern = (new PumpkinTile(TILE_PUMPKIN_LIT, true))
		->init()
		->setDestroyTime(1.0f)
		->setLightEmission(1.0f)
		->setSoundType(Tile::SOUND_WOOD)
		->setDescriptionId("litPumpkin")
		->setBlockUpdate();

	Tile::fire = (new FireTile(TILE_FIRE, TEXTURE_FIRE1))
		->init()
		->setDestroyTime(0.0f)
		->setLightEmission(1.0f)
		->setSoundType(Tile::SOUND_WOOD)
		->setDescriptionId("fire")
		->setBlockUpdate();

	Tile::dispenser = (new DispenserTile(TILE_DISPENSER, TEXTURE_FURNACE_SIDE))
		->init()
		->setDestroyTime(3.5f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("dispenser")
		->setBlockUpdate();

	Tile::musicBlock = (new MusicTile(TILE_NOTE_BLOCK, TEXTURE_JUKEBOX_SIDE))
		->init()
		->setSoundType(Tile::SOUND_WOOD)
		->setDestroyTime(0.8f)
		->setDescriptionId("musicBlock")
		->setBlockUpdate();

	Tile::recordPlayer = new RecordPlayerTile(TILE_JUKEBOX, TEXTURE_JUKEBOX_SIDE);
	recordPlayer
		->init()
		->setSoundType(Tile::SOUND_WOOD)
		->setDestroyTime(2.0f)
		->setExplodeable(10.0f)
		->setDescriptionId("jukebox")
		->setBlockUpdate();

	Tile::mobSpawner = (new MobSpawnerTile(TILE_SPAWNER, TEXTURE_SPAWNER))
		->init()
		->setDestroyTime(5.0f)
		->setSoundType(Tile::SOUND_METAL)
		->setDescriptionId("mobSpawner");

	Tile::chest = (new ChestTile(TILE_CHEST, TEXTURE_CHEST_ONE_SIDE))
		->init()
		->setDestroyTime(2.5f)
		->setSoundType(Tile::SOUND_WOOD)
		->setDescriptionId("chest")
		->setBlockUpdate();

	Tile::redstoneDust = (new RedStoneDustTile(TILE_WIRE, TEXTURE_REDSTONE_DUST))
		->init()
		->setDestroyTime(0.0f)
		->setSoundType(Tile::SOUND_NORMAL)
		->setDescriptionId("redstoneDust")
		->setBlockUpdate();

	Tile::sapling = (new Sapling(TILE_SAPLING, TEXTURE_SAPLING));
	Tile::sapling
		->init()
		->setDestroyTime(0.0f)
		->setSoundType(Tile::SOUND_GRASS)
		->setDescriptionId("sapling")
		->setBlockUpdate();

	Tile::sponge = (new SpongeTile(TILE_SPONGE, TEXTURE_SPONGE))
		->init()
		->setDestroyTime(0.5f)
		->setSoundType(Tile::SOUND_CLOTH)
		->setDescriptionId("sponge");

	Tile::bed = (new BedTile(TILE_BED, 134))
		->init()
		->setDestroyTime(0.2f)
		->setSoundType(Tile::SOUND_CLOTH)
		->setDescriptionId("bed")
		->setBlockUpdate();

	Tile::web = (new Web(TILE_COBWEB, TEXTURE_COBWEB))
		->init()
		->setDestroyTime(4.0f)
		->setLightBlock(1)
		->setSoundType(Tile::SOUND_CLOTH)
		->setDescriptionId("web");

	Tile::craftingTable = (new CraftingTableTile(TILE_WORKBENCH))
		->init()
		->setDestroyTime(2.5f)
		->setSoundType(Tile::SOUND_WOOD)
		->setDescriptionId("craftingTable");

	Tile::crops = (new CropsTile(TILE_WHEAT, TEXTURE_WHEAT_0))
		->init()
		->setDestroyTime(0.0f)
		->setSoundType(Tile::SOUND_GRASS)
		->setDescriptionId("crops")
		->setBlockUpdate();

	Tile::furnace = (new FurnaceTile(TILE_FURNACE, false))
		->init()
		->setDestroyTime(3.5f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("furnace")
		->setBlockUpdate();

	Tile::furnaceLit = (new FurnaceTile(TILE_FURNACE_LIT, true))
		->init()
		->setLightEmission(14.0f / 16.0f)
		->setDestroyTime(3.5f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("furnace")
		->setBlockUpdate();

	Tile::cake = (new CakeTile(TILE_CAKE, TEXTURE_CAKE_TOP))
		->init()
		->setDestroyTime(0.5f)
		->setSoundType(Tile::SOUND_CLOTH)
		->setDescriptionId("cake")
		->setBlockUpdate();

	Tile::repeater = (new RepeaterTile(TILE_REPEATER_OFF, false))
		->init()
		->setDestroyTime(0.0f)
		->setSoundType(Tile::SOUND_WOOD)
		->setDescriptionId("diode")
		->setBlockUpdate();

	Tile::repeaterLit = (new RepeaterTile(TILE_REPEATER_ON, true))
		->init()
		->setDestroyTime(0.0f)
		->setLightEmission(0.625f)
		->setSoundType(Tile::SOUND_WOOD)
		->setDescriptionId("diode")
		->setTicking(true)
		->setBlockUpdate();

	Tile::trapDoor = (new TrapDoorTile(TILE_TRAPDOOR, Material::wood))
		->init()
		->setDestroyTime(3.0f)
		->setSoundType(Tile::SOUND_WOOD)
		->setDescriptionId("trapdoor")
		->setBlockUpdate();

	Tile::portal = new PortalTile(TILE_PORTAL, TEXTURE_PORTAL);
	portal->init()
		->setDestroyTime(-1.0f)
		->setSoundType(Tile::SOUND_GLASS)
		->setLightEmission(0.75f)
		->setDescriptionId("portal");

	Tile::piston = (new PistonBaseTile(TILE_PISTON, TEXTURE_PISTON, false))
		->init()
		->setDescriptionId("pistonBase")
		->setBlockUpdate();

	Tile::stickyPiston = (new PistonBaseTile(TILE_PISTON_STICKY, TEXTURE_STICKY_PISTON, true))
		->init()
		->setDescriptionId("pistonStickyBase")
		->setBlockUpdate();

	Tile::pistonHead = new PistonHeadTile(TILE_PISTON_HEAD, TEXTURE_PISTON);
	pistonHead
		->init()
		->setBlockUpdate();

	Tile::movingPiston = (new MovingPistonTile(TILE_PISTON_MOVING));
	movingPiston->init();

	Item::items[Tile::cloth->m_ID] = (new ClothItem(Tile::cloth->m_ID - C_MAX_TILES))
		->setDescriptionId("cloth");

	Item::items[Tile::stoneSlabHalf->m_ID] = (new SlabItem(Tile::stoneSlabHalf->m_ID - C_MAX_TILES))
		->setDescriptionId("stoneSlab");

	Item::items[Tile::treeTrunk->m_ID] = (new AuxTileItem(Tile::treeTrunk->m_ID - C_MAX_TILES))
		->setDescriptionId("log");

	Item::items[Tile::leaves->m_ID] = (new AuxTileItem(Tile::leaves->m_ID - C_MAX_TILES))
		->setDescriptionId("leaves");

	Item::items[Tile::sapling->m_ID] = (new AuxTileItem(Tile::sapling->m_ID - C_MAX_TILES))
		->setDescriptionId("sapling");

	Item::items[Tile::piston->m_ID] = (new PistonItem(Tile::piston->m_ID - C_MAX_TILES));

	Item::items[Tile::stickyPiston->m_ID] = (new PistonItem(Tile::stickyPiston->m_ID - C_MAX_TILES));

	for (int i = 0; i < C_MAX_TILES; i++)
	{
		if (Tile::tiles[i] && !Item::items[i])
			Item::items[i] = new TileItem(i - C_MAX_TILES);
	}
}

void Tile::teardownTiles()
{
	for (int i = 0; i < C_MAX_TILES; i++)
		delete tiles[i];
}

void Tile::updateShape(const LevelSource* a, const TilePos& pos)
{
}

void Tile::addLights(Level* p, const TilePos& pos)
{
}

float Tile::getBrightness(const LevelSource* pSrc, const TilePos& pos) const
{
	return pSrc->getBrightness(pos);
}

int Tile::getColor(const LevelSource*, const TilePos& pos, Facing::Name facing, int texture) const
{
	return 0xFFFFFF;
}

int Tile::getColor(int type, Facing::Name facing, int texture) const
{
	return 0xFFFFFF;
}

AABB* Tile::getAABB(const Level* pLevel, const TilePos& pos)
{
	Vec3 offset(pos);

	m_aabbReturned = AABB(offset + m_aabb.min, offset + m_aabb.max);
	return &m_aabbReturned;
}

AABB Tile::getTileAABB(const Level* pLevel, const TilePos& pos)
{
	Vec3 offset(pos);
	return AABB(offset + m_aabb.min, offset + m_aabb.max);
}

void Tile::addAABBs(const Level* pLevel, const TilePos& pos, const AABB* aabb, std::vector<AABB>& out)
{
	AABB* pTileAABB = getAABB(pLevel, pos);

	if (pTileAABB && pTileAABB->intersect(*aabb))
	{
		out.push_back(*pTileAABB);
	}
}

bool Tile::shouldRenderFace(const LevelSource* pSrc, const TilePos& pos, Facing::Name face)  const
{

	switch (face)
	{
	case Facing::NORTH:
		if (m_aabb.min.z > 0.0f) return true;
		break;
	case Facing::SOUTH:
		if (m_aabb.max.z < 1.0f) return true;
		break;
	case Facing::WEST:
		if (m_aabb.min.x > 0.0f) return true;
		break;
	case Facing::EAST:
		if (m_aabb.max.x < 1.0f) return true;
		break;
	case Facing::DOWN:
		if (m_aabb.min.y > 0.0f) return true;
		break;
	case Facing::UP:
		if (m_aabb.max.y < 1.0f) return true;
		break;
	}

	return !pSrc->isSolidTile(pos);
}

int Tile::getTexture(const LevelSource* pSrc, const TilePos& pos, Facing::Name face) const
{
	return getTexture(face, pSrc->getData(pos));
}

bool Tile::canSurvive(const Level* pLevel, const TilePos& pos) const
{
	return true;
}

// returns if we can place over the tile
bool Tile::mayPlace(const Level* pLevel, const TilePos& pos) const
{
	TileID tile = pLevel->getTile(pos);
	if (!tile)
		return true; // we can definitely place something over air
	
	return Tile::tiles[tile]->m_pMaterial->isReplaceable();
}

void Tile::tick(Level* pLevel, const TilePos& pos, Random* pRandom)
{

}

void Tile::animateTick(Level* pLevel, const TilePos& pos, Random* pRandom)
{

}

void Tile::destroy(Level* pLevel, const TilePos& pos, int data)
{

}

void Tile::neighborChanged(Level* pLevel, const TilePos& pos, TileID tile)
{

}

void Tile::onPlace(Level* pLevel, const TilePos& pos)
{
	if (hasTileEntity())
	{
		pLevel->setTileEntity(pos, newTileEntity());
	}

}

void Tile::onRemove(Level* pLevel, const TilePos& pos)
{
	if (hasTileEntity())
		pLevel->removeTileEntity(pos);
}

bool Tile::containsX(const Vec3& v)
{
	return v.y >= m_aabb.min.y
		&& v.y <= m_aabb.max.y
		&& v.z >= m_aabb.min.z
		&& v.z <= m_aabb.max.z;
}

bool Tile::containsY(const Vec3& v)
{
	return v.x >= m_aabb.min.x
		&& v.x <= m_aabb.max.x
		&& v.z >= m_aabb.min.z
		&& v.z <= m_aabb.max.z;
}

bool Tile::containsZ(const Vec3& v)
{
	return v.x >= m_aabb.min.x
		&& v.x <= m_aabb.max.x
		&& v.y >= m_aabb.min.y
		&& v.y <= m_aabb.max.y;
}

bool Tile::canStartDestroy(const Level*, const TilePos&, Facing::Name)
{
	return true;
}

HitResult Tile::clip(const Level* level, const TilePos& pos, Vec3 vec1, Vec3 vec2)
{
	updateShape(level, pos);

	Vec3 clipMinX, clipMinY, clipMinZ;
	Vec3 clipMaxX, clipMaxY, clipMaxZ;
	bool bClipMinX, bClipMinY, bClipMinZ;
	bool bClipMaxX, bClipMaxY, bClipMaxZ;

	vec1 -= Vec3(pos);
	vec2 -= Vec3(pos);

	bClipMinX = vec1.clipX(vec2, m_aabb.min.x, clipMinX) && containsX(clipMinX);
	bClipMaxX = vec1.clipX(vec2, m_aabb.max.x, clipMaxX) && containsX(clipMaxX);
	bClipMinY = vec1.clipY(vec2, m_aabb.min.y, clipMinY) && containsY(clipMinY);
	bClipMaxY = vec1.clipY(vec2, m_aabb.max.y, clipMaxY) && containsY(clipMaxY);
	bClipMinZ = vec1.clipZ(vec2, m_aabb.min.z, clipMinZ) && containsZ(clipMinZ);
	bClipMaxZ = vec1.clipZ(vec2, m_aabb.max.z, clipMaxZ) && containsZ(clipMaxZ);

	// the collided side of our AABB
	Facing::Name collType = Facing::DOWN;

	// the preferred vector for our collision
	Vec3* pVec = nullptr;
	if (bClipMinX)
		pVec = &clipMinX, collType = Facing::WEST;

	if (bClipMaxX)
	{
		if (!pVec || clipMaxX.distanceToSqr(vec1) < pVec->distanceToSqr(vec1))
			pVec = &clipMaxX, collType = Facing::EAST;
	}

	if (bClipMinY)
	{
		if (!pVec || clipMinY.distanceToSqr(vec1) < pVec->distanceToSqr(vec1))
			pVec = &clipMinY, collType = Facing::DOWN;
	}

	if (bClipMaxY)
	{
		if (!pVec || clipMaxY.distanceToSqr(vec1) < pVec->distanceToSqr(vec1))
			pVec = &clipMaxY, collType = Facing::UP;
	}

	if (bClipMinZ)
	{
		if (!pVec || clipMinZ.distanceToSqr(vec1) < pVec->distanceToSqr(vec1))
			pVec = &clipMinZ, collType = Facing::NORTH;
	}

	if (bClipMaxZ)
	{
		if (!pVec || clipMaxZ.distanceToSqr(vec1) < pVec->distanceToSqr(vec1))
			pVec = &clipMaxZ, collType = Facing::SOUTH;
	}

	if (!pVec)
	{
		// return a nothing burger
		return HitResult();
	}

	return HitResult(pos, collType, *pVec + pos);
}

int Tile::getSignal(const LevelSource* pLevel, const TilePos& pos) const
{
	return 0;
}

int Tile::getSignal(const LevelSource* pLevel, const TilePos& pos, Facing::Name face) const
{
	return 0;
}

int Tile::getDirectSignal(const Level* pLevel, const TilePos& pos, Facing::Name face) const
{
	return false;
}

void Tile::triggerEvent(Level* pLevel, const TilePos& pos, int a, int b)
{

}

void Tile::entityInside(Level* pLevel, const TilePos& pos, Entity* pEnt) const
{

}

void Tile::handleEntityInside(Level* pLevel, const TilePos& pos, const Entity* pEnt, Vec3& vec)
{

}

float Tile::getDestroyProgress(Player* player) const
{
	if (player->isCreative())
		return 1.0f;

	if (m_hardness < 0.0f)
		return 0.0f;

	if (!player->canDestroy(this))
		return 1.0f / m_hardness / 100.0f;

	return player->getDestroySpeed(this) / m_hardness / 30.0f;
}

void Tile::spawnResources(Level* pLevel, const TilePos& pos, int data)
{
	return spawnResources(pLevel, pos, data, 1.0f);
}

void Tile::spawnResources(Level* pLevel, const TilePos& pos, int data, float fChance)
{
	if (pLevel->m_bIsOnline)
		return;

	int count = getResourceCount(&pLevel->m_random);
	for (int i = 0; i < count; i++)
	{
		if (pLevel->m_random.nextFloat() > fChance)
			continue;

		int id = getResource(data, &pLevel->m_random);
		if (id <= 0)
			continue;
		spawnResources(pLevel, pos, std::make_shared<ItemInstance>(id, 1, getSpawnResourcesAuxValue(data)));
	}
}

void Tile::spawnResources(Level* pLevel, const TilePos& pos, const std::shared_ptr<ItemInstance>& item)
{
	Vec3 o;
	o.x = (pLevel->m_random.nextFloat() * 0.7f) + (1.0f - 0.7f) * 0.5f;
	o.y = (pLevel->m_random.nextFloat() * 0.7f) + (1.0f - 0.7f) * 0.5f;
	o.z = (pLevel->m_random.nextFloat() * 0.7f) + (1.0f - 0.7f) * 0.5f;

	auto pEntity = std::make_shared<ItemEntity>(pLevel, o + pos, item);
	pEntity->m_throwTime = 10;

	pLevel->addEntity(pEntity);
}

int Tile::spawnBurnResources(Level*, float, float, float)
{
	return 0;
}

float Tile::getExplosionResistance(Entity* entity) const
{
	return m_blastResistance / 5.0f;
}

void Tile::wasExploded(Level* pLevel, const TilePos& pos)
{

}

bool Tile::use(Level*, const TilePos& pos, Player*)
{
	return 0;
}

void Tile::stepOn(Level* pLevel, const TilePos& pos, Entity* entity)
{

}

void Tile::setPlacedOnFace(Level* pLevel, const TilePos& pos, Facing::Name face)
{

}

void Tile::setPlacedBy(Level* pLevel, const TilePos& pos, Mob* mob, Facing::Name face)
{

}

void Tile::prepareRender(Level* pLevel, const TilePos& pos)
{

}

void Tile::attack(Level* pLevel, const TilePos& pos, Player* player)
{

}

void Tile::playerDestroy(Level* level, Player* player, const TilePos& pos, int data)
{
	spawnResources(level, pos, data);
}

void Tile::playerWillDestroy(Player* player, const TilePos& pos, int data)
{

}

const Tile::SoundType
	Tile::SOUND_NORMAL("step.stone",  1.0f, 1.0f),
	Tile::SOUND_WOOD  ("step.wood",   1.0f, 1.0f),
	Tile::SOUND_GRAVEL("step.gravel", 1.0f, 1.0f),
	Tile::SOUND_GRASS ("step.grass",  0.5f, 1.0f),
	Tile::SOUND_STONE ("step.stone",  1.0f, 1.0f),
	Tile::SOUND_METAL ("step.stone",  1.0f, 1.5f),
	Tile::SOUND_GLASS ("step.stone", "random.glass", 1.0f, 1.0f),
	Tile::SOUND_CLOTH ("step.cloth",  1.0f, 1.0f),
	Tile::SOUND_SAND  ("step.sand", "step.gravel", 1.0f, 1.0f),
	Tile::SOUND_SILENT("",       1.0f, 1.0f);

Tile* Tile::sand;
Tile* Tile::sandStone;
Tile* Tile::cobblestone;
Tile* Tile::brick;
Tile* Tile::wood;
Tile* Tile::glass;
Tile* Tile::calmWater;
Tile* Tile::calmLava;
Tile* Tile::gravel;
Tile* Tile::stone;
Tile* Tile::bedrock;
Tile* Tile::dirt;
Tile* Tile::grass;
Tile* Tile::button;
Tile* Tile::ice;
Tile* Tile::snow;
Tile* Tile::clay;
Tile* Tile::cactus;
Tile* Tile::farmland;
Tile* Tile::stoneSlab;
Tile* Tile::stoneSlabHalf;
Tile* Tile::tallGrass;
Tile* Tile::deadBush;
Tile* Tile::cloth;
Tile* Tile::flower;
Tile* Tile::rose;
Tile* Tile::mushroom1;
Tile* Tile::mushroom2;
Tile* Tile::topSnow;
Tile* Tile::treeTrunk;
Tile* Tile::leaves;
Tile* Tile::diamondOre;
Tile* Tile::redstoneOre;
Tile* Tile::redstoneOreLit;
Tile* Tile::redstoneTorch;
Tile* Tile::redstoneTorchLit;
Tile* Tile::goldOre;
Tile* Tile::ironOre;
Tile* Tile::coalOre;
Tile* Tile::lapisOre;
Tile* Tile::reeds;
Tile* Tile::fence;
Tile* Tile::ladder;
Tile* Tile::rail;
Tile* Tile::poweredRail;
Tile* Tile::detectorRail;
Tile* Tile::lever;
Tile* Tile::stonePressurePlate;
Tile* Tile::woodPressurePlate;
Tile* Tile::obsidian;
Tile* Tile::tnt;
Tile* Tile::torch;
Tile* Tile::water;
Tile* Tile::lava;
Tile* Tile::pumpkin;
Tile* Tile::netherrack;
Tile* Tile::soulSand;
Tile* Tile::glowstone;
Tile* Tile::pumpkinLantern;
Tile* Tile::fire;
Tile* Tile::dispenser;
Tile* Tile::musicBlock;
RecordPlayerTile* Tile::recordPlayer;
Tile* Tile::mobSpawner;
Tile* Tile::chest;
Tile* Tile::redstoneDust;
Tile* Tile::goldBlock;
Tile* Tile::ironBlock;
Tile* Tile::diamondBlock;
Tile* Tile::stairsWood;
Tile* Tile::stairsStone;
Tile* Tile::sign;
Tile* Tile::wallSign;
Tile* Tile::doorWood;
Tile* Tile::doorIron;
Sapling* Tile::sapling;
Tile* Tile::sponge;
Tile* Tile::bed;
Tile* Tile::web;
Tile* Tile::lapisBlock;
Tile* Tile::bookshelf;
Tile* Tile::mossStone;
Tile* Tile::craftingTable;
Tile* Tile::crops;
Tile* Tile::furnace;
Tile* Tile::furnaceLit;
Tile* Tile::cake;
Tile* Tile::repeater;
Tile* Tile::repeaterLit;
Tile* Tile::trapDoor;
Tile* Tile::piston;
Tile* Tile::stickyPiston;
PistonHeadTile* Tile::pistonHead;
MovingPistonTile* Tile::movingPiston;
PortalTile* Tile::portal;