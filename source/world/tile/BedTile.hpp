#pragma once

#include "Tile.hpp"

class BedTile : public Tile
{
public:
	BedTile(int id, int texture);
	int getTexture(Facing::Name face, int data) const override;
	bool isSolidRender() const override;
	bool isCubeShaped() const override;
	int getRenderShape() const override;
	void updateShape(const LevelSource* level, const TilePos& pos) override;
	void neighborChanged(Level* level, const TilePos& pos, TileID tile) override;
	//int use(Level*, const TilePos& pos, Player*) override;
	static const constexpr int headBlockToFootBlockMap[4][2] = {{0, 1}, {-1, 0}, {0, -1}, {1, 0}};
	static const constexpr int field_22280_a[4] = {3, 4, 2, 5};
	static const constexpr int field_22279_b[4] = {2, 3, 0, 1};
	static const constexpr int bedDirection[4][6] = {{1, 0, 3, 2, 5, 4}, {1, 0, 5, 4, 2, 3}, {1, 0, 2, 3, 4, 5}, {1, 0, 4, 5, 3, 2}};
	static int getDirectionFromData(int meta) {
		return meta & 3;
	}

	static bool isBlockHeadOfBed(int meta) {
		return (meta & 8) != 0;
	}

	static bool isBedOccupied(int meta) {
		return (meta & 4) != 0;
	}
	static TilePos getNearestEmptyChunkTilePos(const Level* level, const TilePos& pos, int steps);
};
