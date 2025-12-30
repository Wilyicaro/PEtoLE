/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "common/Random.hpp"
#include "world/level/Vec3i.hpp"
#include <array>
#include <vector>
#include <memory>

class Level;

class ItemInstance;

class Feature
{
public:
	virtual ~Feature();
	virtual bool place(Level*, Random*, const TilePos& pos) = 0;
	virtual void init(real a, real b, real c);
};

class TreeFeature : public Feature
{
public:
	bool place(Level*, Random*, const TilePos& pos) override;
};

class BirchFeature : public Feature
{
public:
	bool place(Level*, Random*, const TilePos& pos) override;
};

class SpruceFeature : public Feature
{
public:
	bool place(Level*, Random*, const TilePos& pos) override;
};

class PineFeature : public Feature
{
public:
	bool place(Level*, Random*, const TilePos& pos) override;
};

class MonsterRoomFeature : public Feature
{
public:
	bool place(Level*, Random*, const TilePos& pos) override;

	std::shared_ptr<ItemInstance> randomItem(Random*);
	std::string randomEntityId(Random* random);
};

class FancyTreeFeature : public Feature
{
public:
	~FancyTreeFeature() = default;
	void generateBranchesAndTrunk();
	void crossection(int var1, int var2, int var3, float var4, uint8_t var5, int var6);
	float treeShape(int var1);
	float foliageShape(int var1);
	void foliageCluster(int var1, int var2, int var3);
	void limb(const TilePos& start, const TilePos& end, int blockId);
	bool trimBranches(int var1);
	void makeTrunk();
	int checkLine(TilePos& var1, TilePos& var2);
	bool checkLocation();
	void init(real a, real b, real c) override;
	bool place(Level*, Random*, const TilePos& pos) override;
	static const constexpr uint8_t axisConversionArray[] = {2, 0, 0, 1, 2, 1};
private:
	Random m_rnd;
	Level* m_pLevel;
	TilePos m_origin = TilePos();
	int m_height = 0;
	int m_trunkHeight;
	double m_trunkHeightScale = 0.618;
	double m_branchDensity = 1.0;
	double m_branchSlope = 0.381;
	double m_widthScale = 1.0;
	double m_foliageDensity = 1.0;
	int m_trunkWidth = 1;
	int m_heightVariance = 12;
	int m_foliageHeight = 4;
};

class FlowerFeature : public Feature
{
public:
	FlowerFeature(int id);
	bool place(Level*, Random*, const TilePos& pos) override;

private:
	int m_ID;
};

class SpringFeature : public Feature
{
public:
	SpringFeature(int id);
	bool place(Level*, Random*, const TilePos& pos) override;

private:
	int m_ID;
};

class HellSpringFeature : public Feature
{
public:
	HellSpringFeature(int id);
	bool place(Level*, Random*, const TilePos& pos) override;

private:
	int m_ID;
};

class LakeFeature : public Feature
{
public:
	LakeFeature(int id);
	bool place(Level*, Random*, const TilePos& pos) override;

private:
	int m_ID;
};

class ClayFeature : public Feature
{
public:
	ClayFeature(int id, int count);
	bool place(Level*, Random*, const TilePos& pos) override;

private:
	int m_ID;
	int m_count;
};

class OreFeature : public Feature
{
public:
	OreFeature(int id, int count);
	bool place(Level*, Random*, const TilePos& pos) override;

private:
	int m_ID;
	int m_count;
};

class VegetationFeature : public Feature
{
public:
	VegetationFeature(int id, int data, int count = 128);
	bool place(Level*, Random*, const TilePos& pos) override;

private:
	int m_ID;
	int m_data;
	int m_count;
};

class HellFireFeature : public Feature
{
public:
	bool place(Level*, Random*, const TilePos& pos) override;
};


class PumpkinFeature : public Feature
{
public:
	bool place(Level*, Random*, const TilePos& pos) override;
};

class CactusFeature : public Feature
{
public:
	bool place(Level*, Random*, const TilePos& pos) override;
};

class ReedsFeature : public Feature
{
public:
	bool place(Level*, Random*, const TilePos& pos) override;
};

class LightGemFeature : public Feature
{
public:
	bool place(Level*, Random*, const TilePos& pos) override;
};

