#pragma once

#include "Tile.hpp"

class SnowTile : public Tile
{
public:
	SnowTile(int id, int texture);

	void tick(Level*, const TilePos& pos, Random*) override;
	int getResource(int, Random*) const override;
	int getResourceCount(Random*) const override;
};