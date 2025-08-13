#pragma once
#include <string>
#include <functional>
#include <memory>
#include <vector>
#include <unordered_map>

class Level;
class TileEntity;

using TileEntityFactory = std::function<std::shared_ptr<TileEntity>()>;

class TileEntityType
{
public:
	TileEntityType(const std::string& name, const TileEntityFactory& factory);


public:
	template <typename T>
	static TileEntityType* registerType(const std::string& name) {
		TileEntityType* type = new TileEntityType(name, []() -> std::shared_ptr<TileEntity> {
            return std::make_shared<T>();
        });
		entityTypeNameMap[type->m_name] = type;
		entityTypes.push_back(type);
		return type;
	}

	const std::string& getName() const { return m_name; }

	std::shared_ptr<TileEntity> newTileEntity() const 
	{
		return m_factory();
	}

	bool operator==(const std::string& other) const { return this->getName() == other; }
	bool operator!=(const std::string& other) const { return this->getName() != other; }
	bool operator==(const TileEntityType& other) const { return this->getName() == other.getName(); }
	bool operator!=(const TileEntityType& other) const { return this->getName() != other.getName(); }

	static const TileEntityType* getByName(const std::string& name)
	{
		return entityTypeNameMap[name];
	}


	static void initTypes();
	static void teardownTypes();

	static TileEntityType* furnace;
	static TileEntityType* chest;
	static TileEntityType* trap;
	static TileEntityType* sign;
	static TileEntityType* mobSpawner;
	static TileEntityType* music;
	static TileEntityType* recordPlayer;

private:
	std::string m_name;
	TileEntityFactory m_factory;

	static std::unordered_map<std::string, const TileEntityType*> entityTypeNameMap;
	static std::vector<const TileEntityType*> entityTypes;
};