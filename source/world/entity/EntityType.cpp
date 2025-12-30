#include "EntityType.hpp"
#include "ItemEntity.hpp"
#include "Pig.hpp"
#include "Sheep.hpp"
#include "Cow.hpp"
#include "Chicken.hpp"
#include "PrimedTnt.hpp"
#include "FallingTile.hpp"
#include "projectile/Arrow.hpp"
#include "projectile/Snowball.hpp"
#include "projectile/ThrownEgg.hpp"
#include "projectile/Fireball.hpp"
#include "projectile/FishingHook.hpp"
#include "Minecart.hpp"
#include "Boat.hpp"
#include "MobSpawner.hpp"
#include "Monster.hpp"
#include "Creeper.hpp"
#include "Ghast.hpp"
#include "Slime.hpp"
#include "Painting.hpp"
#include "LightningBolt.hpp"
#include "Squid.hpp"
#include "Wolf.hpp"
#include "Zombie.hpp"
#include "Skeleton.hpp"
#include "Spider.hpp"
#include "Giant.hpp"
#include "PigZombie.hpp"
#include <climits>

std::unordered_map<int, const EntityType*> EntityType::entityTypeIdMap;
std::unordered_map<std::string, const EntityType*> EntityType::entityTypeNameMap;
std::vector<const EntityType*> EntityType::entityTypes;

EntityType::EntityType(int id, const std::string& name, const EntityCategories& category, const EntityFactory& factory, int range, int updateInterval, bool trackDelta) :
	m_id(id),
	m_name(name),
	m_category(category),
	m_factory(factory),
	m_range(range),
	m_updateInterval(updateInterval),
	m_bTrackDelta(trackDelta)
{
}

void EntityType::initTypes()
{
	unknown = add(create<Entity>(-1, "", EntityCategories(EntityCategories::ENTITY)));
	player = add(create<Mob>(-1, "", EntityCategories(EntityCategories::MOB), 512, 2));

	arrow = registerType(create<Arrow>(10, "Arrow", EntityCategories(EntityCategories::PROJECTILE), 64, 20));
	snowball = registerType(create<Snowball>(11, "Snowball", EntityCategories(EntityCategories::PROJECTILE), 64, 10, true));
	//@Note: Not registered in the original, so they wouldn't be loaded, but we will be doing this here :)
	thrownEgg = registerType(create<ThrownEgg>(8, "ThrownEgg", EntityCategories(EntityCategories::PROJECTILE), 64, 10, true));
	fireball = registerType(create<Fireball>(7, "Fireball", EntityCategories(EntityCategories::PROJECTILE)));
	fishingHook = registerType(create<FishingHook>(6, "FishingHook", EntityCategories(EntityCategories::PROJECTILE)));
	lightningBolt = registerType(create<LightningBolt>(5, "LightningBolt", EntityCategories(EntityCategories::ENTITY)));

	item = registerType(create<ItemEntity>(1, "Item", EntityCategories(EntityCategories::ENTITY), 64, 20, true));
	painting = registerType(create<Painting>(9, "Painting", EntityCategories(EntityCategories::ENTITY), 160, INT_MAX));
	mob = registerType(create<Mob>(48, "Mob", EntityCategories(EntityCategories::MOB)));
	monster = registerType(create<Monster>(49, "Monster", EntityCategories(EntityCategories::MONSTER)));
	creeper = registerType(create<Creeper>(50, "Creeper", EntityCategories(EntityCategories::MONSTER)));
	skeleton = registerType(create<Skeleton>(51, "Skeleton", EntityCategories(EntityCategories::SKELETON_MONSTER)));
	spider = registerType(create<Spider>(52, "Spider", EntityCategories(EntityCategories::ARTHROPOD)));
	giant = registerType(create<Giant>(53, "Giant", EntityCategories(EntityCategories::ZOMBIE_MONSTER)));
	zombie = registerType(create<Zombie>(54, "Zombie", EntityCategories(EntityCategories::ZOMBIE_MONSTER)));
	slime = registerType(create<Slime>(55, "Slime", EntityCategories(EntityCategories::MONSTER)));
	ghast = registerType(create<Ghast>(56, "Ghast", EntityCategories(EntityCategories::MONSTER)));
	pigZombie = registerType(create<PigZombie>(57, "PigZombie", EntityCategories(EntityCategories::UNDEAD_MOB)));
	pig = registerType(create<Pig>(90, "Pig", EntityCategories(EntityCategories::ANIMAL)));
	sheep = registerType(create<Sheep>(91, "Sheep", EntityCategories(EntityCategories::ANIMAL)));
	cow = registerType(create<Cow>(92, "Cow", EntityCategories(EntityCategories::ANIMAL)));
	chicken = registerType(create<Chicken>(93, "Chicken", EntityCategories(EntityCategories::ANIMAL)));
	squid = registerType(create<Squid>(94, "Squid", EntityCategories(EntityCategories::WATER_ANIMAL), 160, 3, true));
	wolf = registerType(create<Wolf>(95, "Wolf", EntityCategories(EntityCategories::ANIMAL)));
	primedTnt = registerType(create<PrimedTnt>(20, "PrimedTnt", EntityCategories(EntityCategories::ENTITY), 160, 10, true));
	fallingTile = registerType(create<FallingTile>(21, "FallingSand", EntityCategories(EntityCategories::ENTITY), 160, 20, true));
	minecart = registerType(create<Minecart>(40, "Minecart", EntityCategories(EntityCategories::MINECART), 160, 5, true));
	boat = registerType(create<Boat>(41, "Boat", EntityCategories(EntityCategories::ENTITY), 160, 5, true));
	MobSpawner::nightmareEntities.push_back(spider);
	MobSpawner::nightmareEntities.push_back(zombie);
	MobSpawner::nightmareEntities.push_back(skeleton);
}

void EntityType::teardownTypes()
{
	entityTypeIdMap.clear();
	entityTypeNameMap.clear();
	entityTypes.clear();
}

EntityType* EntityType::unknown;
EntityType* EntityType::player;

EntityType* EntityType::arrow;
EntityType* EntityType::snowball;
EntityType* EntityType::thrownEgg;
EntityType* EntityType::fireball;
EntityType* EntityType::fishingHook;
EntityType* EntityType::lightningBolt;
EntityType* EntityType::item;
EntityType* EntityType::painting;
EntityType* EntityType::mob;
EntityType* EntityType::monster;
EntityType* EntityType::creeper;
EntityType* EntityType::skeleton;
EntityType* EntityType::spider;
EntityType* EntityType::giant;
EntityType* EntityType::zombie;
EntityType* EntityType::slime;
EntityType* EntityType::ghast;
EntityType* EntityType::pigZombie;
EntityType* EntityType::pig;
EntityType* EntityType::sheep;
EntityType* EntityType::cow;
EntityType* EntityType::chicken;
EntityType* EntityType::squid;
EntityType* EntityType::wolf;
EntityType* EntityType::primedTnt;
EntityType* EntityType::fallingTile;
EntityType* EntityType::minecart;
EntityType* EntityType::boat;
