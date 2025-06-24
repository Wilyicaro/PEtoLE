#include "MobCategory.hpp"
#include "world/level/Material.hpp"

MobCategory* MobCategory::monster       = new MobCategory(EntityCategories(EntityCategories::MONSTER),      10, 20, nullptr, false);
MobCategory* MobCategory::creature      = new MobCategory(EntityCategories(EntityCategories::ANIMAL),       10, 15, nullptr, true);
MobCategory* MobCategory::waterCreature = new MobCategory(EntityCategories(EntityCategories::WATER_ANIMAL), 5,  10, nullptr, true);
std::vector<MobCategory*> MobCategory::values;
MobCategory::MobCategory(const EntityCategories& baseType, int unknown, int max, const Material* material, bool friendly)
	: m_baseType(baseType)
{
	m_maxInstancesPerChunk = max;
	field_4 = unknown;
	m_pSpawnPositionMaterial = material;
	m_bIsFriendly = friendly;
}

void MobCategory::initMobCategories()
{
	values.push_back(monster);
	values.push_back(creature);
	values.push_back(waterCreature);
	MobCategory::monster->m_pSpawnPositionMaterial       = Material::air;
	MobCategory::creature->m_pSpawnPositionMaterial      = Material::air;
	MobCategory::waterCreature->m_pSpawnPositionMaterial = Material::water;
}