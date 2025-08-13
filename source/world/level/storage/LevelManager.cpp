#include "LevelManager.hpp"
#include "world/level/Level.hpp"
#include "network/ServerSideNetworkHandler.hpp"

LevelManager::LevelManager(const std::string& name, const std::string& path) : m_name(name), m_path(path), m_pConnection(nullptr)
{
	createFolderIfNotExists(m_path.c_str());
	createFolderIfNotExists((m_path + "/region").c_str());
	m_bIsNew = !readLevelData(path + "/level.dat", m_levelData);
}

LevelManager::~LevelManager()
{
	//@Note: This need to be deleted before the levels
	delete m_pConnection;
	for (auto& it : m_levels)
		delete it.second;
}

Level* LevelManager::getLevel(int dim)
{
	auto it = m_levels.find(dim);
	if (it != m_levels.end()) return it->second;
	Level* level = new Level(this, Dimension::getNew(dim));
	m_pConnection->m_levelListeners.try_emplace(dim, m_pConnection, level);
	m_levels[dim] = level;
	return level;
}

void LevelManager::saveLevelData()
{
	m_pConnection->m_pMinecraft->m_pPlayer->saveWithoutId(m_levelData.m_LocalPlayerData = std::make_shared<CompoundTag>());
	writeLevelData(m_path + "/level.dat", m_levelData);
}

void LevelManager::savePlayerData()
{
}

void LevelManager::setConnection(ServerSideNetworkHandler* connection)
{
	m_pConnection = connection;
}

void LevelManager::tick()
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

bool LevelManager::readLevelData(const std::string& path, LevelData& pLevelData)
{
	FILE* pFile = std::fopen(path.c_str(), "rb");
	if (!pFile)
		return false;

	std::fseek(pFile, 0, SEEK_END);
	size_t size = std::ftell(pFile);
	std::fseek(pFile, 0, SEEK_SET);

	std::vector<uint8_t> compressedData(size);
	std::fread(compressedData.data(), 1, size, pFile);
	std::fclose(pFile);

	std::vector<uint8_t> decompressed = decompressZlibStream(compressedData.data(), compressedData.size(), true);

	std::istringstream iss(std::string(reinterpret_cast<const char*>(decompressed.data()), decompressed.size()));


	std::shared_ptr<Tag> tag = Tag::readNamed(iss);

	if (tag->getType() != TagType::TAG_Compound)
		throw std::runtime_error("invalid root tag in level.dat");

	pLevelData.deserialize(std::dynamic_pointer_cast<CompoundTag>(tag)->getOrMakeCompound("Data"));

	return true;
}

bool LevelManager::writeLevelData(const std::string& path, const LevelData& pLevelData)
{
	FILE* pFile = fopen(path.c_str(), "wb");
	if (!pFile)
		return false;

	std::ostringstream oss(std::ios::binary);
	auto root = std::make_shared<CompoundTag>();
	root->put("Data", pLevelData.serialize());
	Tag::writeNamed(oss, "", root);

	std::string nbtData = oss.str();

	std::vector<uint8_t> compressedData(nbtData.begin(), nbtData.end());
	try {
		compressedData = compressZlibStream(compressedData.data(), compressedData.size(), true);
	}
	catch (...) {
		return false;
	}

	fwrite(compressedData.data(), 1, compressedData.size(), pFile);
	fclose(pFile);

	return true;
}