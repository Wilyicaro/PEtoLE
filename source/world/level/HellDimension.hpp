#pragma once

#include "Level.hpp"
#include "Dimension.hpp"

class HellDimension : public Dimension
{
public:
	Vec3f getFogColor(float, float) override;
	float getTimeOfDay(int64_t, float);
	void init() override;
	bool mayRespawn() override;
	bool isValidSpawn(const TilePos& pos) override;
	void init(Level* pLevel) override;
	void updateLightRamp() override;


	ChunkSource* createRandomLevelSource() override;
};