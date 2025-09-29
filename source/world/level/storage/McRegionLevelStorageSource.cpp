/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "McRegionLevelStorageSource.hpp"
#include "MinecraftServer.hpp"
#include "common/Util.hpp"

#ifndef DEMO

McRegionLevelStorageSource::McRegionLevelStorageSource(const std::string& path)
{
	m_worldsPath = path;

	m_worldsPath += "/games";
	if (createFolderIfNotExists(m_worldsPath.c_str()))
	{
		m_worldsPath += "/com.mojang";
		if (createFolderIfNotExists(m_worldsPath.c_str()))
		{
			m_worldsPath += "/saves";
			if (createFolderIfNotExists(m_worldsPath.c_str()))
			{
				std::vector<LevelSummary> vls;
				getLevelList(vls);
			}
		}
	}

	m_worldsPath = path + "/games" + "/com.mojang" + "/saves";
}

std::string McRegionLevelStorageSource::getName()
{
	return "Scaevolus\' McRegion";
}

MinecraftServer* McRegionLevelStorageSource::selectLevel(const std::string& name, bool b)
{
	return new MinecraftServer(name, m_worldsPath + "/" + name);
}

void McRegionLevelStorageSource::getLevelList(std::vector<LevelSummary>& vls)
{
	DIR* dir = opendir(m_worldsPath.c_str());
	if (!dir)
		return;

	while (true)
	{
		dirent* de = readdir(dir);
		if (!de)
			break;

		LOG_I("Entry: %s", de->d_name);

#if defined( __HAIKU__ )
		std::string temp = m_worldsPath + '/' + de->d_name;

		struct stat buf;
		if ( ( lstat( temp.c_str(), &buf ) == 0 ) && S_ISDIR( buf.st_mode ) )
#else
		if (de->d_type == DT_DIR)
#endif
		{
			addLevelSummaryIfExists(vls, de->d_name);
		}
	}

	closedir(dir);
}

void McRegionLevelStorageSource::clearAll()
{
}

int McRegionLevelStorageSource::getDataTagFor(const std::string& str)
{
	return 0;
}

bool McRegionLevelStorageSource::isNewLevelIdAcceptable(const std::string& levelID)
{
	return true;
}

static char g_EFLSSFilterArray[] = { '/','\n','\r','\x09','\0','\xC','`','?','*','\\','<','>','|','"',':' };

void McRegionLevelStorageSource::deleteLevel(const std::string& levelName)
{
	std::error_code _Ec;
	std::filesystem::remove_all(m_worldsPath + "/" + levelName, _Ec);
	if (_Ec)
		LOG_I("Failed to delete level %s: %s", levelName.c_str(), _Ec.message().c_str());
}

void McRegionLevelStorageSource::renameLevel(const std::string& oldName, const std::string& newName)
{
	int accessResult = XPL_ACCESS((m_worldsPath + "/" + oldName).c_str(), 0);
	if (accessResult)
		return;

	std::string levelName = Util::stringTrim(newName);
	for (int i = 0; i < sizeof(g_EFLSSFilterArray); i++)
	{
		std::string str;
		str.push_back(g_EFLSSFilterArray[i]);
		Util::stringReplace(levelName, str, "");
	}

	std::vector<LevelSummary> vls;
	getLevelList(vls);

	std::set<std::string> maps;

	const size_t size = vls.size();
	for (int i = 0; i < size; i++)
		maps.insert(vls.at(i).m_fileName);

	std::string levelUniqueName = levelName;
	while (maps.find(levelUniqueName) != maps.end())
		levelUniqueName += "-";

	int renameResult = rename((m_worldsPath + "/" + oldName).c_str(), (m_worldsPath + "/" + levelUniqueName).c_str());
	if (renameResult != 0)
		levelUniqueName = oldName;

	LevelData ld;
	MinecraftServer::readLevelData(m_worldsPath + "/" + levelUniqueName + "/" + "level.dat", ld);
	ld.setLevelName(levelName);
	MinecraftServer::writeLevelData(m_worldsPath + "/" + levelUniqueName + "/" + "level.dat", ld);
}

bool McRegionLevelStorageSource::isConvertible(const std::string&)
{
	return false;
}

bool McRegionLevelStorageSource::requiresConversion(const std::string&)
{
	return false;
}

int McRegionLevelStorageSource::convertLevel(const std::string&, ProgressListener*)
{
	return 0;
}

void McRegionLevelStorageSource::addLevelSummaryIfExists(std::vector<LevelSummary>& vls, const char* name)
{
	std::string levelDat = m_worldsPath + "/" + name + "/" + "level.dat";
	
	LevelData ld;
	
	if (!MinecraftServer::readLevelData(levelDat, ld))
		return;

	vls.push_back(LevelSummary(name, ld.getLevelName(), ld.getLastPlayed(), ld.getSizeOnDisk()));
}

#endif
