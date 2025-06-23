#include "client/gui/screens/inventory/ContainerScreen.hpp"
#include "client/locale/Language.hpp"
#include "world/item/ItemInstance.hpp"
#include <client/renderer/Lighting.hpp>

ContainerScreen::ContainerScreen(ContainerMenu* menu)
    : menu(menu) {}

void ContainerScreen::init() {
    Screen::init();
    m_pMinecraft->m_pLocalPlayer->m_containerMenu = menu;
    m_leftPos = (m_width - m_imageWidth) / 2;
    m_topPos = (m_height - m_imageHeight) / 2;

}

void ContainerScreen::render(int mouseX, int mouseY, float partialTicks) {
    renderBackground();
    renderBg(mouseX, mouseY, partialTicks);

    //glPushMatrix();
    //glRotatef(-30.0F, 0.0F, 1.5F, 0.0F);
    //glRotatef(200.0F, 1.0F, 0.0F, 0.0F);
    //Lighting::turnOn();
    //glPopMatrix();

    glPushMatrix();
    glTranslatef(m_leftPos, m_topPos, 0.0F);
    glColor4f(1.0F, 1.0F, 1.0F, 1.0F);
    glEnable(GL_RESCALE_NORMAL);

    Slot* hoveredSlot = nullptr;

    for (auto& slot : menu->slots) {
        renderSlot(slot.get());
        if (isHovering(slot.get(), mouseX, mouseY)) {
            hoveredSlot = slot.get();
            //glDisable(GL_LIGHTING);
            glDisable(GL_DEPTH_TEST);
            int slotX = slot->x;
            int slotY = slot->y;
            fillGradient(slotX, slotY, slotX + 16, slotY + 16, -2130706433, -2130706433);
            //glEnable(GL_LIGHTING);
            glEnable(GL_DEPTH_TEST);
        }
    }

    Inventory* inv = m_pMinecraft->m_pLocalPlayer->m_pInventory;
    if (inv->getCarried()) {
        glTranslatef(0.0F, 0.0F, 200.0F);
        ItemRenderer::renderGuiItem(m_pMinecraft->m_pTextures, inv->getCarried(), mouseX - m_leftPos - 8, mouseY - m_topPos - 8);
        ItemRenderer::renderGuiItemDecorations(m_pFont, m_pMinecraft->m_pTextures, inv->getCarried(), mouseX - m_leftPos - 8, mouseY - m_topPos - 8);
    }

    glDisable(GL_RESCALE_NORMAL);
    //Lighting::turnOff();
    //glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    renderLabels();

    if (!inv->getCarried() && hoveredSlot && hoveredSlot->hasItem()) {
        std::string name = Language::getInstance()->get(hoveredSlot->getItem()->getDescriptionId());
        if (!name.empty()) {
            int tx = mouseX - m_leftPos + 12;
            int ty = mouseY - m_topPos - 12;
            int w = m_pFont->width(name);
            fillGradient(tx - 3, ty - 3, tx + w + 3, ty + 8 + 3, -1073741824, -1073741824);
            m_pFont->drawShadow(name, tx, ty, -1);
        }
    }

    //glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
}

void ContainerScreen::mouseClicked(int mouseX, int mouseY, int button) {
    if (m_pMinecraft->isTouchscreen()) return;
    slotClicked(mouseX, mouseY, button);
}

void ContainerScreen::mouseReleased(int mouseX, int mouseY, int button)
{
    if (m_pMinecraft->isTouchscreen() && m_timeSlotDragged < 5)
        slotClicked(mouseX, mouseY, button);
    m_timeSlotDragged = 0;
}

void ContainerScreen::mouseDragged(double x, double y, int button, double deltaX, double deltaY)
{
    if ((button == MouseButtonType::BUTTON_LEFT || button == MouseButtonType::BUTTON_RIGHT) && findSlot(x, y))
        m_timeSlotDragged++;
    else m_timeSlotDragged = 0;

    if (m_pMinecraft->isTouchscreen() && m_timeSlotDragged % 5 == 0)
    {
        slotClicked(x, y, MouseButtonType::BUTTON_RIGHT);
    }
}

void ContainerScreen::slotClicked(int mouseX, int mouseY, int button) {
    if (button == MouseButtonType::BUTTON_LEFT || button == MouseButtonType::BUTTON_RIGHT) {
        Slot* slot = findSlot(mouseX, mouseY);
        bool outside = mouseX < m_leftPos || mouseY < m_topPos || mouseX >= m_leftPos + m_imageWidth || mouseY >= m_topPos + m_imageHeight;
        int index = -1;
        if (slot) index = slot->index;
        if (outside) index = -999;
        if (index != -1) {
            slotClicked(slot, index, button - 1, index != -999 && m_pMinecraft->m_pPlatform->shiftPressed());
        }
    }
}

void ContainerScreen::slotClicked(Slot* slot, int index, int button, bool quick)
{
    m_pMinecraft->m_pGameMode->handleInventoryMouseClick(menu->containerId, index, button, m_pMinecraft->m_pLocalPlayer.get());
}


void ContainerScreen::keyPressed(int keyCode) {
    Screen::keyPressed(keyCode);
}

void ContainerScreen::removed() {
    if (m_pMinecraft->m_pLocalPlayer) {
        m_pMinecraft->m_pGameMode->handleCloseInventory(menu->containerId, m_pMinecraft->m_pLocalPlayer.get());
    }
}

void ContainerScreen::slotsChanged(Container* container) {}

bool ContainerScreen::isPauseScreen() {
    return false;
}

Slot* ContainerScreen::findSlot(int mouseX, int mouseY) {
    for (auto& slot : menu->slots) {
        if (isHovering(slot.get(), mouseX, mouseY)) return slot.get();
    }
    return nullptr;
}

bool ContainerScreen::isHovering(Slot* slot, int mouseX, int mouseY) const {
    mouseX -= m_leftPos;
    mouseY -= m_topPos;
    return mouseX >= slot->x - 1 && mouseX < slot->x + 17 && mouseY >= slot->y - 1 && mouseY < slot->y + 17;
}

void ContainerScreen::renderSlot(Slot* slot) {
    int x = slot->x;
    int y = slot->y;
    auto item = slot->getItem();
    if (!item) {
        int icon = slot->getNoItemIcon();
        if (icon >= 0) {
            //glDisable(GL_LIGHTING);
            m_pMinecraft->m_pTextures->loadAndBindTexture("gui/items.png");
            blit(x, y, (icon % 16) * 16, (icon / 16) * 16, 16, 16, 256, 256);
            //glEnable(GL_LIGHTING);
            return;
        }
    }
    ItemRenderer::renderGuiItem(m_pMinecraft->m_pTextures, item, x, y);
    ItemRenderer::renderGuiItemDecorations(m_pFont, m_pMinecraft->m_pTextures, item, x, y);
}
