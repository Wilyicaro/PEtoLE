#include "CraftingScreen.hpp"
#include "world/item/CraftingMenu.hpp"
#include <client/renderer/Lighting.hpp>

CraftingScreen::CraftingScreen(Inventory* inventory, const TilePos& tilePos, Level* level) : ContainerScreen(new CraftingMenu(inventory, tilePos, level))
{
}

void CraftingScreen::renderLabels()
{
	m_pFont->draw("Crafting", 28, 6, 0x404040);
    m_pFont->draw("Inventory", 8, m_imageHeight - 96 + 2, 0x404040);
}

void CraftingScreen::renderBg(int mouseX, int mouseY, float partialTick)
{
    glColor4f(1.0F, 1.0F, 1.0F, 1.0F);
    m_pMinecraft->m_pTextures->loadAndBindTexture("gui/crafting.png");

    blit(m_leftPos, m_topPos, 0, 0, m_imageWidth, m_imageHeight, 256, 256);
}
