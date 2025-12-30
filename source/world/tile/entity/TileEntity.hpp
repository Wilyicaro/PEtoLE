#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <iostream>
#include <cmath>
#include <common/CompoundTag.hpp>
#include <world/level/Vec3i.hpp>
#include <functional>

#include "TileEntityType.hpp"

class Tile;
class Level;
class Packet;

class TileEntity : public std::enable_shared_from_this<TileEntity>
{
public:
    virtual ~TileEntity() = default;

    static std::shared_ptr<TileEntity> loadStatic(CompoundIO tag);

    virtual void load(CompoundIO tag);
    virtual void save(CompoundIO tag);
    virtual void tick() {}

    bool isRemoved() const;
    void setRemoved();
    void clearRemoved();

    virtual Packet* getUpdatePacket() { return nullptr; }

    const TileEntityType* getType() const { return m_pType; }

    virtual int getData() const;
    void setData(int data);
    void setChanged();

    real distanceToSqr(const Vec3& vec) const;

    virtual Tile* getTile() const;

    Level* m_pLevel = nullptr;
    TilePos m_pos;


protected:
    const TileEntityType* m_pType;
    bool m_bRemove = false;

    virtual std::string getId() const;
};

