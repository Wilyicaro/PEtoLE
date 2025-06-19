#include "EntityType.hpp"
#include "ItemEntity.hpp"
#include "Pig.hpp"
#include "Sheep.hpp"
#include "Cow.hpp"
#include "Chicken.hpp"
#include "PrimedTnt.hpp"
#include "FallingTile.hpp"

std::unordered_map<int, const EntityType*> EntityType::entityTypeIdMap;
std::unordered_map<std::string, const EntityType*> EntityType::entityTypeNameMap;
std::vector<const EntityType*> EntityType::entityTypes;

EntityType::EntityType(int id, const std::string& name, const EntityCategories& category, const EntityFactory& factory) : m_id(id), m_name(name), m_category(category), m_factory(factory)
{
}

void EntityType::initTypes()
{
	unknown = add(create<Entity>(-1, "", EntityCategories(EntityCategories::ENTITY)));
	player = add(create<Mob>(-1, "", EntityCategories(EntityCategories::MOB)));

	item = registerType(create<ItemEntity>(1, "Item", EntityCategories(EntityCategories::ENTITY)));
	pig = registerType(create<Pig>(90, "Pig", EntityCategories(EntityCategories::ANIMAL)));
	sheep = registerType(create<Sheep>(91, "Sheep", EntityCategories(EntityCategories::ANIMAL)));
	cow = registerType(create<Cow>(92, "Cow", EntityCategories(EntityCategories::ANIMAL)));
	chicken = registerType(create<Chicken>(93, "Chicken", EntityCategories(EntityCategories::ANIMAL)));
	//squid = registerType(create<Squid>(94, "Squid", EntityCategories(EntityCategories::WATER_ANIMAL)));
	primedTnt = registerType(create<PrimedTnt>(20, "PrimedTnt", EntityCategories(EntityCategories::ENTITY)));
	fallingTile = registerType(create<FallingTile>(21, "FallingSand", EntityCategories(EntityCategories::ENTITY)));
}

void EntityType::teardownTypes()
{
	entityTypeIdMap.clear();
	entityTypeNameMap.clear();
	entityTypes.clear();
}
