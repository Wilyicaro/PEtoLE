#include "MobCategory.hpp"
#include "world/level/Material.hpp"

std::vector<MobCategory*> MobCategory::values;
MobCategory::MobCategory(const EntityCategories& baseType, int max, const Material* material, bool friendly)
	: m_baseType(baseType)
{
	m_maxInstancesPerChunk = max;
	m_pSpawnPositionMaterial = material;
	m_bIsFriendly = friendly;
	values.push_back(this);
}

void MobCategory::initMobCategories()
{
	monster = new MobCategory(EntityCategories(EntityCategories::MONSTER), 70, Material::air, false);
	creature = new MobCategory(EntityCategories(EntityCategories::ANIMAL), 15, Material::air, true);
	waterCreature = new MobCategory(EntityCategories(EntityCategories::WATER_ANIMAL), 5, Material::water, true);
}

MobCategory* MobCategory::monster;
MobCategory* MobCategory::creature;
MobCategory* MobCategory::waterCreature;