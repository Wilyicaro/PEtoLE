#pragma once

#include <unordered_map>
#include <string>
#include <memory>

class User;
class StatsCounter;
class Stat;

class StatsSyncher
{
public:
    StatsSyncher(User* user, StatsCounter* statsCounter, const std::string& statsDir);

    void save(const std::unordered_map<Stat*, int>& statsMap);
    void destroy(const std::unordered_map<Stat*, int>& statsMap);
    bool canSync() const;
    void tick();

private:
    void writeStatsToFile(const std::unordered_map<Stat*, int>& statsMap,
                          const std::string& path,
                          const std::string& tmpPath,
                          const std::string& backupPath);

    std::unordered_map<Stat*, int> readStatsFromFile(const std::string& path,
        const std::string& tmpPath,
        const std::string& backupPath);

    bool busy = false;
    int syncDelay = 0;
    int sendDelay = 0;

    std::unordered_map<Stat*, int> pendingSend;
    std::unordered_map<Stat*, int> pendingReceive;

    StatsCounter* statsCounter;
    User* user;

    std::string dataUnsentPath;
    std::string dataPath;
    std::string oldUnsentPath;
    std::string oldPath;
    std::string tmpUnsentPath;
    std::string tmpPath;
};