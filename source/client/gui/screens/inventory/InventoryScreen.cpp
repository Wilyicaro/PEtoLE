#include "InventoryScreen.hpp"
#include <client/renderer/Lighting.hpp>

InventoryScreen::InventoryScreen(Player* player) : ContainerScreen(player->m_inventoryMenu)
{
}

void InventoryScreen::renderLabels()
{
	m_pFont->draw("Crafting", 86, 16, 0x404040);
}

void InventoryScreen::renderBg(int mouseX, int mouseY, float partialTick)
{
    glColor4f(1.0F, 1.0F, 1.0F, 1.0F);
    m_pMinecraft->m_pTextures->loadAndBindTexture("gui/inventory.png");

    blit(m_leftPos, m_topPos, 0, 0, m_imageWidth, m_imageHeight, 256, 256);

    glEnable(GL_RESCALE_NORMAL);
    glEnable(GL_COLOR_MATERIAL);
    glPushMatrix();

    glTranslatef(m_leftPos + 51.0f, m_topPos + 75, 50.0F);
    float scale = 30.0F;
    glScalef(-scale, scale, scale);
    glRotatef(180.0F, 0.0F, 0.0F, 1.0F);

    float prevYBodyRot = m_pMinecraft->m_pLocalPlayer->m_yBodyRot;
    float prevYRot = m_pMinecraft->m_pLocalPlayer->m_rot.x;
    float prevXRot = m_pMinecraft->m_pLocalPlayer->m_rot.y;

    float dx = m_leftPos + 51.0f - mouseX;
    float dy = m_topPos + 75 - 50 - mouseY;

    glRotatef(135.0F, 0.0F, 1.0F, 0.0F);
    Lighting::turnOn();
    glRotatef(-135.0F, 0.0F, 1.0F, 0.0F);

    glRotatef(-Mth::atan(dy / 40.0F) * 20.0F, 1.0F, 0.0F, 0.0F);
    m_pMinecraft->m_pLocalPlayer->m_yBodyRot = Mth::atan(dx / 40.0F) * 20.0F;
    m_pMinecraft->m_pLocalPlayer->m_rot.x = -Mth::atan(dy / 40.0F) * 20.0F;
    m_pMinecraft->m_pLocalPlayer->m_rot.y = Mth::atan(dx / 40.0F) * 40.0F;

    glTranslatef(0.0F, m_pMinecraft->m_pLocalPlayer->m_heightOffset, 0.0F);
    EntityRenderDispatcher::instance->m_rot.y = 180;
    EntityRenderDispatcher::instance->render(m_pMinecraft->m_pLocalPlayer.get(), Vec3::ZERO, 0.0F, 1.0F, false);

    m_pMinecraft->m_pLocalPlayer->m_yBodyRot = prevYBodyRot;
    m_pMinecraft->m_pLocalPlayer->m_rot.x = prevYRot;
    m_pMinecraft->m_pLocalPlayer->m_rot.y = prevXRot;

    glPopMatrix();
    Lighting::turnOff();
    glDisable(GL_RESCALE_NORMAL);
}
