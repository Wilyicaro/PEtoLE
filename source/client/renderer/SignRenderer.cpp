#include "SignRenderer.hpp"
#include "entity/EntityRenderDispatcher.hpp"

void SignRenderer::renderTyped(SignTileEntity* sign, const Vec3& pos, float partialTicks)
{
    Tile* tile = sign->getTile();
    glPushMatrix();
    float size = 0.6666667F;
    float rot;
    if (tile == Tile::sign) {
        glTranslatef(pos.x + 0.5F, pos.y + 0.75F * size, pos.z + 0.5F);
        float rot = (float)(sign->getData() * 360) / 16.0F;
        glRotatef(-rot, 0.0F, 1.0F, 0.0F);
        m_signModel.m_cube2.m_bVisible = true;
    }
    else {
        int face = sign->getData();
        rot = 0.0F;
        if (face == 2) {
            rot = 180.0F;
        }

        if (face == 4) {
            rot = 90.0F;
        }

        if (face == 5) {
            rot = -90.0F;
        }

        glTranslatef(pos.x + 0.5F, pos.y + 0.75F * size, pos.z + 0.5F);
        glRotatef(-rot, 0.0F, 1.0F, 0.0F);
        glTranslatef(0.0F, -0.3125F, -0.4375F);
        m_signModel.m_cube2.m_bVisible = false;
    }

    bindTexture("item/sign.png");
    glPushMatrix();
    glScalef(size, -size, -size);
    m_signModel.render();
    glPopMatrix();
    Font* font = getFont();
    rot = 0.016666668F * size;
    glTranslatef(0.0F, 0.5F * size, 0.07F * size);
    glScalef(rot, -rot, rot);
    glNormal3f(0.0F, 0.0F, -1.0F * rot);
    glDepthMask(false);
    int col = 0;

    for (int i = 0; i < sign->m_messages.size(); ++i) {
        std::string& msg = sign->m_messages[i];
        if (i == sign->m_selectedLine) {
            std::string select = "> " + msg + " <";
            font->draw(select, -font->width(select) / 2, i * 10 - sign->m_messages.size() * 5, col);
        }
        else {
            font->draw(msg, -font->width(msg) / 2, i * 10 - sign->m_messages.size() * 5, col);
        }
    }

    glDepthMask(true);
    glColor4f(1.0F, 1.0F, 1.0F, 1.0F);
    glPopMatrix();
}
