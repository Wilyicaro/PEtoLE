#include "FishingHookRenderer.hpp"
#include "EntityRenderDispatcher.hpp"
#include "world/level/Level.hpp"
#include "world/entity/projectile/FishingHook.hpp"


FishingHookRenderer::FishingHookRenderer()
{
}

void FishingHookRenderer::render(Entity* pEntity, const Vec3& pos, float rot, float a)
{
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);
    glEnable(GL_RESCALE_NORMAL);
    glScalef(0.5F, 0.5F, 0.5F);
    int xi = 1;
    int yi = 2;
    bindTexture("particles.png");
    Tesselator& t = Tesselator::instance;
    float u0 = (float)(xi * 8 + 0) / 128.0F;
    float u1 = (float)(xi * 8 + 8) / 128.0F;
    float v0 = (float)(yi * 8 + 0) / 128.0F;
    float v1 = (float)(yi * 8 + 8) / 128.0F;
    float r = 1.0F;
    float xo = 0.5F;
    float yo = 0.5F;
    EntityRenderDispatcher* entityRenderDispatcher = EntityRenderDispatcher::getInstance();
    glRotatef(180.0F - entityRenderDispatcher->m_rot.y, 0.0F, 1.0F, 0.0F);
    glRotatef(-entityRenderDispatcher->m_rot.x, 1.0F, 0.0F, 0.0F);
    t.begin();
    t.normal(0.0F, 1.0F, 0.0F);
    t.vertexUV((0.0F - xo), (0.0F - yo), 0.0, u0, v1);
    t.vertexUV((r - xo), (0.0F - yo), 0.0, u1, v1);
    t.vertexUV((r - xo), (1.0F - yo), 0.0, u1, v0);
    t.vertexUV((0.0F - xo), (1.0F - yo), 0.0, u0, v0);
    t.draw();
    glDisable(GL_RESCALE_NORMAL);
    glPopMatrix();
    FishingHook* hook = (FishingHook*)pEntity;
    if (hook->m_owner)
    {
        float rr = (hook->m_owner->m_rotPrev.y + (hook->m_owner->m_rot.y - hook->m_owner->m_rotPrev.y) * a) * M_PI / 180.0F;
        real ss = Mth::sin(rr);
        real cc = Mth::cos(rr);
        float var25 = hook->m_owner->getAttackAnim(a);
        float var26 = Mth::sin(Mth::sqrt(var25) * M_PI);
        Vec3 var27 = Vec3(-0.5, 0.03, 0.8);
        var27.xRot(-(hook->m_owner->m_rotPrev.x + (hook->m_owner->m_rot.x - hook->m_owner->m_rotPrev.x) * a) * M_PI / 180.0F);
        var27.yRot(-(hook->m_owner->m_rotPrev.y + (hook->m_owner->m_rot.y - hook->m_owner->m_rotPrev.y) * a) * M_PI / 180.0F);
        var27.yRot(var26 * 0.5F);
        var27.xRot(-var26 * 0.7F);
        real xp = hook->m_owner->m_oPos.x + (hook->m_owner->m_pos.x - hook->m_owner->m_oPos.x) * a + var27.x;
        real yp = hook->m_owner->m_oPos.y + hook->m_owner->m_heightOffset + (hook->m_owner->m_pos.y - hook->m_owner->m_oPos.y) * a + var27.y;
        real zp = hook->m_owner->m_oPos.z + (hook->m_owner->m_pos.z - hook->m_owner->m_oPos.z) * a + var27.z;
        if (entityRenderDispatcher->m_pOptions->m_bThirdPerson)
        {
            rr = (hook->m_owner->m_yBodyRotO + (hook->m_owner->m_yBodyRot - hook->m_owner->m_yBodyRotO) * a) * M_PI / 180.0F;
            ss = Mth::sin(rr);
            cc = Mth::cos(rr);
            xp = hook->m_owner->m_oPos.x + (hook->m_owner->m_pos.x - hook->m_owner->m_oPos.x) * a - cc * 0.35 - ss * 0.85;
            yp = hook->m_owner->m_oPos.y + hook->m_owner->m_heightOffset + (hook->m_owner->m_pos.y - hook->m_owner->m_oPos.y) * a - 0.45;
            zp = hook->m_owner->m_oPos.z + (hook->m_owner->m_pos.z - hook->m_owner->m_oPos.z) * a - ss * 0.35 + cc * 0.85;
        }

        real xh = hook->m_oPos.x + (hook->m_pos.x - hook->m_oPos.x) * a;
        real yh = hook->m_oPos.y + (hook->m_pos.y - hook->m_oPos.y) * a + 0.25;
        real zh = hook->m_oPos.z + (hook->m_pos.z - hook->m_oPos.z) * a;
        real xa = (xp - xh);
        real ya = (yp - yh);
        real za = (zp - zh);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
        t.begin(3);
        t.color(0);
        int steps = 16;

        for (int i = 0; i <= steps; ++i) {
            float aa = (float)i / (float)steps;
            t.vertex(pos.x + xa * aa, pos.y + ya * (aa * aa + aa) * 0.5 + 0.25, pos.z + za * aa);
        }

        t.draw();
        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
    }
}