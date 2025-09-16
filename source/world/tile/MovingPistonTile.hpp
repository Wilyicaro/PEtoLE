#pragma once

#include "Tile.hpp"

class PistonMovingTileEntity;

class MovingPistonTile : public Tile
{
public:
	MovingPistonTile(int id);

	bool hasTileEntity() const override;
	void onPlace(Level*, const TilePos& pos) override;
	void onRemove(Level*, const TilePos& pos) override;
	bool mayPlace(const Level*, const TilePos& pos) const override;
	bool mayPlace(const Level*, const TilePos& pos, Facing::Name face) const override;
	int getRenderShape() const override;
	bool isSolidRender() const override;
	bool isCubeShaped() const override;
	bool use(Level*, const TilePos& pos, Player*) override;
	int getResource(int, Random*);
	void spawnResources(Level*, const TilePos& pos, int, float) override;
	AABB* getAABB(const Level*, const TilePos& pos) override;
	void updateShape(const LevelSource*, const TilePos&) override;

	AABB* moveByPositionAndProgress(const Level*, const TilePos& pos, TileID id, float progress, Facing::Name dir);
	static std::shared_ptr<PistonMovingTileEntity> newMovingTileEntity(TileID tile, int meta, Facing::Name direction, bool extending, bool isSourcePiston);
};

