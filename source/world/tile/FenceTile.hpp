#pragma once

#include "Tile.hpp"

class FenceTile : public Tile
{
public:
	FenceTile(int id, int texture);
	AABB* getAABB(const Level* pLevel, const TilePos& pos) override;
	bool isSolidRender() const override;
	bool isCubeShaped() const override;
	int getRenderShape() const override;
};
