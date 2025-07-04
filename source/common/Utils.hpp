/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <time.h>
#include <stdio.h> // have to include this to avoid it being included again later from being a problem
#include <stdint.h>
#include <stdarg.h>
#include <assert.h>
#include <limits>
#include <vector>
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <math.h>

#ifndef M_PI
#define M_PI (3.14159265358979)
#endif

#include <string>
#include <sstream>

#include "compat/LegacyCPPCompatibility.hpp"

#ifdef _MSC_VER
#pragma warning (disable : 4068)
#endif

#if defined(_WIN32)

// Do we even need all this WinSock stuff anymore?
#ifndef _XBOX // assume we're on a normal Windows device
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <direct.h>
#include <io.h>

#elif defined(_XBOX)

#include <xtl.h>
#include <winsockx.h>

#endif

// XPL means "Cross PLatform"
#define XPL_ACCESS _access
#define XPL_MKDIR(path, mode)  _mkdir(path)

// Bare bones non conforming implementation, but good enough
struct dirent
{
	int  d_type;
	char d_name[_MAX_PATH + 1];
};

struct DIR
{
	HANDLE current;
	WIN32_FIND_DATA findData;
	bool returnedFirstFileData;
};

#define DT_UNKNOWN (0)
#define DT_DIR     (4)
#define DT_REG     (8)

DIR* opendir(const char* name);
dirent* readdir(DIR* dir);
void closedir(DIR* dir);

#include <sys/stat.h>

#ifndef S_ISDIR
#define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)
#endif

// https://stackoverflow.com/questions/341817/is-there-a-replacement-for-unistd-h-for-windows-visual-c
/* Values for the second argument to access.
   These may be OR'd together.  */
#define R_OK    4       /* Test for read permission.  */
#define W_OK    2       /* Test for write permission.  */
   //#define   X_OK    1       /* execute permission - unsupported in windows*/
#define F_OK    0       /* Test for existence.  */

#else

#include <unistd.h>
#include <dirent.h>

 // XPL means "Cross PLatform"
#define XPL_ACCESS access
#define XPL_MKDIR(path, mode) mkdir(path, mode)

#endif

#include "../../compat/KeyCodes.hpp"
#include "Logger.hpp"

// options:
#include "../../GameMods.hpp"

// don't know where to declare these:

#ifndef MOD_USE_BIGGER_SCREEN_SIZE
#define C_DEFAULT_SCREEN_WIDTH  (854)
#define C_DEFAULT_SCREEN_HEIGHT (480)
#else
#define C_DEFAULT_SCREEN_WIDTH  (1280)
#define C_DEFAULT_SCREEN_HEIGHT (720)
#endif

#define C_MAX_TILES (256)

#define C_DEFAULT_PORT (19132)
#define C_MAX_CONNECTIONS (4) // pitiful

constexpr int C_MIN_X = -32000000, C_MAX_X = 32000000;
constexpr int C_MIN_Z = -32000000, C_MAX_Z = 32000000;
constexpr int C_MIN_Y = 0, C_MAX_Y = 128;

const char* GetTerrainName();
const char* GetItemsName();
const char* GetGUIBlocksName();

#ifdef ORIGINAL_CODE
#define C_TERRAIN_NAME "terrain.png"
#define C_ITEMS_NAME   "gui/items.png"
#define C_BLOCKS_NAME  "gui/gui_blocks.png"
#else
#define C_TERRAIN_NAME GetTerrainName()
#define C_TERRAIN_MIPMAP2_NAME   "terrain_MipmapLevel2.png"
#define C_TERRAIN_MIPMAP3_NAME   "terrain_MipmapLevel3.png"
#define C_ITEMS_NAME   "gui/items.png"
#define C_BLOCKS_NAME  "gui/gui_blocks.png"
#endif

#define C_MIN_CHUNKS_X (-27)
#define C_MIN_CHUNKS_Z (-27)

#define C_MAX_CHUNKS_X (27)
#define C_MAX_CHUNKS_Z (27)

#define C_MAX_CHUNKS (2916)

// 9 chunks around a player things will tick
#define C_TICK_DISTANCE_CHKS (9)

