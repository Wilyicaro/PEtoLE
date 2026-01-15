#include "StatsCounter.hpp"
#include "StatsSyncher.hpp"
#include "Achievement.hpp"
#include "common/Utils.hpp"
#include "common/md5.hpp"
#include "client/app/AppPlatform.hpp"

StatsCounter::StatsCounter(User* user, const std::string& gamePath)
{
    std::string statsPath = gamePath + "/stats";
    m_pSyncher = new StatsSyncher(user, this, statsPath);
    createFolderIfNotExists(statsPath.c_str());
}

StatsCounter::~StatsCounter()
{
    delete m_pSyncher;
}

void StatsCounter::addStat(Stat* stat, int amount)
{
    addToMap(m_statsMap, stat, amount);
    addToMap(m_achievementMap, stat, amount);
    dirty = true;
}

void StatsCounter::addToMap(std::unordered_map<Stat*, int>& map, Stat* stat, int amount)
{
    auto it = map.find(stat);
    int current = (it == map.end()) ? 0 : it->second;
    map[stat] = current + amount;
}

std::unordered_map<Stat*, int> StatsCounter::getAllStats() const
{
    return m_statsMap;
}

void StatsCounter::syncStats(const std::unordered_map<Stat*, int>& statsMap)
{
    if (!statsMap.empty())
    {
        dirty = true;
        for (const auto& pair : statsMap)
        {
            addToMap(this->m_statsMap, pair.first, pair.second);
            addToMap(this->m_achievementMap, pair.first, pair.second);
        }
    }
}

void StatsCounter::applyRemoteStats(const std::unordered_map<Stat*, int>& remoteMap)
{
    if (!remoteMap.empty())
    {
        for (const auto& pair : remoteMap)
        {
            int current = getStatValue(pair.first);
            m_achievementMap[pair.first] = pair.second + current;
        }
    }
}

void StatsCounter::applyAchievementStats(const std::unordered_map<Stat*, int>& achievementMap)
{
    if (!achievementMap.empty()) {
        dirty = true;
        for (const auto& pair : achievementMap) {
            addToMap(this->m_statsMap, pair.first, pair.second);
        }
    }
}

bool StatsCounter::hasAchievement(Achievement* achievement) const
{
    return m_achievementMap.find(achievement) != m_achievementMap.end();
}

bool StatsCounter::canUnlockAchievement(Achievement* achievement) const
{
    return !achievement->m_pParent || hasAchievement(achievement->m_pParent);
}


int StatsCounter::getStatValue(Stat* stat) const
{
    auto it = m_achievementMap.find(stat);
    return (it == m_achievementMap.end()) ? 0 : it->second;
}

void StatsCounter::clear()
{
}

void StatsCounter::destroy()
{
    m_pSyncher->destroy(getAllStats());
}

void StatsCounter::tick()
{
    if (dirty && m_pSyncher->canSync())
    {
        m_pSyncher->save(getAllStats());
        dirty = false;
    }
    m_pSyncher->tick();
}

std::unordered_map<Stat*, int> StatsCounter::parseStatsJson(const std::string& jsonString)
{
    std::unordered_map<Stat*, int> result;

    try {
        auto json = nlohmann::json::parse(jsonString);

        if (!json.contains("stats-change") || !json["stats-change"].is_array())
            return result;

        std::string checksumBuilder;

        for (const auto& change : json["stats-change"]) {
            if (!change.is_object() || change.size() != 1) continue;

            auto it = change.items().begin();
            std::string key = it.key();
            std::string valueStr = it.value().get<std::string>();

            int statId = std::stoi(key);
            int amount = std::stoi(valueStr);

            auto stat = Stats::byId(statId);
            if (!stat)
                continue;
           

            checksumBuilder += std::to_string(statId) + "," + std::to_string(amount) + ",";

            result[stat] = amount;
        }

        std::string computedChecksum = computeChecksum("local", checksumBuilder);
        std::string expectedChecksum = json.value("checksum", "");

        if (computedChecksum != expectedChecksum)
        {
            result.clear();
        }

    }
    catch (const nlohmann::json::exception& e)
    {
    }

    return result;
}

std::string StatsCounter::generateStatsJson(const std::string& username, const std::string& sessionId, const std::unordered_map<Stat*, int>& statsMap)
{
    nlohmann::json json;

    if (!username.empty() && !sessionId.empty())
    {
        json["user"]["name"] = username;
        json["user"]["sessionid"] = sessionId;
    }

    nlohmann::json statsChange = nlohmann::json::array();
    std::string checksumBuilder;

    for (const auto& pair : statsMap)
    {
        auto stat = pair.first;
        int value = pair.second;

        nlohmann::json entry;
        entry[std::to_string(stat->m_id)] = std::to_string(value);
        statsChange.push_back(entry);

        checksumBuilder += std::to_string(stat->m_id) + "," + std::to_string(value) + ",";
    }

    json["stats-change"] = statsChange;

    std::string checksum = computeChecksum(sessionId.empty() ? "local" : sessionId, checksumBuilder);
    json["checksum"] = checksum;

    return json.dump(2);
}

std::string StatsCounter::computeChecksum(const std::string& sessionId, const std::string& data)
{
	return md5(sessionId + data);
}
