#pragma once

#include <unordered_map>
#include <string>

class Stat;
class Achievement;
class User;
class StatsSyncher;

class StatsCounter
{
public:
    StatsCounter(User* user, const std::string& gamePath);
    ~StatsCounter();

    void addStat(Stat* stat, int amount = 1);

    std::unordered_map<Stat*, int> getAllStats() const;

    void syncStats(const std::unordered_map<Stat*, int>& statsMap);
    void applyRemoteStats(const std::unordered_map<Stat*, int>& remoteMap);
    void applyAchievementStats(const std::unordered_map<Stat*, int>& achievementMap);

    bool hasAchievement(Achievement* achievement) const;
    bool canUnlockAchievement(Achievement* achievement) const;

    int getStatValue(Stat* stat) const;

    void clear();
    void destroy();
    void tick();

    static std::unordered_map<Stat*, int> parseStatsJson(const std::string& jsonString);
    static std::string generateStatsJson(const std::string& username, const std::string& sessionId, const std::unordered_map<Stat*, int>& statsMap);
    static std::string computeChecksum(const std::string& sessionId, const std::string& data);

private:
    void addToMap(std::unordered_map<Stat*, int>& map, Stat* stat, int amount);

    std::unordered_map<Stat*, int> m_statsMap;
    std::unordered_map<Stat*, int> m_achievementMap;
    bool dirty = false;

    StatsSyncher* m_pSyncher;
};