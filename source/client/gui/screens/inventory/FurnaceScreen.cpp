#include "FurnaceScreen.hpp"
#include "world/item/FurnaceMenu.hpp"

FurnaceScreen::FurnaceScreen(Inventory* inventory, std::shared_ptr<FurnaceTileEntity> container)
    : ContainerScreen(new FurnaceMenu(inventory, container)),
      m_inventory(inventory), m_furnace(container)
{
}

void FurnaceScreen::renderLabels() {
    m_pFont->draw(m_furnace->getName(), 60, 6, 0x404040);
    m_pFont->draw(m_inventory->getName(), 8, m_imageHeight - 96 + 2, 0x404040);
}

void FurnaceScreen::renderBg(int mouseX, int mouseY, float partialTicks) {
    m_pMinecraft->m_pTextures->loadAndBindTexture("gui/furnace.png");
    glColor4f(1.0F, 1.0F, 1.0F, 1.0F);

    blit(m_leftPos, m_topPos, 0, 0, m_imageWidth, m_imageHeight, 256, 256);

    int p;
    if (m_furnace->isLit()) {
        p = m_furnace->getLitProgress(12);
        blit(m_leftPos + 56, m_topPos + 36 + 12 - p, 176, 12 - p, 14, p + 2, 256, 256);
    }

    p = m_furnace->getBurnProgress(24);
    blit(m_leftPos + 79, m_topPos + 34, 176, 14, p + 1, 16, 256, 256);
}
