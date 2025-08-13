#pragma once

#include <unordered_map>
#include <typeindex>
#include <memory>
#include <type_traits>
#include <cmath>

#include "world/level/Level.hpp"

#include "Font.hpp"
#include "Textures.hpp"
#include <world/tile/entity/TileEntity.hpp>
#include <world/entity/Player.hpp>

class TileEntityRendererBase;

class TileEntityRenderDispatcher {
public:
    static TileEntityRenderDispatcher* getInstance()
    {
        if (!instance) instance = new TileEntityRenderDispatcher();
        return instance;
    }

    void registerRenderer(TileEntityType*, TileEntityRendererBase*);

    TileEntityRendererBase* getRenderer(const TileEntityType*);

    TileEntityRendererBase* getRenderer(TileEntity* e);

    bool hasRenderer(TileEntity* e);

    void prepare(Level* level, Textures* textures, Font* font, std::shared_ptr<Mob> player, float a);

    void render(TileEntity* e, float a);
    void render(TileEntity* e, const Vec3&, float a);

    void setLevel(Level* level);
    double distanceToSqr(const Vec3&) const;
    Font* getFont() const;


    Textures* textures = nullptr;
    Level* level = nullptr;
    std::shared_ptr<Mob> m_mob = nullptr;

    Vec2 m_rot = Vec2::ZERO;
    Vec3 m_pos = Vec3::ZERO;
    static Vec3 off;


private:
    TileEntityRenderDispatcher();
    std::unordered_map<const TileEntityType*, TileEntityRendererBase*> m_renderers;

    Font* font = nullptr;

    static TileEntityRenderDispatcher* instance;
};
