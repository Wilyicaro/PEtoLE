#include "ArrowRenderer.hpp"
#include "world/entity/projectile/Arrow.hpp"

ArrowRenderer::ArrowRenderer()
{
}

void ArrowRenderer::render(Entity* entity, const Vec3& pos, float rot, float a)
{
    Arrow* arrow = (Arrow*)entity;
    if (!arrow) return;

    bindTexture("item/arrows.png");
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);
    glRotatef(arrow->m_rotPrev.y + (arrow->m_rot.y - arrow->m_rotPrev.y) * a - 90.0F, 0.0F, 1.0F, 0.0F);
    glRotatef(arrow->m_rotPrev.x + (arrow->m_rot.x - arrow->m_rotPrev.x) * a, 0.0F, 0.0F, 1.0F);
    Tesselator& t = Tesselator::instance;
    int type = 0;
    float u0 = 0.0F;
    float u1 = 0.5F;
    float v0 = (float)(0 + type * 10) / 32.0F;
    float v1 = (float)(5 + type * 10) / 32.0F;
    float u02 = 0.0F;
    float u12 = 0.15625F;
    float v02 = (float)(5 + type * 10) / 32.0F;
    float v12 = (float)(10 + type * 10) / 32.0F;
    float ss = 0.05625F;
    glEnable(32826);
    float shake = (float)arrow->m_shakeTime - a;
    if (shake > 0.0F) {
        float pow = -Mth::sin(shake * 3.0F) * shake;
        glRotatef(pow, 0.0F, 0.0F, 1.0F);
    }

    glRotatef(45.0F, 1.0F, 0.0F, 0.0F);
    glScalef(ss, ss, ss);
    glTranslatef(-4.0F, 0.0F, 0.0F);
    glNormal3f(ss, 0.0F, 0.0F);
    t.begin();
    t.vertexUV(-7.0, -2.0, -2.0, (double)u02, (double)v02);
    t.vertexUV(-7.0, -2.0, 2.0, (double)u12, (double)v02);
    t.vertexUV(-7.0, 2.0, 2.0, (double)u12, (double)v12);
    t.vertexUV(-7.0, 2.0, -2.0, (double)u02, (double)v12);
    t.draw();
    glNormal3f(-ss, 0.0F, 0.0F);
    t.begin();
    t.vertexUV(-7.0, 2.0, -2.0, (double)u02, (double)v02);
    t.vertexUV(-7.0, 2.0, 2.0, (double)u12, (double)v02);
    t.vertexUV(-7.0, -2.0, 2.0, (double)u12, (double)v12);
    t.vertexUV(-7.0, -2.0, -2.0, (double)u02, (double)v12);
    t.draw();

    for (int i = 0; i < 4; ++i) {
        glRotatef(90.0F, 1.0F, 0.0F, 0.0F);
        glNormal3f(0.0F, 0.0F, ss);
        t.begin();
        t.vertexUV(-8.0, -2.0, 0.0, (double)u0, (double)v0);
        t.vertexUV(8.0, -2.0, 0.0, (double)u1, (double)v0);
        t.vertexUV(8.0, 2.0, 0.0, (double)u1, (double)v1);
        t.vertexUV(-8.0, 2.0, 0.0, (double)u0, (double)v1);
        t.draw();
    }

    glDisable(32826);
    glPopMatrix();
}
