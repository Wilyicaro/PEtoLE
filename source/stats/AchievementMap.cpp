#include "AchievementMap.hpp"
#include "client/app/AppPlatform.hpp"
#include <sstream>

AchievementMap::AchievementMap()
{
}

const std::string& AchievementMap::get(int id) const
{
	return m_map.at(id);
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

AchievementMap* AchievementMap::getInstance()
{
	return instance;
}
