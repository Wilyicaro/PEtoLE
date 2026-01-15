#include "StatsSyncher.hpp"
#include "StatsCounter.hpp"
#include <fstream>
#include <cstdio>
#include <thread>
#include <chrono>
#include "common/Utils.hpp"

#include "client/player/input/User.hpp"

StatsSyncher::StatsSyncher(User* user, StatsCounter* statsCounter, const std::string& statsDir) 
    : statsCounter(statsCounter), user(user)
{
    std::string usernameLower = user->m_username;
    std::transform(usernameLower.begin(), usernameLower.end(), usernameLower.begin(), ::tolower);

    dataUnsentPath = statsDir + "/stats_" + usernameLower + "_unsent.dat";
    dataPath       = statsDir + "/stats_" + usernameLower + ".dat";
    oldUnsentPath  = statsDir + "/stats_" + usernameLower + "_unsent.old";
    oldPath        = statsDir + "/stats_" + usernameLower + ".old";
    tmpUnsentPath  = statsDir + "/stats_" + usernameLower + "_unsent.tmp";
    tmpPath        = statsDir + "/stats_" + usernameLower + ".tmp";

    if (fileExists(dataUnsentPath.c_str()))
    {
        auto stats = readStatsFromFile(dataUnsentPath, tmpUnsentPath, oldUnsentPath);
        if (!stats.empty())
        {
            statsCounter->syncStats(stats);
        }
    }

    std::thread([this]()
    {
        while (true)
        {
            if (!busy && pendingSend.empty())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    }).detach();
}

std::unordered_map<Stat*, int> StatsSyncher::readStatsFromFile(const std::string& path, const std::string& tmpPath, const std::string& backupPath)
{
    const std::string* fileToRead = &path;

    if (!fileExists(path.c_str()))
    {
        if (fileExists(backupPath.c_str())) fileToRead = &backupPath;
        else if (fileExists(tmpPath.c_str())) fileToRead = &tmpPath;
        else return {};
    }

    std::ifstream file(*fileToRead, std::ios::binary);
    if (!file.is_open()) return {};

    std::string content((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());

    return StatsCounter::parseStatsJson(content);
}

void StatsSyncher::writeStatsToFile(const std::unordered_map<Stat*, int>& statsMap, const std::string& path, const std::string& tmpPath, const std::string& backupPath)
{
    std::string content = StatsCounter::generateStatsJson(user->m_username, "local", statsMap);

    std::ofstream tmpFile(tmpPath);
    if (tmpFile.is_open())
    {
        tmpFile << content;
        tmpFile.close();

        if (fileExists(backupPath.c_str())) remove(backupPath.c_str());
        if (fileExists(path.c_str())) std::rename(path.c_str(), backupPath.c_str());
        std::rename(tmpPath.c_str(), path.c_str());
    }
}

void StatsSyncher::save(const std::unordered_map<Stat*, int>& statsMap)
{
    if (busy) throw std::runtime_error("Can't save stats while StatsSyncher is busy!");

    busy = true;
    syncDelay = 100;

    std::thread([this, statsMap]() {
        try {
            writeStatsToFile(statsMap, dataUnsentPath, tmpUnsentPath, oldUnsentPath);
        } catch (...) {
            // log error
        }
        busy = false;
    }).detach();
}

void StatsSyncher::destroy(const std::unordered_map<Stat*, int>& statsMap)
{
    int attempts = 30;
    while (busy && attempts-- > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    busy = true;
    try {
        writeStatsToFile(statsMap, dataUnsentPath, tmpUnsentPath, oldUnsentPath);
    } catch (...) {
        // log
    }
    busy = false;
}

bool StatsSyncher::canSync() const
{
    return syncDelay <= 0 && !busy && pendingReceive.empty();
}

void StatsSyncher::tick()
{
    if (syncDelay > 0) --syncDelay;
    if (sendDelay > 0) --sendDelay;


}