// note: bunch of these are unimplemented.
enum eTileID
{
	TILE_AIR,
	TILE_STONE,
	TILE_GRASS,
	TILE_DIRT,
	TILE_COBBLESTONE,
	TILE_WOOD,
	TILE_SAPLING,
	TILE_BEDROCK,
	TILE_WATER,
	TILE_WATER_CALM,
	TILE_LAVA,
	TILE_LAVA_CALM,
	TILE_SAND,
	TILE_GRAVEL,
	TILE_ORE_GOLD,
	TILE_ORE_IRON,
	TILE_ORE_COAL,
	TILE_TREE_TRUNK,
	TILE_LEAVES,
	TILE_SPONGE,
	TILE_GLASS,
	TILE_ORE_LAPIS,
	TILE_BLOCK_LAPIS,
	TILE_DISPENSER,
	TILE_SANDSTONE,
	TILE_NOTE_BLOCK,
	TILE_BED,
	TILE_RAIL_POWERED,
	TILE_RAIL_ACTIVATOR,
	TILE_PISTON_STICKY,
	TILE_COBWEB,
	TILE_TALL_GRASS,
	TILE_DEAD_BUSH,
	TILE_PISTON,
	TILE_PISTON_HEAD,
	TILE_CLOTH,
	TILE_PISTON_MOVING,
	TILE_FLOWER,
	TILE_ROSE,
	TILE_MUSHROOM_1,
	TILE_MUSHROOM_2,
	TILE_BLOCK_GOLD,
	TILE_BLOCK_IRON,
	TILE_STONESLAB_FULL,
	TILE_STONESLAB_HALF,
	TILE_BRICKS,
	TILE_TNT,
	TILE_BOOKSHELF,
	TILE_MOSS_STONE,
	TILE_OBSIDIAN,
	TILE_TORCH,
	TILE_FIRE,
	TILE_SPAWNER,
	TILE_STAIRS_WOOD,
	TILE_CHEST,
	TILE_WIRE,
	TILE_ORE_DIAMOND,
	TILE_BLOCK_DIAMOND,
	TILE_WORKBENCH,
	TILE_WHEAT,
	TILE_FARMLAND,
	TILE_FURNACE,
	TILE_FURNACE_LIT,
	TILE_SIGN,
	TILE_DOOR_WOOD,
	TILE_LADDER,
	TILE_RAIL,
	TILE_STAIRS_STONE,
	TILE_SIGN_WALL,
	TILE_LEVER,
	TILE_PLATE_STONE,
	TILE_DOOR_IRON,
	TILE_PLATE_WOOD,
	TILE_ORE_REDSTONE,
	TILE_ORE_REDSTONE_LIT,
	TILE_NOT_GATE_OFF,
	TILE_NOT_GATE_ON,
	TILE_BUTTON_STONE,
	TILE_TOPSNOW,
	TILE_ICE,
	TILE_SNOW,
	TILE_CACTUS,
	TILE_CLAY,
	TILE_REEDS,
	TILE_JUKEBOX,
	TILE_FENCE,
	TILE_PUMPKIN,
	TILE_NETHERRACK,
	TILE_SOUL_SAND,
	TILE_GLOWSTONE,
	TILE_PORTAL,
	TILE_PUMPKIN_LIT,
	TILE_CAKE,
	TILE_REPEATER_OFF,
	TILE_REPEATER_ON,
	TILE_INVISIBLE = 95, // in JE, this is a locked chest
	TILE_TRAPDOOR,
	TILE_STONE_MONSTER_EGG,
	TILE_STONE_BRICKS,
	TILE_MUSHROOM1_BLOCK,
	TILE_MUSHROOM2_BLOCK,

	TILE_INFO_UPDATEGAME1 = 248,
	TILE_INFO_UPDATEGAME2 = 249,
	TILE_LEAVES_CARRIED = 254,

	TILE_OBSIDIAN_CRYING = 200, // custom stuff - ID of 200
	TILE_ROCKET_LAUNCHER,

