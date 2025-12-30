#pragma once

#include "world/level/Vec3i.hpp"
#include "world/level/levelgen/chunk/ChunkTilePos.hpp"

class DataLayer
{
public:
	DataLayer(uint8_t* data, int size = 0);
	DataLayer(int size = 16384);

	~DataLayer();

	static constexpr int MakeBlockDataIndex(const ChunkTilePos& pos)
	{
		return (pos.x << 11) | (pos.z << 7) | pos.y;
	}

	int operator[](const ChunkTilePos&) const;

	int get(const ChunkTilePos&) const;

	void set(const ChunkTilePos&, int data);

	bool isValid() const;

public:
	uint8_t* m_pData;
	int m_size;
};