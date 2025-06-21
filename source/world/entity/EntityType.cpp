#include "EntityType.hpp"
#include "ItemEntity.hpp"
#include "Pig.hpp"
#include "Sheep.hpp"
#include "Cow.hpp"
#include "Chicken.hpp"
#include "PrimedTnt.hpp"
#include "FallingTile.hpp"
#include "projectile/Arrow.hpp"

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

	arrow = registerType(create<Arrow>(10, "Arrow", EntityCategories(EntityCategories::PROJECTILE)));
	//snowball = registerType(create<Snowball>(11, "Snowball", EntityCategories(EntityCategories::PROJECTILE)));
	item = registerType(create<ItemEntity>(1, "Item", EntityCategories(EntityCategories::ENTITY)));
	//painting = registerType(create<Painting>(11, "Painting", EntityCategories(EntityCategories::ENTITY)));
	mob = registerType(create<Mob>(48, "Mob", EntityCategories(EntityCategories::MOB)));
	//monster = registerType(create<Monster>(49, "Monster", EntityCategories(EntityCategories::MONSTER)));
	//creeper = registerType(create<Mob>(50, "Creeper", EntityCategories(EntityCategories::MONSTER)));
	//skeleton = registerType(create<Skeleton>(51, "Skeleton", EntityCategories(EntityCategories::SKELETON_MONSTER)));
	//spider = registerType(create<Spider>(52, "Spider", EntityCategories(EntityCategories::ARTHROPOD)));
	//giant = registerType(create<Giant>(53, "Giant", EntityCategories(EntityCategories::ZOMBIE_MONSTER)));
	//zombie = registerType(create<Zombie>(54, "Zombie", EntityCategories(EntityCategories::ZOMBIE_MONSTER)));
	//slime = registerType(create<Slime>(55, "Slime", EntityCategories(EntityCategories::MONSTER)));
	//ghast = registerType(create<Ghast>(56, "Ghast", EntityCategories(EntityCategories::MONSTER)));
	//pigZombie = registerType(create<PigZombie>(57, "PígZombie", EntityCategories(EntityCategories::UNDEAD_MOB)));
	pig = registerType(create<Pig>(90, "Pig", EntityCategories(EntityCategories::ANIMAL)));
	sheep = registerType(create<Sheep>(91, "Sheep", EntityCategories(EntityCategories::ANIMAL)));
	cow = registerType(create<Cow>(92, "Cow", EntityCategories(EntityCategories::ANIMAL)));
	chicken = registerType(create<Chicken>(93, "Chicken", EntityCategories(EntityCategories::ANIMAL)));
	//squid = registerType(create<Squid>(94, "Squid", EntityCategories(EntityCategories::WATER_ANIMAL)));
	primedTnt = registerType(create<PrimedTnt>(20, "PrimedTnt", EntityCategories(EntityCategories::ENTITY)));
	fallingTile = registerType(create<FallingTile>(21, "FallingSand", EntityCategories(EntityCategories::ENTITY)));
	//minecart = registerType(create<Minecart>(40, "Minecart", EntityCategories(EntityCategories::ENTITY)));
	//boat = registerType(create<Boat>(41, "Boat", EntityCategories(EntityCategories::ENTITY)));
}

void EntityType::teardownTypes()
{
	entityTypeIdMap.clear();
	entityTypeNameMap.clear();
	entityTypes.clear();
}
