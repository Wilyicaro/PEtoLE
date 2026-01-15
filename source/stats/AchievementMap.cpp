#include "AchievementMap.hpp"
#include "client/app/AppPlatform.hpp"
#include <sstream>

AchievementMap::AchievementMap()
{
}

//@NOTE: This will make sure that stats/achievements without a mapping won't crash the game and will have the same behavior as the original, which used StringBuilder.append and allowed null String values
//Certainly every stat should have a mapping in the original (tall grass mine block stat doesn't have one tho), so that isn't really an intentional behavior
const std::string& AchievementMap::get(int id) const
{
    auto it = m_map.find(id);
    return it == m_map.end() ? NULL_UUID : it->second;
}

void AchievementMap::init()
{
	std::istringstream stream = std::istringstream(AppPlatform::singleton()->readFile("achievement/map.txt"));
    std::string line;

    while (std::getline(stream, line))
    {
        if (line.empty())
            continue;

        std::istringstream lineStream = std::istringstream(line);
        std::vector<std::string> entry;
        std::string part;
        while (std::getline(lineStream, part, ','))
        {
            entry.push_back(part);
            if (entry.size() >= 2) break;
        }

        if (entry.size() < 2)
            continue;

        std::istringstream keyStr(entry[0]);
        int key;
        keyStr >> key;
        if (keyStr.fail())
            continue;

        m_map[key] = entry[1];
    }

    //LOG_I("Achievement Map started with %d entries!", m_map.size());
}

AchievementMap* AchievementMap::instance = new AchievementMap();

std::string AchievementMap::NULL_UUID = "null";

AchievementMap* AchievementMap::getInstance()
{
	return instance;
}
