#include "BoatRenderer.hpp"
#include "client/model/BoatModel.hpp"
#include <world/entity/Boat.hpp>

BoatRenderer::BoatRenderer() : m_pModel(new BoatModel())
{
	m_shadowRadius = 0.5f;
}

void BoatRenderer::render(Entity* entity, float x, float y, float z, float rot, float a)
{
    Boat* boat = (Boat*)entity;
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(180.0F - rot, 0.0F, 1.0F, 0.0F);
    float hurt = boat->m_hurtTime - a;
    float dmg = boat->m_damage - a;
    if (dmg < 0.0F) {
        dmg = 0.0F;
    }

    if (hurt > 0.0F) {
        glRotatef(Mth::sin(hurt) * hurt * dmg / 10.0F * boat->m_hurtDir, 1.0F, 0.0F, 0.0F);
    }

    bindTexture("item/boat.png");
    glScalef(-1.0F, -1.0F, 1.0F);
    m_pModel->render(0.0F, 0.0F, -0.1F, 0.0F, 0.0F, 0.0625F);
    glPopMatrix();
}
