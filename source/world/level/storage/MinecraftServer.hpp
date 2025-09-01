#pragma once

#include "LevelData.hpp"
#include <unordered_map>
#include <vector>
#include "DimensionDataStorage.hpp"

class ServerSideNetworkHandler;
class Level;
class ProgressListener;

class MinecraftServer
{
public:
	MinecraftServer(const std::string& name, const std::string& path);
	virtual ~MinecraftServer();
	virtual Level* getLevel(int dim = 0);
	virtual void saveLevelData();
	virtual void savePlayerData();
	virtual void tick();
	virtual void manageLevels(ProgressListener&);
	virtual void setConnection(ServerSideNetworkHandler*);

	static bool readLevelData(const std::string& path, LevelData& pLevelData);
	static bool writeLevelData(const std::string& path, const LevelData& pLevelData);

public:
	std::string m_name;
	std::string m_path;
	bool m_bIsNew;
	LevelData m_levelData;
	ServerSideNetworkHandler* m_pConnection;
	DimensionDataStorage* m_pDataStorage;
	std::unordered_map<int, Level*> m_levels;
};

