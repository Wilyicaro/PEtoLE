#pragma once

#include "Bush.hpp"

class TallGrass : public Bush
{
public:
	TallGrass(int id, int texture);
	int getResource(int, Random*) const override;
	int getColor(const LevelSource*, const TilePos& pos, Facing::Name facing, int texture = -1) const override;
	int getColor(int data, Facing::Name facing, int texture = -1) const override;
	int getTexture(Facing::Name face, int data) const override;
	virtual int getRenderShape() const override;
};
