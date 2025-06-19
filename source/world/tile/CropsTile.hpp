#pragma once

#include "Bush.hpp"

class CropsTile : public Bush
{
public:
	CropsTile(int id, int texture);
	bool isSolidRender() const override;
	bool isCubeShaped() const override;
	int getRenderShape() const override;
	void tick(Level*, const TilePos& pos, Random*) override;
	int getTexture(Facing::Name face, int data) const override;
	void updateShape(const LevelSource* level, const TilePos& pos) override;
	float getGrowthRate(Level* level, const TilePos& pos);
	void fertilize(Level* level, const TilePos& pos);
	int getResource(int x, Random* random) const override;
	void spawnResources(Level*, const TilePos& pos, int, float) override;
};
