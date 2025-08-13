#include "client/gui/screens/inventory/TextEditScreen.hpp"
#include <client/renderer/TileEntityRenderDispatcher.hpp>

TextEditScreen::TextEditScreen(std::shared_ptr<SignTileEntity> sign) : m_title("Edit sign message:"), m_frame(0), m_line(0), m_sign(sign), m_btnDone(Button(0, "Done"))
{
}

void TextEditScreen::init() {
    int x = m_width / 2 - 50;
    int y = m_height / 4 + 20;
    m_pMinecraft->m_pPlatform->showKeyboard(x / Gui::InvGuiScale, y / Gui::InvGuiScale, (x + 100) / Gui::InvGuiScale, (y + 100) / Gui::InvGuiScale);
    m_buttons.clear();
    m_btnDone.m_xPos = m_width / 2 - 100;
    m_btnDone.m_yPos = m_height / 4 + 120;
    m_buttons.push_back(&m_btnDone);
}

void TextEditScreen::render(int mouseX, int mouseY, float partialTicks)
{
    renderBackground();
    drawCenteredString(m_pFont, m_title, m_width / 2, 40, 0xFFFFFF);
    glPushMatrix();
    glTranslatef((float)(m_width / 2), 120, 50.0F);
    float ss = 93.75F;
    glScalef(-ss, -ss, -ss);
    glRotatef(180.0F, 0.0F, 1.0F, 0.0F);
    Tile* tile = m_sign->getTile();
    if (tile == Tile::sign) {
        float rot = (float)(m_sign->getData() * 360) / 16.0F;
        glRotatef(rot, 0.0F, 1.0F, 0.0F);
        glTranslatef(0.0F, 0.3125F, 0.0F);
    }
    else {
        int face = m_sign->getData();
        float rot = 0.0F;
        if (face == 2) {
            rot = 180.0F;
        }

        if (face == 4) {
            rot = 90.0F;
        }

        if (face == 5) {
            rot = -90.0F;
        }

        glRotatef(rot, 0.0F, 1.0F, 0.0F);
        glTranslatef(0.0F, 0.3125F, 0.0F);
    }

    if (m_frame / 6 % 2 == 0) {
        m_sign->m_selectedLine = m_line;
    }

    TileEntityRenderDispatcher::getInstance()->render(m_sign.get(), Vec3(-0.5, -0.75, -0.5), 0.0F);
    m_sign->m_selectedLine = -1;
    glPopMatrix();
    Screen::render(mouseX, mouseY, partialTicks);
}

void TextEditScreen::mouseClicked(int mouseX, int mouseY, int button)
{
    Screen::mouseClicked(mouseX, mouseY, button);
}

void TextEditScreen::keyPressed(int keyCode)
{
    Screen::keyPressed(keyCode);

    if (keyCode == AKEYCODE_ARROW_UP)
        m_line = m_line - 1 & 3;

    if (keyCode == AKEYCODE_ARROW_DOWN || keyCode == AKEYCODE_ENTER)
        m_line = m_line + 1 & 3;

    auto& line = m_sign->m_messages[m_line];
    if (keyCode == AKEYCODE_DEL && line.length() > 0)
        line.erase(line.end() - 1);
}

void TextEditScreen::keyboardNewChar(char chr)
{
    Screen::keyboardNewChar(chr);
    if (chr == '\n' || chr < ' ' || chr > '~')
        return;
    auto& line = m_sign->m_messages[m_line];
    if (line.length() < 15)
        line.insert(line.end(), chr);
}

void TextEditScreen::buttonClicked(Button* pButton)
{
    if (pButton->m_bEnabled && pButton->m_buttonId == 0)
    {
        m_sign->setChanged();
        m_pMinecraft->setScreen(nullptr);
    }
}

void TextEditScreen::tick()
{
    ++m_frame;
}

void TextEditScreen::removed()
{
    if (m_pMinecraft->m_pLevel->m_bIsOnline)
        m_pMinecraft->m_pRakNetInstance->send(m_sign->getUpdatePacket());
    m_pMinecraft->m_pPlatform->hideKeyboard();
}

