#pragma once

#include "LevelData.hpp"
#include <unordered_map>
#include <vector>

class ServerSideNetworkHandler;
class Level;

class LevelManager
{
public:
	LevelManager(const std::string& name, const std::string& path);
	virtual ~LevelManager();
	virtual Level* getLevel(int dim = 0);
	virtual void saveLevelData();
	virtual void savePlayerData();
	virtual void tick();
	virtual void setConnection(ServerSideNetworkHandler*);

	static bool readLevelData(const std::string& path, LevelData& pLevelData);
	static bool writeLevelData(const std::string& path, const LevelData& pLevelData);

public:
	std::string m_name;
	std::string m_path;
	bool m_bIsNew;
	LevelData m_levelData;
	ServerSideNetworkHandler* m_pConnection;
	std::unordered_map<int, Level*> m_levels;
};

