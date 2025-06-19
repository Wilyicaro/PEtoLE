#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include "TileEntityRenderer.hpp"
#include <world/tile/entity/MobSpawnerTileEntity.hpp>

class MobSpawnerRenderer : public TileEntityRenderer<MobSpawnerTileEntity>
{
public:
    void renderTyped(MobSpawnerTileEntity* spawner, const Vec3&, float partialTicks) override;

private:
    std::unordered_map<std::string, std::shared_ptr<Entity>> m_models;
};
