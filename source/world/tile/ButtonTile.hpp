#pragma once

#include "Tile.hpp"

class ButtonTile : public Tile
{
public:
	ButtonTile(int id, int texture);
	virtual AABB* getAABB(const Level*, const TilePos& pos) override;
	bool mayPlace(const Level*, const TilePos& pos, Facing::Name face) const override;
	bool mayPlace(const Level*, const TilePos& pos) const override;
	void neighborChanged(Level* level, const TilePos& pos, TileID tile) override;
	void attack(Level*, const TilePos& pos, Player*) override;
	bool use(Level*, const TilePos& pos, Player*) override;
	int getTickDelay() const override;
	bool isSolidRender() const override;
	bool isCubeShaped() const override;
	void updateShape(const LevelSource* level, const TilePos& pos) override;
	void updateDefaultShape() override;
	void setPlacedOnFace(Level*, const TilePos& pos, Facing::Name face) override;
	bool checkCanSurvive(Level* level, const TilePos& pos);
	void tick(Level*, const TilePos& pos, Random*) override;
	void onPlace(Level* pLevel, const TilePos& pos) override;
	void onRemove(Level* pLevel, const TilePos& pos) override;
	int getSignal(const LevelSource*, const TilePos& pos, Facing::Name face) const override;
	int getDirectSignal(const Level*, const TilePos& pos, Facing::Name face) const override;
	bool isSignalSource() const override;

};

