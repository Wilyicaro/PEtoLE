#pragma once

#include "Tile.hpp"

class SignTile : public Tile
{
public:
	SignTile(int id, bool isWall);
	void neighborChanged(Level* level, const TilePos& pos, TileID tile) override;
	bool isSolidRender() const override;
	bool isCubeShaped() const override;
	virtual AABB* getAABB(const Level*, const TilePos& pos) override;
	int getRenderShape() const override;
	void updateShape(const LevelSource* level, const TilePos& pos) override;
	int getResource(int, Random*) const override;
	bool hasTileEntity() const override;
	std::shared_ptr<TileEntity> newTileEntity() override;

public:
	bool m_bIsWall;
};

