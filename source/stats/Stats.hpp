#pragma once

#include <unordered_map>
#include <vector>
#include <string>

class Stat;

class Stats
{
public:
    static std::unordered_map<int, Stat*> statById;
    static std::vector<Stat*> stats;
    static std::vector<Stat*> customStats;
    static std::vector<Stat*> tileStats;
    static std::vector<Stat*> itemStats;
    static std::vector<Stat*> statItemUsed;
    static std::vector<Stat*> statItemBreak;
    static std::vector<Stat*> statMineBlock;
    static std::vector<Stat*> itemCrafted;

    static Stat* startGame;
    static Stat* createWorld;
    static Stat* loadWorld;
    static Stat* joinMultiplayer;
    static Stat* leaveGame;

    static Stat* playOneMinute;
    static Stat* walkOneCm;
    static Stat* swimOneCm;
    static Stat* fallOneCm;
    static Stat* climbOneCm;
    static Stat* flyOneCm;
    static Stat* diveOneCm;
    static Stat* minecartOneCm;
    static Stat* boatOneCm;
    static Stat* pigOneCm;
    static Stat* jump;
    static Stat* drop;
    static Stat* damageDealt;
    static Stat* damageTaken;
    static Stat* deaths;
    static Stat* mobKills;
    static Stat* playerKills;
    static Stat* fishCaught;

    static bool statsInitialized;
    static bool itemStatsInitialized;

public:
    static void initStats();

    static void initBlockStats();

    static void initItemStats();

    static void finalizeStats();

private:
    static std::vector<Stat*>& createMineBlockStats(std::vector<Stat*>& statsArray, const std::string& statName, int baseId);

    static std::vector<Stat*>& createItemStats(std::vector<Stat*>& statsArray, const std::string& statName, int baseId, int start, int end);

    static std::vector<Stat*> createBreakItemStats(std::vector<Stat*>& statsArray, const std::string& statName, int baseId, int start, int end);

    static void finalizeStatArray(std::vector<Stat*>& statsArray);

    static void mapDuplicateIds(std::vector<Stat*>& statsArray, int fromId, int toId);

public:
    static Stat* byId(int id)
    {
        auto it = statById.find(id);
        return (it != statById.end()) ? it->second : nullptr;
    }
};