/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ExternalFileLevelStorageSource.hpp"
#include "ExternalFileLevelStorage.hpp"
#include "common/Util.hpp"

#ifndef DEMO

ExternalFileLevelStorageSource::ExternalFileLevelStorageSource(const std::string& path)
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

std::string ExternalFileLevelStorageSource::getName()
{
	return "External File Level Storage";
}

LevelStorage* ExternalFileLevelStorageSource::selectLevel(const std::string& name, bool b)
{
	return new ExternalFileLevelStorage(name, m_worldsPath + "/" + name);
}

void ExternalFileLevelStorageSource::getLevelList(std::vector<LevelSummary>& vls)
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

void ExternalFileLevelStorageSource::clearAll()
{
}

int ExternalFileLevelStorageSource::getDataTagFor(const std::string& str)
{
	return 0;
}

bool ExternalFileLevelStorageSource::isNewLevelIdAcceptable(const std::string& levelID)
{
	return true;
}

static char g_EFLSSFilterArray[] = { '/','\n','\r','\x09','\0','\xC','`','?','*','\\','<','>','|','"',':' };

void ExternalFileLevelStorageSource::deleteLevel(const std::string& levelName)
{
	std::filesystem::remove_all(m_worldsPath + "/" + levelName);
}

void ExternalFileLevelStorageSource::renameLevel(const std::string& oldName, const std::string& newName)
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
	ExternalFileLevelStorage::readLevelData(m_worldsPath + "/" + levelUniqueName + "/" + "level.dat", &ld);
	ld.setLevelName(levelName);
	ExternalFileLevelStorage::writeLevelData(m_worldsPath + "/" + levelUniqueName + "/" + "level.dat", &ld);
}

bool ExternalFileLevelStorageSource::isConvertible(const std::string&)
{
	return false;
}

bool ExternalFileLevelStorageSource::requiresConversion(const std::string&)
{
	return false;
}

int ExternalFileLevelStorageSource::convertLevel(const std::string&, ProgressListener*)
{
	return 0;
}

void ExternalFileLevelStorageSource::addLevelSummaryIfExists(std::vector<LevelSummary>& vls, const char* name)
{
	std::string levelDat = m_worldsPath + "/" + name + "/" + "level.dat";
	
	LevelData ld;
	
	if (!ExternalFileLevelStorage::readLevelData(levelDat, &ld))
		return;

	vls.push_back(LevelSummary(name, ld.getLevelName(), ld.getLastPlayed(), ld.getSizeOnDisk()));
}

#endif
