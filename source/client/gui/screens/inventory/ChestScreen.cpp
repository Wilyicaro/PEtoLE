#include "ChestScreen.hpp"
#include "world/item/ChestMenu.hpp"

ChestScreen::ChestScreen(Container* inventory, Container* container)
    : ContainerScreen(new ChestMenu(inventory, container)),
      m_inventory(inventory), m_container(container)
{
    int defaultHeight = 222;
    int noRowHeight = defaultHeight - 108;
    m_containerRows = m_container->getContainerSize() / 9;
    m_imageHeight = noRowHeight + m_containerRows * 18;
}

void ChestScreen::renderLabels() {
    m_pFont->draw(m_container->getName(), 8, 6, 0x404040);
    m_pFont->draw(m_inventory->getName(), 8, m_imageHeight - 96 + 2, 0x404040);
}

void ChestScreen::renderBg(int mouseX, int mouseY, float partialTicks) {
    m_pMinecraft->m_pTextures->loadAndBindTexture("gui/container.png");
    glColor4f(1.0F, 1.0F, 1.0F, 1.0F);

    blit(m_leftPos, m_topPos, 0, 0, m_imageWidth, m_containerRows * 18 + 17, 256, 256);
    blit(m_leftPos, m_topPos + m_containerRows * 18 + 17, 0, 126, m_imageWidth, 96, 256, 256);
}
