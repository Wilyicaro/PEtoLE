#include "MinecartRenderer.hpp"
#include "client/model/MinecartModel.hpp"
#include <world/entity/Minecart.hpp>

MinecartRenderer::MinecartRenderer() : m_pModel(new MinecartModel())
{
	m_shadowRadius = 0.5f;
}

void MinecartRenderer::render(Entity* entity, const Vec3& pos, float rot, float a)
{
    Minecart* cart = (Minecart*)entity;
    Vec3 cPos = pos;
    glPushMatrix();
    Vec3 smoothPos = cart->m_oPos + (cart->m_pos - cart->m_oPos) * a;
    real r = 0.3;
    Vec3* p = cart->getPos(smoothPos);
    float xRot = cart->m_rotPrev.x + (cart->m_rot.x - cart->m_rotPrev.x) * a;
    if (p)
    {
        Vec3* p0 = cart->getPosOffs(smoothPos, r);
        Vec3* p1 = cart->getPosOffs(smoothPos, -r);
        if (!p0)
            p0 = p;

        if (!p1)
            p1 = p;

        cPos.x += p->x - smoothPos.x;
        cPos.y += (p0->y + p1->y) / 2.0 - smoothPos.y;
        cPos.z += p->z - smoothPos.z;
        Vec3 dir = *p1 - *p0;
        if (dir.length() != 0.0) {
            dir = dir.normalize();
            rot = Mth::atan2(dir.z, dir.x) * 180.0 / M_PI;
            xRot = Mth::atan(dir.y) * 73.0;
        }

        if (p != p0)
            SAFE_DELETE(p0);
        if (p != p1)
            SAFE_DELETE(p1);
        SAFE_DELETE(p);
    }

    glTranslatef(cPos.x, cPos.y, cPos.z);
    glRotatef(180.0F - rot, 0.0F, 1.0F, 0.0F);
    glRotatef(-xRot, 0.0F, 0.0F, 1.0F);
    float hurt = cart->m_hurtTime - a;
    float dmg = cart->m_damage - a;
    if (dmg < 0.0F) {
        dmg = 0.0F;
    }

    if (hurt > 0.0F) {
        glRotatef(Mth::sin(hurt) * hurt * dmg / 10.0F * cart->m_hurtDir, 1.0F, 0.0F, 0.0F);
    }

    if (cart->m_type != 0) {
        bindTexture(C_TERRAIN_NAME);
        float ss = 0.75F;
        glScalef(ss, ss, ss);
        glTranslatef(0.0F, 0.3125F, 0.0F);
        glRotatef(90.0F, 0.0F, 1.0F, 0.0F);
        if (cart->m_type == 1) {
            m_tileRenderer.renderTile(Tile::chest, 0, getBrightness(entity, a));
        }
        else if (cart->m_type == 2) {
            m_tileRenderer.renderTile(Tile::furnace, 0, getBrightness(entity, a));
        }

        glRotatef(-90.0F, 0.0F, 1.0F, 0.0F);
        glTranslatef(0.0F, -0.3125F, 0.0F);
        glScalef(1.0F / ss, 1.0F / ss, 1.0F / ss);
    }

    bindTexture("item/cart.png");
    glScalef(-1.0F, -1.0F, 1.0F);
    m_pModel->render(0.0F, 0.0F, -0.1F, 0.0F, 0.0F, 0.0625F);
    glPopMatrix();
}
