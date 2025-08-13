#include "TileEntityRenderDispatcher.hpp"
#include "TileEntityRenderer.hpp"
#include "MobSpawnerRenderer.hpp"
#include "SignRenderer.hpp"

Vec3 TileEntityRenderDispatcher::off = Vec3::ZERO;

TileEntityRenderDispatcher* TileEntityRenderDispatcher::instance;

TileEntityRenderDispatcher::TileEntityRenderDispatcher()
{
    registerRenderer(TileEntityType::sign, new SignRenderer);
    registerRenderer(TileEntityType::mobSpawner, new MobSpawnerRenderer);
}

void TileEntityRenderDispatcher::registerRenderer(TileEntityType* type, TileEntityRendererBase* tileEntityRenderer)
{
    m_renderers[type] = tileEntityRenderer;
    tileEntityRenderer->init(this);
}

TileEntityRendererBase* TileEntityRenderDispatcher::getRenderer(const TileEntityType* type)
{
    if (!type) return nullptr;

    auto it = m_renderers.find(type);
    if (it != m_renderers.end())
        return it->second;

    return nullptr;
}

TileEntityRendererBase* TileEntityRenderDispatcher::getRenderer(TileEntity* e)
{
    return e ? getRenderer(e->getType()) : nullptr;
}

bool TileEntityRenderDispatcher::hasRenderer(TileEntity* e) {
    return getRenderer(e) != nullptr;
}

void TileEntityRenderDispatcher::prepare(Level* level, Textures* textures, Font* font, std::shared_ptr<Mob> mob, float a)
{
    this->level = level;
    this->textures = textures;
    this->font = font;
    this->m_mob = mob;

    m_rot = mob->m_rotPrev + (mob->m_rot - mob->m_rotPrev) * a;
    m_pos = mob->m_posPrev + (mob->m_pos - mob->m_posPrev) * a;
}

void TileEntityRenderDispatcher::render(TileEntity* e, float a)
{
    if (!e || !level) return;

    if (e->distanceToSqr(m_pos) < 4096.0) {
        float br = level->getBrightness(e->m_pos);
        glColor4f(br, br, br, 1.0f);
        render(e, -off + e->m_pos, a);
    }
}

void TileEntityRenderDispatcher::render(TileEntity* e, const Vec3& vec, float a)
{
    TileEntityRendererBase* renderer = getRenderer(e);
    if (renderer) {
        renderer->render(e, vec, a);
    }
}

void TileEntityRenderDispatcher::setLevel(Level* level) {
    this->level = level;
}

double TileEntityRenderDispatcher::distanceToSqr(const Vec3& vec) const {
    return vec.distanceToSqr(m_pos);
}

Font* TileEntityRenderDispatcher::getFont() const {
    return font;
}