	ITEM_SHOVEL_IRON = 256,
	ITEM_PICKAXE_IRON,
	ITEM_HATCHET_IRON,
	ITEM_FLINT_AND_STEEL,
	ITEM_APPLE,
	ITEM_BOW,
	ITEM_ARROW,
	ITEM_COAL,
	ITEM_DIAMOND,
	ITEM_INGOT_IRON,
	ITEM_INGOT_GOLD,
	ITEM_SWORD_IRON,
	ITEM_SWORD_WOOD,
	ITEM_SHOVEL_WOOD,
	ITEM_PICKAXE_WOOD,
	ITEM_HATCHET_WOOD,
	ITEM_SWORD_STONE,
	ITEM_SHOVEL_STONE,
	ITEM_PICKAXE_STONE,
	ITEM_HATCHET_STONE,
	ITEM_SWORD_DIAMOND,
	ITEM_SHOVEL_DIAMOND,
	ITEM_PICKAXE_DIAMOND,
	ITEM_HATCHET_DIAMOND,
	ITEM_STICK,
	ITEM_BOWL,
	ITEM_STEW_MUSHROOM,
	ITEM_SWORD_GOLD,
	ITEM_SHOVEL_GOLD,
	ITEM_PICKAXE_GOLD,
	ITEM_HATCHET_GOLD,
	ITEM_STRING,
	ITEM_FEATHER,
	ITEM_SULPHUR,
	ITEM_HOE_WOOD,
	ITEM_HOE_STONE,
	ITEM_HOE_IRON,
	ITEM_HOE_DIAMOND,
	ITEM_HOE_GOLD,
	ITEM_SEEDS,
	ITEM_WHEAT,
	ITEM_BREAD,
	ITEM_HELMET_CLOTH,
	ITEM_CHESTPLATE_CLOTH,
	ITEM_LEGGINGS_CLOTH,
	ITEM_BOOTS_CLOTH,
	ITEM_HELMET_CHAIN,
	ITEM_CHESTPLATE_CHAIN,
	ITEM_LEGGINGS_CHAIN,
	ITEM_BOOTS_CHAIN,
	ITEM_HELMET_IRON,
	ITEM_CHESTPLATE_IRON,
	ITEM_LEGGINGS_IRON,
	ITEM_BOOTS_IRON,
	ITEM_HELMET_DIAMOND,
	ITEM_CHESTPLATE_DIAMOND,
	ITEM_LEGGINGS_DIAMOND,
	ITEM_BOOTS_DIAMOND,
	ITEM_HELMET_GOLD,
	ITEM_CHESTPLATE_GOLD,
	ITEM_LEGGINGS_GOLD,
	ITEM_BOOTS_GOLD,
	ITEM_FLINT,
	ITEM_PORKCHOP_RAW,
	ITEM_PORKCHOP_COOKED,
	ITEM_PAINTING,
	ITEM_APPLE_GOLD,
	ITEM_SIGN,
	ITEM_DOOR_WOOD,
	ITEM_BUCKET,
	ITEM_BUCKET_WATER,
	ITEM_BUCKET_LAVA,
	ITEM_MINECART,
	ITEM_SADDLE,
	ITEM_DOOR_IRON,
	ITEM_REDSTONE,
	ITEM_SNOWBALL,
	ITEM_BOAT,
	ITEM_LEATHER,
	ITEM_BUCKET_MILK,
	ITEM_BRICK,
	ITEM_CLAY,
	ITEM_REEDS,
	ITEM_PAPER,
	ITEM_BOOK,
	ITEM_SLIME_BALL,
	ITEM_MINECART_CHEST,
	ITEM_MINECART_FURNACE,
	ITEM_EGG,
	ITEM_COMPASS,
	ITEM_FISHING_ROD,
	ITEM_CLOCK,
	ITEM_YELLOW_DUST,
	ITEM_FISH_RAW,
	ITEM_FISH_COOKED,
	ITEM_DYE_POWDER,
	ITEM_BONE,
	ITEM_SUGAR,
	ITEM_CAKE,
	ITEM_BED,
	ITEM_DIODE,
	ITEM_COOKIE,
	ITEM_RECORD_01,
	ITEM_RECORD_02,
	ITEM_CAMERA = 456,

	// Custom items
	ITEM_ROCKET = 470,
};

