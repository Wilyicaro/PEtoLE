#pragma once

#include <string>
#include "TileEntityRenderDispatcher.hpp"
#include "Textures.hpp"
#include "Font.hpp"

class TileEntityRendererBase {
public:
    virtual ~TileEntityRendererBase() = default;
    virtual void init(TileEntityRenderDispatcher* dispatcher) = 0;
    virtual void render(TileEntity* entity, const Vec3& pos, float partialTicks) = 0;
};

template<typename T>
class TileEntityRenderer : public TileEntityRendererBase {
public:
    TileEntityRenderer() = default;

    void init(TileEntityRenderDispatcher* dispatcher) override 
    {
        this->tileEntityRenderDispatcher = dispatcher;
    }

    void render(TileEntity* entity, const Vec3& pos, float partialTicks) override 
    {
        renderTyped(static_cast<T*>(entity), pos, partialTicks);
    }

    virtual void renderTyped(T* entity, const Vec3& pos, float partialTicks) = 0;

protected:
    TileEntityRenderDispatcher* tileEntityRenderDispatcher = nullptr;

    void bindTexture(const std::string& resourceName) {
        if (!tileEntityRenderDispatcher || !tileEntityRenderDispatcher->textures) return;
        tileEntityRenderDispatcher->textures->loadAndBindTexture(resourceName);
    }

    void bindTexture(const std::string& urlTexture, const std::string& fallbackTexture) {
        if (!tileEntityRenderDispatcher || !tileEntityRenderDispatcher->textures) return;
        Textures* t = tileEntityRenderDispatcher->textures;
        t->bind(t->loadHttpTexture(urlTexture, fallbackTexture));
    }

    Font* getFont() {
        return tileEntityRenderDispatcher ? tileEntityRenderDispatcher->getFont() : nullptr;
    }

    Level* getLevel() {
        return tileEntityRenderDispatcher ? tileEntityRenderDispatcher->level : nullptr;
    }
};
