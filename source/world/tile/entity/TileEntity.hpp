#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <iostream>
#include <cmath>
#include <common/CompoundTag.hpp>
#include <world/level/TilePos.hpp>
#include <functional>

#include "TileEntityType.hpp"

class Tile;
class Level;
class Packet;

class TileEntity : public std::enable_shared_from_this<TileEntity>
{
public:
    virtual ~TileEntity() = default;

    static std::shared_ptr<TileEntity> loadStatic(std::shared_ptr<CompoundTag> tag);

    virtual void load(std::shared_ptr<CompoundTag> tag);
    virtual void save(std::shared_ptr<CompoundTag> tag);
    virtual void tick() {}

    virtual Packet* getUpdatePacket() { return nullptr; }

    const TileEntityType* getType() const { return m_pType; }

    int getData() const;
    void setData(int data);
    void setChanged();

    real distanceToSqr(const Vec3& vec) const;

    Tile* getTile() const;

    Level* m_pLevel = nullptr;
    TilePos m_pos;


protected:
    const TileEntityType* m_pType;

    virtual std::string getId() const;
};

