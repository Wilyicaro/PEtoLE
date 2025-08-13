#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include "TileEntityRenderer.hpp"
#include <world/tile/entity/SignTileEntity.hpp>
#include <client/model/SignModel.hpp>

class SignRenderer : public TileEntityRenderer<SignTileEntity>
{
public:
    void renderTyped(SignTileEntity* spawner, const Vec3&, float partialTicks) override;

    SignModel m_signModel;
};
