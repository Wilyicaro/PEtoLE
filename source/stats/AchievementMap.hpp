#pragma once

#include <unordered_map>
#include <string>

class AchievementMap
{
public:
	AchievementMap();

	const std::string& get(int) const;

	void init();

	static AchievementMap* getInstance();

private:
	std::unordered_map<int, std::string> m_map;

	static AchievementMap* instance;

	static std::string NULL_UUID;
};