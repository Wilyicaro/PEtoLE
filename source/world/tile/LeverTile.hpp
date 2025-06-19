#pragma once

#include "Tile.hpp"

class LeverTile : public Tile
{
public:
	LeverTile(int id, int texture);
	bool mayPlace(const Level*, const TilePos& pos) const override;
	AABB* getAABB(const Level*, const TilePos& pos) override;
	void neighborChanged(Level* level, const TilePos& pos, TileID tile) override;
	bool use(Level*, const TilePos& pos, Player*) override;
	void attack(Level* level, const TilePos& pos, Player* player) override;
	bool isSolidRender() const override;
	bool isCubeShaped() const override;
	int getRenderShape() const override;
	int getDirectSignal(const Level*, const TilePos& pos, Facing::Name face) const override;
	int getSignal(const LevelSource*, const TilePos& pos, Facing::Name face) const override;
	bool isSignalSource() const override;
	void updateShape(const LevelSource* level, const TilePos& pos) override;
	void setPlacedBy(Level*, const TilePos& pos, Mob*, Facing::Name face) override;
	void onRemove(Level* level, const TilePos& pos) override;
	bool checkIfAttachedToBlock(Level* level, const TilePos& pos);
};

