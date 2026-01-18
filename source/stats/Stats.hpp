#pragma once

#include <unordered_map>
#include <vector>
#include <string>

class Stat;
class IdStat;

class Stats
{
public:
    static std::unordered_map<int, Stat*> statById;
    static std::vector<Stat*> stats;
    static std::vector<Stat*> customStats;
    static std::vector<IdStat*> tileStats;
    static std::vector<IdStat*> itemStats;
    static std::vector<IdStat*> statItemUsed;
    static std::vector<IdStat*> statItemBreak;
    static std::vector<IdStat*> statMineBlock;
    static std::vector<IdStat*> itemCrafted;

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
    static std::vector<IdStat*>& createMineBlockStats(std::vector<IdStat*>& statsArray, const std::string& statName, int baseId);

    static std::vector<IdStat*>& createItemStats(std::vector<IdStat*>& statsArray, const std::string& statName, int baseId, int start, int end);

    static std::vector<IdStat*> createBreakItemStats(std::vector<IdStat*>& statsArray, const std::string& statName, int baseId, int start, int end);

    static void finalizeStatArray(std::vector<IdStat*>& statsArray);

    static void mapDuplicateIds(std::vector<IdStat*>& statsArray, int fromId, int toId);

public:
    static Stat* byId(int id)
    {
        auto it = statById.find(id);
        return (it != statById.end()) ? it->second : nullptr;
    }
};