enum // Textures
{
	TEXTURE_GRASS_TOP,
	TEXTURE_STONE,
	TEXTURE_DIRT,
	TEXTURE_GRASS_SIDE,
	TEXTURE_PLANKS,
	TEXTURE_STONE_SLAB_SIDE,
	TEXTURE_STONE_SLAB_TOP,
	TEXTURE_BRICKS,
	TEXTURE_TNT_SIDE,
	TEXTURE_TNT_TOP,
	TEXTURE_TNT_BOTTOM,
	TEXTURE_COBWEB,
	TEXTURE_ROSE,
	TEXTURE_FLOWER,
	TEXTURE_WATER_STATIC,
	TEXTURE_SAPLING,
	TEXTURE_COBBLESTONE,
	TEXTURE_BEDROCK,
	TEXTURE_SAND,
	TEXTURE_GRAVEL,
	TEXTURE_LOG_SIDE,
	TEXTURE_LOG_TOP,
	TEXTURE_IRON,
	TEXTURE_GOLD,
	TEXTURE_DIAMOND,
	TEXTURE_CHEST_ONE_TOP,
	TEXTURE_CHEST_ONE_SIDE,
	TEXTURE_CHEST_ONE_FRONT,
	TEXTURE_MUSHROOM_RED,
	TEXTURE_MUSHROOM_BROWN,
	TEXTURE_NONE53,
	TEXTURE_FIRE1,
	TEXTURE_ORE_GOLD,
	TEXTURE_ORE_IRON,
	TEXTURE_ORE_COAL,
	TEXTURE_BOOKSHELF,
	TEXTURE_MOSSY_STONE,
	TEXTURE_OBSIDIAN,
	TEXTURE_GRASS_SIDE_OVERLAY,
	TEXTURE_TALL_GRASS,
	TEXTURE_NONE40,
	TEXTURE_CHEST_TWO_FRONT_LEFT,
	TEXTURE_CHEST_TWO_FRONT_RIGHT,
	TEXTURE_WORKBENCH_TOP,
	TEXTURE_FURNACE_FRONT,
	TEXTURE_FURNACE_SIDE,
	TEXTURE_DISPENSER_SIDE,
	TEXTURE_FIRE2,
	TEXTURE_SPONGE,
	TEXTURE_GLASS,
	TEXTURE_ORE_DIAMOND,
	TEXTURE_ORE_RED_STONE,
	TEXTURE_LEAVES_TRANSPARENT,
	TEXTURE_LEAVES_OPAQUE,
	TEXTURE_NONE54,
	TEXTURE_DEAD_BUSH,
	TEXTURE_NONE56,
	TEXTURE_CHEST_TWO_BACK_LEFT,
	TEXTURE_CHEST_TWO_BACK_RIGHT,
	TEXTURE_WORKBENCH_SIDE_1,
	TEXTURE_WORKBENCH_SIDE_2,
	TEXTURE_FURNACE_LIT,
	TEXTURE_FURNACE_TOP,
	TEXTURE_NONE63,
	TEXTURE_CLOTH_64,
	TEXTURE_SPAWNER,
	TEXTURE_SNOW,
	TEXTURE_ICE,
	TEXTURE_GRASS_SIDE_SNOW,
	TEXTURE_CACTUS_TOP,
	TEXTURE_CACTUS_SIDE,
	TEXTURE_CACTUS_BOTTOM,
	TEXTURE_CLAY,
	TEXTURE_REEDS,
	TEXTURE_JUKEBOX_SIDE,
	TEXTURE_JUKEBOX_TOP,
	TEXTURE_NONE76,
	TEXTURE_NONE77,
	TEXTURE_NONE78,
	TEXTURE_NONE79,
	TEXTURE_TORCH_LIT,
	TEXTURE_DOOR_TOP,
	TEXTURE_DOOR_IRON_TOP,
	TEXTURE_LADDER,
	TEXTURE_NONE84,
	TEXTURE_NONE85,
	TEXTURE_FARMLAND,
	TEXTURE_FARMLAND_DRY,
	TEXTURE_WHEAT_0,
	TEXTURE_WHEAT_1,
	TEXTURE_WHEAT_2,
	TEXTURE_WHEAT_3,
	TEXTURE_WHEAT_4,
	TEXTURE_WHEAT_5,
	TEXTURE_WHEAT_6,
	TEXTURE_WHEAT_7,
	TEXTURE_LEVER,
	TEXTURE_DOOR_BOTTOM,
	TEXTURE_DOOR_IRON_BOTTOM,
	TEXTURE_TORCH_RED_STONE,
	TEXTURE_NONE100,
	TEXTURE_NONE101,
	TEXTURE_PUMPKIN_TOP,
	TEXTURE_HELLROCK,
	TEXTURE_SOULSAND,
	TEXTURE_GLOWSTONE,
	TEXTURE_NONE106,
	TEXTURE_NONE107,
	TEXTURE_NONE108,
	TEXTURE_NONE109,
	TEXTURE_NONE110,
	TEXTURE_NONE111,
	TEXTURE_RAIL_CURVED,
	TEXTURE_CLOTH_112,
	TEXTURE_CLOTH_113,
	TEXTURE_TORCH_RED_STONE_OFF,
	TEXTURE_LOG_SPRUCE,
	TEXTURE_LOG_BIRCH,
	TEXTURE_PUMPKIN_SIDE,
	TEXTURE_PUMPKIN_FACE,
	TEXTURE_PUMPKIN_FACE_LIT,
	TEXTURE_CAKE_TOP,
	TEXTURE_CAKE_SIDE,
	TEXTURE_CAKE_SIDE_BIT,
	TEXTURE_CAKE_BOTTOM,
	TEXTURE_NONE125,
	TEXTURE_NONE126,
	TEXTURE_NONE127,
	TEXTURE_RAIL,

