#include "MobSpawnerRenderer.hpp"
#include "entity/EntityRenderDispatcher.hpp"

void MobSpawnerRenderer::renderTyped(MobSpawnerTileEntity* spawner, const Vec3& pos, float partialTicks)
{
    glPushMatrix();
    glTranslatef(pos.x + 0.5f, pos.y, pos.z + 0.5f);

    const std::string& id = spawner->getEntityId();
    auto it = m_models.find(id);
    std::shared_ptr<Entity> e;

    if (it == m_models.end()) {
        e = EntityIO::newEntity(id, nullptr);
        if (e) {
            m_models[id] = e;
        }
    } else {
        e = it->second;
    }

    if (e) {
        e->setLevel(spawner->m_pLevel);

        float scale = 0.4375f;

        glTranslatef(0.0f, 0.4f, 0.0f);
        float spin = float(spawner->oSpin + (spawner->spin - spawner->oSpin) * partialTicks) * 10.0f;
        glRotatef(spin, 0.0f, 1.0f, 0.0f);
        glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
        glTranslatef(0.0f, -0.4f, 0.0f);

        glScalef(scale, scale, scale);

        e->moveTo(pos, Vec2::ZERO);
        EntityRenderDispatcher::getInstance()->render(e.get(), Vec3::ZERO, 0.0f, partialTicks);
    }

    glPopMatrix();
}
