/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ExternalFileLevelStorage.hpp"
#include "world/level/Level.hpp"
#include "GetTime.h"
#include <common/CompoundTag.hpp>
#include <common/ByteArrayTag.hpp>
#include <common/LongTag.hpp>
#include <common/IntTag.hpp>
#include <common/ByteTag.hpp>
#include <common/ListTag.hpp>

#ifndef DEMO

#define C_CHUNKS_TO_SAVE_PER_TICK (2)

ExternalFileLevelStorage::ExternalFileLevelStorage(const std::string& a, const std::string& path) :
	field_8(a),
	m_levelDirPath(path),
	m_timer(0)
{
	m_pLevel = nullptr;

	createFolderIfNotExists(m_levelDirPath.c_str());
	createFolderIfNotExists((m_levelDirPath+"/region").c_str());

	std::string datLevel  = m_levelDirPath + "/" + "level.dat";
	std::string datPlayer = m_levelDirPath + "/" + "player.dat";

	m_pLevelData = new LevelData;
	if (!readLevelData(datLevel, m_pLevelData))
	{
		delete m_pLevelData;
		m_pLevelData = nullptr;
		return;
	}

	readPlayerData(datPlayer, m_pLevelData);
}

ExternalFileLevelStorage::~ExternalFileLevelStorage()
{
	SAFE_DELETE(m_pLevelData);

	for (auto it = m_pRegionMap.begin(); it != m_pRegionMap.end(); ) {
		SAFE_DELETE(it->second);
		it = m_pRegionMap.erase(it);
	}
}

LevelData* ExternalFileLevelStorage::prepareLevel(Level* level)
{
	m_pLevel = level;
	return m_pLevelData;
}

ChunkStorage* ExternalFileLevelStorage::createChunkStorage(Dimension* pDim)
{
	return this;
}

void ExternalFileLevelStorage::saveLevelData(LevelData* levelData, std::vector<std::shared_ptr<Player>>& players)
{
	if (!players.empty()) players[0]->saveWithoutId(levelData->m_LocalPlayerData = std::make_shared<CompoundTag>());
	writeLevelData(m_levelDirPath + "/" + "level.dat", levelData);
	savePlayerData(levelData, players);

	SAFE_DELETE(m_pLevelData);

	m_pLevelData = new LevelData(*levelData);
}

void ExternalFileLevelStorage::savePlayerData(LevelData* levelData, std::vector<std::shared_ptr<Player>>& players)
{

}

void ExternalFileLevelStorage::closeAll()
{
}

void ExternalFileLevelStorage::tick()
{
	m_timer++;
	if (m_timer % 6000 != 0 || !m_pLevel)
		return;

	m_pLevel->saveUnsavedChunks(true);
}

void ExternalFileLevelStorage::flush()
{
}

RegionFile* ExternalFileLevelStorage::createOrGetRegion(const ChunkPos& pos)
{
	RegionFile* regionFile = m_pRegionMap[pos.regionKey()];
	if (!regionFile)
		m_pRegionMap[pos.regionKey()] = regionFile = new RegionFile(pos, m_levelDirPath+"/region");
	return regionFile;
}

LevelChunk* ExternalFileLevelStorage::load(Level* level, const ChunkPos& pos)
{
	RegionFile* regionFile = createOrGetRegion(pos);

	if (!regionFile->open()) {
		LOG_W("Not loading :(   (x: %d  z: %d)", pos.x, pos.z);
		return nullptr;
	}

	std::vector<uint8_t> decompressedData;
	if (!regionFile->readChunk(pos, decompressedData)) return nullptr;

	std::istringstream is(std::string(decompressedData.begin(), decompressedData.end()), std::ios::binary);
	auto rootTag = Tag::readNamed(is);
	auto levelTag = std::dynamic_pointer_cast<CompoundTag>(rootTag)->getOrMakeCompound("Level");

	if (!levelTag) {
		LOG_E("NBT: Missing 'Level' tag");
		return nullptr;
	}


	LevelChunk* chunk = new LevelChunk(level, new TileID[16 * 16 * 128], pos);
	chunk->deserialize(levelTag);
	chunk->recalcHeightmap();
	chunk->m_bUnsaved = false;
	chunk->m_bIsTerrainPopulated = true;
	chunk->field_237 = true;

	return chunk;
}

void ExternalFileLevelStorage::save(Level* level, LevelChunk* chunk)
{
	RegionFile* regionFile = createOrGetRegion(chunk->m_chunkPos);
	if (!regionFile->open()) {
		LOG_W("Not saving :(   (x: %d  z: %d)", chunk->m_chunkPos.x, chunk->m_chunkPos.z);
		return;
	}

	auto root = std::make_shared<CompoundTag>();
	root->put("Level", chunk->serialize());

	std::ostringstream oss(std::ios::binary);
	Tag::writeNamed(oss, "", root);

	std::string nbtData = oss.str();
	std::vector<uint8_t> nbtBytes(nbtData.begin(), nbtData.end());

	LOG_I("Saving chunk %d,%d, NBT size: %zu", chunk->m_chunkPos.x, chunk->m_chunkPos.z, nbtData.size());

	regionFile->writeChunk(chunk->m_chunkPos, nbtBytes);
	chunk->m_bUnsaved = false;
}

void ExternalFileLevelStorage::saveEntities(Level* level, LevelChunk* chunk)
{
	// no op
}

bool ExternalFileLevelStorage::readLevelData(const std::string& path, LevelData* pLevelData)
{
	FILE* pFile = std::fopen(path.c_str(), "rb");
	if (!pFile)
		return false;

	// Ler todo o conteúdo
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

	pLevelData->deserialize(std::dynamic_pointer_cast<CompoundTag>(tag)->getOrMakeCompound("Data"));

	fclose(pFile);

	return true;
}

bool ExternalFileLevelStorage::readPlayerData(const std::string& path, LevelData* pLevelData)
{
	return false;
}

bool ExternalFileLevelStorage::writeLevelData(const std::string& path, LevelData* pLevelData)
{
	FILE* pFile = fopen(path.c_str(), "wb");
	if (!pFile)
		return false;

	std::ostringstream oss(std::ios::binary);
	auto root = std::make_shared<CompoundTag>();
	root->put("Data", pLevelData->serialize());
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


#endif
