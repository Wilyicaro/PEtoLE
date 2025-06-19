#pragma once

#include "LevelChunk.hpp"

//@OVERSIGHT: Why the hell is EmptyLevelChunk derived from the WHOLE of LevelChunk?!
class FakeLevelChunk : public LevelChunk
{
public:
	enum ContentType {
		NONE,
		OCEAN,
		DATA
	};
	FakeLevelChunk(Level*, TileID* pBlockData, const ChunkPos& pos, ContentType content = OCEAN);

	ContentType contentType;

	void addEntity(std::shared_ptr<Entity>) override;
	void removeEntity(std::shared_ptr<Entity>) override;
	void removeEntity(std::shared_ptr<Entity>, int vec) override;
	void lightLava() override;
	void skyBrightnessChanged() override;
	void load() override;
	void unload() override;
	void markUnsaved() override;
	TileID getTile(const ChunkTilePos& pos) override;
	bool setTile(const ChunkTilePos& pos, TileID tile) override;
	bool setTileAndData(const ChunkTilePos& pos, TileID tile, int data) override;
	int  getData(const ChunkTilePos& pos) override;
	void setData(const ChunkTilePos& pos, int data) override;
	void recalcHeight(const ChunkTilePos& pos) override;
};