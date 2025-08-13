#include "TileEntityType.hpp"
#include "FurnaceTileEntity.hpp"
#include "ChestTileEntity.hpp"
#include "MobSpawnerTileEntity.hpp"
#include "DispenserTileEntity.hpp"
#include "SignTileEntity.hpp"
#include "MusicTileEntity.hpp"
#include "RecordPlayerTileEntity.hpp"

std::unordered_map<std::string, const TileEntityType*> TileEntityType::entityTypeNameMap;
std::vector<const TileEntityType*> TileEntityType::entityTypes;

TileEntityType::TileEntityType(const std::string& name, const TileEntityFactory& factory) : m_name(name), m_factory(factory)
{
}

void TileEntityType::initTypes()
{
	furnace = registerType<FurnaceTileEntity>("Furnace");
	chest = registerType<ChestTileEntity>("Chest");
	trap = registerType<DispenserTileEntity>("Trap");
	sign = registerType<SignTileEntity>("Sign");
	mobSpawner = registerType<MobSpawnerTileEntity>("MobSpawner");
	music = registerType<MusicTileEntity>("Music");
	recordPlayer = registerType<RecordPlayerTileEntity>("RecordPlayer");
}

void TileEntityType::teardownTypes()
{
	entityTypeNameMap.clear();
	entityTypes.clear();
}

TileEntityType* TileEntityType::furnace;
TileEntityType* TileEntityType::chest;
TileEntityType* TileEntityType::trap;
TileEntityType* TileEntityType::sign;
TileEntityType* TileEntityType::mobSpawner;
TileEntityType* TileEntityType::music;
TileEntityType* TileEntityType::recordPlayer;
