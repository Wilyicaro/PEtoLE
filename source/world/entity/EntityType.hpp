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

	inline static EntityType* unknown;
	inline static EntityType* player;

	inline static EntityType* arrow;
	//inline static EntityType* snowball;
	inline static EntityType* item;
	//inline static EntityType* painting;
	inline static EntityType* mob;
	//inline static EntityType* monster;
	//inline static EntityType* creeper;
	//inline static EntityType* skeleton;
	//inline static EntityType* spider;
	//inline static EntityType* giant;
	//inline static EntityType* zombie;
	//inline static EntityType* slime;
	//inline static EntityType* monster;
	//inline static EntityType* ghast;
	//inline static EntityType* pigZombie;
	inline static EntityType* pig;
	inline static EntityType* sheep;
	inline static EntityType* cow;
	inline static EntityType* chicken;
	//inline static EntityType* squid;
	inline static EntityType* primedTnt;
	inline static EntityType* fallingTile;
	//inline static EntityType* minecart;
	//inline static EntityType* boat;



private:
	int m_id;
	std::string m_name;
	EntityCategories m_category;
	EntityFactory m_factory;

	static std::unordered_map<int, const EntityType*> entityTypeIdMap;
	static std::unordered_map<std::string, const EntityType*> entityTypeNameMap;
	static std::vector<const EntityType*> entityTypes;
};