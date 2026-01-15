#include "Stats.hpp"
#include "world/item/Item.hpp"
#include "world/item/crafting/Recipes.hpp"
#include "world/item/crafting/FurnaceRecipes.hpp"
#include "client/locale/Language.hpp"
#include "CustomStat.hpp"
#include <unordered_set>

std::unordered_map<int, Stat*> Stats::statById;
std::vector<Stat*> Stats::stats;
std::vector<Stat*> Stats::customStats;
std::vector<Stat*> Stats::tileStats;
std::vector<Stat*> Stats::itemStats;
std::vector<Stat*> Stats::statItemUsed;
std::vector<Stat*> Stats::statItemBreak;
std::vector<Stat*> Stats::statMineBlock;
std::vector<Stat*> Stats::itemCrafted;

Stat* Stats::startGame;
Stat* Stats::createWorld;
Stat* Stats::loadWorld;
Stat* Stats::joinMultiplayer;
Stat* Stats::leaveGame;
Stat* Stats::playOneMinute;
Stat* Stats::walkOneCm;
Stat* Stats::swimOneCm;
Stat* Stats::fallOneCm;
Stat* Stats::climbOneCm;
Stat* Stats::flyOneCm;
Stat* Stats::diveOneCm;
Stat* Stats::minecartOneCm;
Stat* Stats::boatOneCm;
Stat* Stats::pigOneCm;
Stat* Stats::jump;
Stat* Stats::drop;
Stat* Stats::damageDealt;
Stat* Stats::damageTaken;
Stat* Stats::deaths;
Stat* Stats::mobKills;
Stat* Stats::playerKills;
Stat* Stats::fishCaught;

bool Stats::statsInitialized = false;
bool Stats::itemStatsInitialized = false;

void Stats::initStats()
{
    startGame = (new CustomStat(1000, "stat.startGame"))->root()->registerStat();
    createWorld = (new CustomStat(1001, "stat.createWorld"))->root()->registerStat();
    loadWorld = (new CustomStat(1002, "stat.loadWorld"))->root()->registerStat();
    joinMultiplayer = (new CustomStat(1003, "stat.joinMultiplayer"))->root()->registerStat();
    leaveGame = (new CustomStat(1004, "stat.leaveGame"))->root()->registerStat();

    playOneMinute = (new CustomStat(1100, "stat.playOneMinute", DateFormatter::INSTANCE))->root()->registerStat();
    walkOneCm = (new CustomStat(2000, "stat.walkOneCm", DistanceFormatter::INSTANCE))->root()->registerStat();
    swimOneCm = (new CustomStat(2001, "stat.swimOneCm", DistanceFormatter::INSTANCE))->root()->registerStat();
    fallOneCm = (new CustomStat(2002, "stat.fallOneCm", DistanceFormatter::INSTANCE))->root()->registerStat();
    climbOneCm = (new CustomStat(2003, "stat.climbOneCm", DistanceFormatter::INSTANCE))->root()->registerStat();
    flyOneCm = (new CustomStat(2004, "stat.flyOneCm", DistanceFormatter::INSTANCE))->root()->registerStat();
    diveOneCm = (new CustomStat(2005, "stat.diveOneCm", DistanceFormatter::INSTANCE))->root()->registerStat();
    minecartOneCm = (new CustomStat(2006, "stat.minecartOneCm", DistanceFormatter::INSTANCE))->root()->registerStat();
    boatOneCm = (new CustomStat(2007, "stat.boatOneCm", DistanceFormatter::INSTANCE))->root()->registerStat();
    pigOneCm = (new CustomStat(2008, "stat.pigOneCm", DistanceFormatter::INSTANCE))->root()->registerStat();
    jump = (new CustomStat(2010, "stat.jump"))->root()->registerStat();
    drop = (new CustomStat(2011, "stat.drop"))->root()->registerStat();
    damageDealt = (new CustomStat(2020, "stat.damageDealt"))->registerStat();
    damageTaken = (new CustomStat(2021, "stat.damageTaken"))->registerStat();
    deaths = (new CustomStat(2022, "stat.deaths"))->registerStat();
    mobKills = (new CustomStat(2023, "stat.mobKills"))->registerStat();
    playerKills = (new CustomStat(2024, "stat.playerKills"))->registerStat();
    fishCaught = (new CustomStat(2025, "stat.fishCaught"))->registerStat();

    statsInitialized = false;
    itemStatsInitialized = false;

    createMineBlockStats(statMineBlock, "stat.mineBlock", 16777216);

    initBlockStats();
    initItemStats();
}

void Stats::initBlockStats()
{
    createItemStats(statItemUsed, "stat.useItem", 16908288, 0, C_MAX_TILES);
    createBreakItemStats(statItemBreak, "stat.breakItem", 16973824, 0, C_MAX_TILES);
    statsInitialized = true;
    finalizeStats();
}

void Stats::initItemStats()
{
    createItemStats(statItemUsed, "stat.useItem", 16908288, C_MAX_TILES, C_MAX_ITEMS);
    createBreakItemStats(statItemBreak, "stat.breakItem", 16973824, C_MAX_TILES, C_MAX_ITEMS);
    itemStatsInitialized = true;
    finalizeStats();
}

