#pragma once

#include "TileEntity.hpp"
#include "world/level/Level.hpp"

class PistonMovingTileEntity : public TileEntity
{
public:
    PistonMovingTileEntity();
    PistonMovingTileEntity(TileID tile, int meta, Facing::Name direction, bool extending, bool isSourcePiston);

    int getTileId() const;
    //Not sure if they meant to override TileEntity::getData, as this should be the data from the tile at this pos
    int getData() const override;
    bool isExtending() const;
    Facing::Name getDirection() const;
    bool isSourcePiston() const;
    float getProgress(float) const;
    Vec3f getOff(float) const;

    void tick() override;

    void load(CompoundIO tag) override;
    void save(CompoundIO tag) override;

    void finalTick();

private:
    void moveCollidedEntities(float progress, float force);

private:
    int m_tileId;
    int m_meta;
    Facing::Name m_direction;
    bool m_bExtending;
    bool m_bIsSourcePiston;
    float m_progress;
    float m_progressO;
};