	TEXTURE_LAPIS = 144,
	TEXTURE_ORE_LAPIS = 160,
	TEXTURE_POWERED_RAIL = 163,
	TEXTURE_REDSTONE_DUST,
	TEXTURE_DETECTOR_RAIL = 195,

	TEXTURE_SANDSTONE_TOP = 176,
	TEXTURE_SANDSTONE_SIDE = 192,
	TEXTURE_WATER = 205,
	TEXTURE_SANDSTONE_BOTTOM = 208,

	TEXTURE_LAVA = 237,

	TEXTURE_INFO_UPDATEGAME1 = 252,
	TEXTURE_INFO_UPDATEGAME2 = 253,

	TEXTURE_LAVA_PLACEHOLDER = 255,
};

enum eRenderShape
{
	SHAPE_NONE = -1,
	SHAPE_SOLID,
	SHAPE_CROSS,
	SHAPE_TORCH,
	SHAPE_FIRE,
	SHAPE_WATER,
	SHAPE_DUST,
	SHAPE_CROPS,
	SHAPE_DOOR,
	SHAPE_LADDER,
	SHAPE_RAIL,
	SHAPE_STAIRS,
	SHAPE_FENCE,
	SHAPE_LEVER,
	SHAPE_CACTUS,
	SHAPE_BED,
	SHAPE_RANDOM_CROSS
};

enum eRenderLayer
{
	LAYER_OPAQUE,
	LAYER_ALPHA
};

typedef uint8_t TileID;
// TODO: "FullTile" struct with TileID and auxvalue?

/*struct Pos
{
	int x, y, z;
	Pos()
	{
		x = 0;
		y = 0;
		z = 0;
	}
	Pos(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}
};*/

#define SAFE_DELETE(ptr) do { if (ptr) delete ptr; } while (0)
#define SAFE_DELETE_ARRAY(ptr) do { if (ptr) delete[] ptr; } while (0)

#define SSTR( x ) static_cast< const std::ostringstream & >( \
		( std::ostringstream() << std::dec << x ) ).str()

// functions from Mojang
int64_t getMillis();
time_t getEpochTimeS();
time_t getRawTimeS();
double getTimeS();
int getTimeMs();
int64_t getTimeNano();

std::string getFormattedDate(int64_t time);

void sleepMs(int ms);

bool createFolderIfNotExists(const char* pDir);
bool DeleteDirectory(const std::string& name, bool unused);

// compress and decompress stuff with zlib: ( you must SAFE_DELETE_ARRAY what it returns )
uint8_t* ZlibInflateToMemory(uint8_t* pInput, size_t compressedSize, size_t decompressedSize);
uint8_t* ZlibDeflateToMemory(uint8_t* pInput, size_t sizeBytes, size_t *compressedSizeOut);
uint8_t* ZlibDeflateToMemoryLvl(uint8_t* pInput, size_t sizeBytes, size_t* compressedSizeOut, int level);

std::vector<uint8_t> compressZlibStream(const uint8_t* inputData, size_t inputSize, bool useGzip = false, int compressionLevel = 9);

std::vector<uint8_t> decompressZlibStream(const uint8_t* inputData, size_t inputSize, bool useGzip = false);

void writeBE16(std::ostream& os, uint16_t val);

void writeIntBE(FILE* file, int value);

int readIntBE(FILE* file);

bool w_get(const std::string& url, std::vector<unsigned char>& outData);

bool http_get(const std::string& host, const std::string& path, std::vector<unsigned char>& outData);
bool https_get(const std::string& host, const std::string& path, std::vector<unsigned char>& outData);

// things that we added:

#ifdef _WIN32

HINSTANCE GetInstance();
HWND GetHWND();
void CenterWindow(HWND hWnd);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

void SetInstance(HINSTANCE hinst);
void SetHWND(HWND hwnd);

#endif
