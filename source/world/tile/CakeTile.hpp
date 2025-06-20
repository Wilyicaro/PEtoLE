#pragma once

#include "Tile.hpp"

class CakeTile : public Tile
{
public:
	CakeTile(int id, int texture);
	bool mayPlace(const Level*, const TilePos& pos) const override;
	void neighborChanged(Level* level, const TilePos& pos, TileID tile) override;
	void eat(Level*, const TilePos& pos, Player*);
	void attack(Level*, const TilePos& pos, Player*) override;
	bool use(Level*, const TilePos& pos, Player*) override;
	bool isSolidRender() const override;
	int getTexture(Facing::Name face, int data) const override;
	int getTexture(Facing::Name face) const override;
	bool isCubeShaped() const override;
	void updateShape(const LevelSource* level, const TilePos& pos) override;
	void updateDefaultShape() override;
	bool canSurvive(const Level* level, const TilePos& pos) const override;
	int getResource(int, Random*) const override;
	int getResourceCount(Random*) const override;
};

