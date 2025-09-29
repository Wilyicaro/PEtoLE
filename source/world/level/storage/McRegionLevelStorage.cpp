/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "McRegionLevelStorage.hpp"
#include "world/level/Level.hpp"
#include "GetTime.h"
#include <common/CompoundTag.hpp>

#ifndef DEMO

McRegionLevelStorage::McRegionLevelStorage(int i, const std::string& path) : m_path(path + "/DIM" + std::to_string(i))
{
	createFolderIfNotExists(m_path.c_str());
	createFolderIfNotExists((m_path + "/region").c_str());
}

McRegionLevelStorage::McRegionLevelStorage(const std::string& path) : m_path(path)
{
}

McRegionLevelStorage::~McRegionLevelStorage()
{
	for (auto it = m_pRegionMap.begin(); it != m_pRegionMap.end(); ) {
		it->second->close();
		SAFE_DELETE(it->second);
		it = m_pRegionMap.erase(it);
	}
}


RegionFile* McRegionLevelStorage::createOrGetRegion(const ChunkPos& pos)
{
	RegionFile* regionFile = m_pRegionMap[pos.regionKey()];
	if (!regionFile)
		m_pRegionMap[pos.regionKey()] = regionFile = new RegionFile(pos, m_path + "/region");
	return regionFile;
}

LevelChunk* McRegionLevelStorage::load(Level* level, const ChunkPos& pos)
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
	chunk->load(levelTag);
	chunk->recalcHeightmap();
	chunk->m_bUnsaved = false;
	chunk->field_237 = true;

	return chunk;
}

void McRegionLevelStorage::save(Level* level, LevelChunk* chunk)
{
	RegionFile* regionFile = createOrGetRegion(chunk->m_chunkPos);
	if (!regionFile->open())
	{
		LOG_W("Not saving :(   (x: %d  z: %d)", chunk->m_chunkPos.x, chunk->m_chunkPos.z);
		return;
	}

	auto root = std::make_shared<CompoundTag>();
	auto tag = std::make_shared<CompoundTag>();
	chunk->save(tag);
	root->put("Level", tag);

	std::ostringstream oss(std::ios::binary);
	Tag::writeNamed(oss, "", root);

	std::string nbtData = oss.str();
	std::vector<uint8_t> nbtBytes(nbtData.begin(), nbtData.end());

	//LOG_I("Saving chunk %d,%d, NBT size: %zu", chunk->m_chunkPos.x, chunk->m_chunkPos.z, nbtData.size());

	regionFile->writeChunk(chunk->m_chunkPos, nbtBytes);
	chunk->m_bUnsaved = false;
}

void McRegionLevelStorage::saveEntities(Level* level, LevelChunk* chunk)
{
	// no op
}
#endif
