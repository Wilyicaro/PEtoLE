#pragma once

#include "Tile.hpp"

class SignTile : public Tile
{
public:
	SignTile(int id, bool isWall);
	bool mayPlace(const Level*, const TilePos& pos) const override;
	void neighborChanged(Level* level, const TilePos& pos, TileID tile) override;
	bool isSolidRender() const override;
	bool isCubeShaped() const override;
	int getRenderShape() const override;
	void updateShape(const LevelSource* level, const TilePos& pos) override;
	void setPlacedBy(Level*, const TilePos& pos, Mob*, Facing::Name face) override;
	bool checkIfAttachedToBlock(Level* level, const TilePos& pos);
	bool m_bIsWall;
};

