#include "TrapScreen.hpp"
#include "world/item/TrapMenu.hpp"

TrapScreen::TrapScreen(Container* inventory, std::shared_ptr<DispenserTileEntity> trap)
    : ContainerScreen(new TrapMenu(inventory, trap)),
      m_inventory(inventory), m_trap(trap)
{
}

void TrapScreen::renderLabels() {
    m_pFont->draw(m_trap->getName(), 60, 6, 0x404040);
    m_pFont->draw(m_inventory->getName(), 8, m_imageHeight - 96 + 2, 0x404040);
}

void TrapScreen::renderBg(int mouseX, int mouseY, float partialTicks) {
    m_pMinecraft->m_pTextures->loadAndBindTexture("gui/trap.png");
    glColor4f(1.0F, 1.0F, 1.0F, 1.0F);

    blit(m_leftPos, m_topPos, 0, 0, m_imageWidth, m_imageHeight, 256, 256);
}
