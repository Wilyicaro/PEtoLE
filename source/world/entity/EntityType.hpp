#pragma once
#include <string>
#include "EntityCategories.hpp"
#include <functional>
#include <memory>
#include <vector>
#include <unordered_map>

class Level;
class Entity;

using EntityFactory = std::function<std::shared_ptr<Entity>(Level*)>;

class EntityType
{
public:
	EntityType(int type, const std::string& name, const EntityCategories& category, const EntityFactory& factory);


public:
	template <typename T>
	static EntityType* create(int type, const std::string& name, const EntityCategories& category)
	{
		return new EntityType(type, name, category, [](Level* level) -> std::shared_ptr<Entity> {return std::make_shared<T>(level);});
	}

	static EntityType* registerType(EntityType* type) {
		entityTypeIdMap[type->m_id] = type;
		entityTypeNameMap[type->m_name] = type;
		return add(type);
	}

	static EntityType* add(EntityType* type) {
		entityTypes.push_back(type);
		return type;
	}

	int getId() const { return m_id; }
	const std::string& getName() const { return m_name; }
	const EntityCategories& getCategory() const { return m_category; }

	std::shared_ptr<Entity> newEntity(Level* level) const 
	{
		return m_factory(level);
	}

	bool operator==(int other) const { return this->getId() == other; }
	bool operator!=(int other) const { return this->getId() != other; }
	bool operator==(const EntityType& other) const { return this->getId() == other.getId(); }
	bool operator!=(const EntityType& other) const { return this->getId() != other.getId(); }

	static const EntityType* getById(int id)
	{
		return entityTypeIdMap[id];
	}

	static const EntityType* getByName(const std::string& name)
	{
		return entityTypeNameMap[name];
	}


	static void initTypes();
	static void teardownTypes();

	static EntityType* unknown;
	static EntityType* player;

	static EntityType* arrow;
	static EntityType* snowball;
	static EntityType* thrownEgg;
	static EntityType* fireball;
	static EntityType* fishingHook;
	static EntityType* lightningBolt;
	static EntityType* item;
	static EntityType* painting;
	static EntityType* mob;
	static EntityType* monster;
	static EntityType* creeper;
	static EntityType* skeleton;
	static EntityType* spider;
	static EntityType* giant;
	static EntityType* zombie;
	static EntityType* slime;
	static EntityType* ghast;
	static EntityType* pigZombie;
	static EntityType* pig;
	static EntityType* sheep;
	static EntityType* cow;
	static EntityType* chicken;
	static EntityType* squid;
	static EntityType* wolf;
	static EntityType* primedTnt;
	static EntityType* fallingTile;
	static EntityType* minecart;
	static EntityType* boat;



private:
	int m_id;
	std::string m_name;
	EntityCategories m_category;
	EntityFactory m_factory;

	static std::unordered_map<int, const EntityType*> entityTypeIdMap;
	static std::unordered_map<std::string, const EntityType*> entityTypeNameMap;
	static std::vector<const EntityType*> entityTypes;
};