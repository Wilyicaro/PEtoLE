#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <iostream>
#include <cmath>
#include <typeindex>
#include <common/CompoundTag.hpp>
#include <world/level/TilePos.hpp>
#include <functional>

class TileEntity;
using TileEntityFactory = std::function<std::shared_ptr<TileEntity>()>;

class Tile;
class Level;
class Packet;

class TileEntity : public std::enable_shared_from_this<TileEntity>
{
public:
    template <typename T>
    static void registerTileEntity(const std::string& id) {
        setId(std::type_index(typeid(T)), id, []() -> std::shared_ptr<TileEntity> {
            return std::make_shared<T>();
            });
    };

    virtual ~TileEntity() = default;

    static void initTileEntities();
    static std::shared_ptr<TileEntity> loadStatic(std::shared_ptr<CompoundTag> tag);

    virtual void load(std::shared_ptr<CompoundTag> tag);
    virtual void save(std::shared_ptr<CompoundTag> tag);
    virtual void tick() {}

    virtual Packet* getUpdatePacket() { return nullptr; }

    int getData() const;
    void setData(int data);
    void setChanged();

    double distanceToSqr(const Vec3& vec) const;

    Tile* getTile() const;

    Level* m_pLevel = nullptr;
    TilePos m_pos;

protected:
    static std::unordered_map<std::string, TileEntityFactory> idClassMap;
    static std::unordered_map<std::type_index, std::string> classIdMap;

    static void setId(std::type_index type, const std::string& id, TileEntityFactory factory);

    virtual std::string getId() const;
};

