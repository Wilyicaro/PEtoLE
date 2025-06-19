#pragma once

#include "Tile.hpp"

class RailTile : public Tile
{
public:
	RailTile(int id, int texture, bool isPowered);
	bool mayPlace(const Level*, const TilePos& pos) const override;
	void neighborChanged(Level* level, const TilePos& pos, TileID tile) override;
	bool isSolidRender() const override;
	bool isCubeShaped() const override;
	int getRenderShape() const override;
	int getTexture(Facing::Name face, int data) const override;
	void updateShape(const LevelSource* level, const TilePos& pos) override;
	void setPlacedBy(Level*, const TilePos& pos, Mob*, Facing::Name face) override;
	bool m_bIsPowered;
};