void Stats::finalizeStats()
{
    if (!statsInitialized || !itemStatsInitialized) return;

    std::unordered_set<int> craftableIds;

    for (const auto& recipe : Recipes::getInstance().m_recipes)
        craftableIds.insert(recipe->assemble(nullptr)->m_itemID);

    for (const auto& result : FurnaceRecipes::getInstance().m_furnaceRecipes)
        craftableIds.insert(result.second.get()->assemble(nullptr)->m_itemID);

    itemCrafted.resize(C_MAX_ITEMS);

    for (int id : craftableIds)
    {
        if (Item::items[id])
        {
            itemCrafted[id] = (new IdStat(16842752 + id, "stat.craftItem", id))
                ->setNameFormatter([id](const std::string& str) {return Language::getInstance()->get(str, Item::items[id]->getName()); })
                ->registerStat();
        }
    }

    finalizeStatArray(itemCrafted);
}

std::vector<Stat*>& Stats::createMineBlockStats(std::vector<Stat*>& statsArray, const std::string& statName, int baseId)
{
    statsArray.resize(C_MAX_TILES);
    for (int i = 0; i < C_MAX_TILES; ++i) {
        if (Tile::tiles[i] && Tile::tiles[i]->shouldTrack())
        {
            statsArray[i] = (new IdStat(baseId + i, statName, i))
                ->setNameFormatter([i](const std::string& str) {return Language::getInstance()->get(str, Tile::tiles[i]->getName()); })
                ->registerStat();
            tileStats.push_back(statsArray[i]);
        }
    }

    finalizeStatArray(statsArray);
    return statsArray;
}

std::vector<Stat*>& Stats::createItemStats(std::vector<Stat*>& statsArray, const std::string& statName, int baseId, int start, int end)
{
    if (statsArray.empty()) statsArray.resize(C_MAX_ITEMS);

    for (int i = start; i < end; ++i)
    {
        if (Item::items[i])
        {
            statsArray[i] = (new IdStat(baseId + i, statName, i))
                ->setNameFormatter([i](const std::string& str) {return Language::getInstance()->get(str, Item::items[i]->getName()); })
                ->registerStat();

            if (i >= C_MAX_ITEMS)
                itemStats.push_back(statsArray[i]);
        }
    }

    finalizeStatArray(statsArray);
    return statsArray;
}

std::vector<Stat*> Stats::createBreakItemStats(std::vector<Stat*>& statsArray, const std::string& statName, int baseId, int start, int end)
{
    if (statsArray.empty()) statsArray.resize(C_MAX_ITEMS);

    for (int i = start; i < end; ++i)
    {
        if (Item::items[i] && Item::items[i]->isDamageable())
        {
            statsArray[i] = (new IdStat(baseId + i, statName, i))
                ->setNameFormatter([i](const std::string& str) {return Language::getInstance()->get(str, Item::items[i]->getName()); })
                ->registerStat();
        }
    }

    finalizeStatArray(statsArray);
    return statsArray;
}

void Stats::finalizeStatArray(std::vector<Stat*>& statsArray)
{
    mapDuplicateIds(statsArray, Tile::calmWater->m_ID, Tile::water->m_ID);
    mapDuplicateIds(statsArray, Tile::calmLava->m_ID, Tile::lava->m_ID);
    mapDuplicateIds(statsArray, Tile::pumpkinLantern->m_ID, Tile::pumpkin->m_ID);
    mapDuplicateIds(statsArray, Tile::furnaceLit->m_ID, Tile::furnace->m_ID);
    mapDuplicateIds(statsArray, Tile::redstoneOreLit->m_ID, Tile::redstoneOre->m_ID);
    mapDuplicateIds(statsArray, Tile::repeaterLit->m_ID, Tile::repeater->m_ID);
    mapDuplicateIds(statsArray, Tile::redstoneTorchLit->m_ID, Tile::redstoneTorch->m_ID);
    mapDuplicateIds(statsArray, Tile::mushroom1->m_ID, Tile::mushroom2->m_ID);
    mapDuplicateIds(statsArray, Tile::stoneSlab->m_ID, Tile::stoneSlabHalf->m_ID);
    mapDuplicateIds(statsArray, Tile::grass->m_ID, Tile::dirt->m_ID);
    mapDuplicateIds(statsArray, Tile::farmland->m_ID, Tile::dirt->m_ID);
}

void Stats::mapDuplicateIds(std::vector<Stat*>& statsArray, int fromId, int toId)
{
    if (statsArray[fromId] && !statsArray[toId])
        statsArray[toId] = statsArray[fromId];
    else if (statsArray[fromId])
    {
        stats.erase(std::remove(stats.begin(), stats.end(), statsArray[fromId]), stats.end());
        customStats.erase(std::remove(customStats.begin(), customStats.end(), statsArray[fromId]), customStats.end());
        itemStats.erase(std::remove(itemStats.begin(), itemStats.end(), statsArray[fromId]), itemStats.end());
        statsArray[fromId] = statsArray[toId];
    }
}
