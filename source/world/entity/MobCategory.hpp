#pragma once
#include "EntityCategories.hpp"
#include <vector>

class Material;

class MobCategory
{
public:
	static MobCategory* monster;
	static MobCategory* creature;
	static MobCategory* waterCreature;
	static std::vector<MobCategory*> values;

private:
	MobCategory(const EntityCategories&, int, const Material*, bool);

public:
	static void initMobCategories();

public:
	const EntityCategories& getBaseType() const { return m_baseType; }
	int getMaxInstancesPerChunk() const { return m_maxInstancesPerChunk; }
	const Material* getSpawnPositionMaterial() const { return m_pSpawnPositionMaterial; }
	bool isFriendly() const { return m_bIsFriendly; }

private:
	EntityCategories m_baseType;
	int m_maxInstancesPerChunk;
	const Material* m_pSpawnPositionMaterial;
	bool m_bIsFriendly;
};

