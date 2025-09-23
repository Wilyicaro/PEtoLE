#include "MinecraftServer.hpp"
#include "world/level/Level.hpp"
#include "network/ServerSideNetworkHandler.hpp"
#include <common/NbtIo.hpp>

MinecraftServer::MinecraftServer(const std::string& name, const std::string& path) : m_name(name), m_path(path), m_pConnection(nullptr)
{
	m_pDataStorage = new DimensionDataStorage(this);
	createFolderIfNotExists(m_path.c_str());
	createFolderIfNotExists((m_path + "/region").c_str());
	createFolderIfNotExists((m_path + "/data").c_str());
	m_bIsNew = !readLevelData(path + "/level.dat", m_levelData);
}

MinecraftServer::~MinecraftServer()
{
	delete m_pDataStorage;
	//@Note: This need to be deleted before the levels
	delete m_pConnection;
	for (auto& it : m_levels)
		delete it.second;
}

Level* MinecraftServer::getLevel(int dim)
{
	auto it = m_levels.find(dim);
	if (it != m_levels.end()) return it->second;
	Level* level = new Level(this, Dimension::getNew(dim));
	m_pConnection->m_levelListeners.try_emplace(dim, m_pConnection, level);
	m_levels[dim] = level;
	return level;
}

void MinecraftServer::saveLevelData()
{
	m_pConnection->m_pMinecraft->m_pPlayer->saveWithoutId(m_levelData.m_LocalPlayerData = std::make_shared<CompoundTag>());
	LevelData oldData;
	if (readLevelData(m_path + "/level.dat", oldData))
		writeLevelData(m_path + "/level.dat_old", oldData);
	writeLevelData(m_path + "/level.dat", m_levelData);
}

void MinecraftServer::savePlayerData()
{
}

void MinecraftServer::setConnection(ServerSideNetworkHandler* connection)
{
	m_pConnection = connection;
}

void MinecraftServer::tick()
{
	for (auto it = m_levels.begin(); it != m_levels.end(); )
	{
		Level* level = it->second;

		if (level->m_players.empty())
		{
			it = m_levels.erase(it);
			level->save();
			m_pConnection->m_levelListeners.erase(level->m_pDimension->m_ID);
			delete level;
		}
		else
		{
			level->tickEntities();
			level->tick();
			++it;
		}
	}

	for (auto& it : m_pConnection->m_onlinePlayers)
	{
		if (it.second->m_pPlayer != m_pConnection->m_pMinecraft->m_pPlayer)
		{
			const int viewDistance = it.second->m_pPlayer->m_pLevel->viewDistance;
			ChunkPos pos;
			for (int dx = -viewDistance; dx <= viewDistance; ++dx)
			{
				for (int dz = -viewDistance; dz <= viewDistance; ++dz)
				{
					pos.x = it.second->m_pPlayer->m_chunkPos.x + dx;
					pos.z = it.second->m_pPlayer->m_chunkPos.z + dz;
					LevelChunk* chunk = it.second->m_pPlayer->m_pLevel->getChunk(pos);

					if (!chunk || chunk->m_bDontSave) continue;
					auto ch = it.second->m_sentChunks.find(pos);
					if (ch == it.second->m_sentChunks.end())
					{
						it.second->m_sentChunks.insert(pos);

						m_pConnection->m_pRakNetInstance->send(new ChunkDataPacket(chunk));
					}
				}
			}
		}
	}
}

void MinecraftServer::manageLevels(ProgressListener& listener)
{
	for (auto it = m_levels.begin(); it != m_levels.end(); )
	{
		Level* level = it->second;

		if (level->m_players.empty())
		{
			it = m_levels.erase(it);
			level->save(true, listener);
			m_pConnection->m_levelListeners.erase(level->m_pDimension->m_ID);
			delete level;
		}
		else ++it;
	}
}

bool MinecraftServer::readLevelData(const std::string& path, LevelData& pLevelData)
{
	std::shared_ptr<CompoundTag> tag = NbtIo::read(path);

	if (!tag) return false;

	if (tag->getType() != TagType::TAG_Compound)
		throw std::runtime_error("invalid root tag in level.dat");

	pLevelData.load(tag->getOrMakeCompound("Data"));

	return true;
}

bool MinecraftServer::writeLevelData(const std::string& path, const LevelData& pLevelData)
{
	FILE* pFile = fopen(path.c_str(), "wb");
	if (!pFile)
		return false;

	auto root = std::make_shared<CompoundTag>();
	auto tag = std::make_shared<CompoundTag>();
	pLevelData.save(tag);
	root->put("Data", tag);
	
	return NbtIo::write(root, pFile);
}