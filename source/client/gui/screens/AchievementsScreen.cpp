#include "AchievementsScreen.hpp"
#include "stats/Achievement.hpp"
#include "client/renderer/Lighting.hpp"
#include "client/renderer/entity/ItemRenderer.hpp"


AchievementsScreen::AchievementsScreen(StatsCounter* statsCounter) : 
    m_pStatsCounter(statsCounter),
    m_doneBtn(1, 0, 0, 80, 20, Language::getInstance()->get("gui.done")),
    minX(Achievements::minX * 24 - 112),
    minY(Achievements::minY * 24 - 112),
    maxX(Achievements::maxX * 24 - 77),
    maxY(Achievements::maxY * 24 - 77)
{
    short offsetX = 141;
    short offsetY = 141;

    m_targetScrollX = m_scrollX = m_prevScrollX = (double)(Achievements::openInventory->m_x * 24 - offsetX / 2 - 12);
    m_targetScrollY = m_scrollY = m_prevScrollY = (double)(Achievements::openInventory->m_y * 24 - offsetY / 2);
}

void AchievementsScreen::init()
{
    Screen::init();
    m_doneBtn.m_xPos = m_width / 2 + 24;
    m_doneBtn.m_yPos = m_height / 2 + 74;
    m_buttons.push_back(&m_doneBtn);
}

void AchievementsScreen::buttonClicked(Button* button)
{
    if (button->m_buttonId == 1)
    {
        m_pMinecraft->setScreen(nullptr);
        m_pMinecraft->grabMouse();
    }

    Screen::buttonClicked(button);
}

void AchievementsScreen::keyPressed(int eventKey)
{
    if (m_pMinecraft->getOptions()->isKey(KM_INVENTORY, eventKey))
    {
        m_pMinecraft->grabMouse();
        m_pMinecraft->setScreen(nullptr);
    }
    else
    {
        Screen::keyPressed(eventKey);
    }
}

void AchievementsScreen::render(int mouseX, int mouseY, float partialTicks)
{
    if (Mouse::isButtonDown(BUTTON_LEFT))
    {
        int guiX = (m_width - m_imageWidth) / 2;
        int guiY = (m_height - m_imageHeight) / 2;
        int dragX = guiX + 8;
        int dragY = guiY + 17;

        if ((m_mouseButtonState == 0 || m_mouseButtonState == 1) &&
            mouseX >= dragX && mouseX < dragX + 224 &&
            mouseY >= dragY && mouseY < dragY + 155)
        {
            if (m_mouseButtonState == 0)
            {
                m_mouseButtonState = 1;
            }
            else
            {
                m_scrollX -= (double)(mouseX - m_mouseX);
                m_scrollY -= (double)(mouseY - m_mouseY);
                m_targetScrollX = m_prevScrollX = m_scrollX;
                m_targetScrollY = m_prevScrollY = m_scrollY;
            }

            m_mouseX = mouseX;
            m_mouseY = mouseY;
        }

        if (m_targetScrollX < (double)minX) m_targetScrollX = (double)minX;
        if (m_targetScrollY < (double)minY) m_targetScrollY = (double)minY;
        if (m_targetScrollX >= (double)maxX) m_targetScrollX = (double)(maxX - 1);
        if (m_targetScrollY >= (double)maxY) m_targetScrollY = (double)(maxY - 1);
    }
    else
    {
        m_mouseButtonState = 0;
    }

    renderBackground();

    renderInside(mouseX, mouseY, partialTicks);

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    renderTitle();
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}

void AchievementsScreen::tick()
{
    m_prevScrollX = m_scrollX;
    m_prevScrollY = m_scrollY;

    double deltaX = m_targetScrollX - m_scrollX;
    double deltaY = m_targetScrollY - m_scrollY;

    if (deltaX * deltaX + deltaY * deltaY < 4.0)
    {
        m_scrollX += deltaX;
        m_scrollY += deltaY;
    }
    else
    {
        m_scrollX += deltaX * 0.85;
        m_scrollY += deltaY * 0.85;
    }
}

void AchievementsScreen::renderTitle()
{
    int guiX = (m_width - m_imageWidth) / 2;
    int guiY = (m_height - m_imageHeight) / 2;
    m_pFont->draw("Achievements", guiX + 15, guiY + 5, 0x404040);
}

void AchievementsScreen::renderInside(int mouseX, int mouseY, float partialTicks)
{
    int scrollX = Mth::floor(m_prevScrollX + (m_scrollX - m_prevScrollX) * partialTicks);
    int scrollY = Mth::floor(m_prevScrollY + (m_scrollY - m_prevScrollY) * partialTicks);

    if (scrollX < minX) scrollX = minX;
    if (scrollY < minY) scrollY = minY;
    if (scrollX >= maxX) scrollX = maxX - 1;
    if (scrollY >= maxY) scrollY = maxY - 1;

    int backgroundTexture = m_pMinecraft->m_pTextures->loadTexture("achievement/bg.png");

    int guiX = (m_width - m_imageWidth) / 2;
    int guiY = (m_height - m_imageHeight) / 2;

    int drawX = guiX + 16;
    int drawY = guiY + 17;

    zLevel = 0.0F;

    glDepthFunc(GL_GEQUAL);
    glPushMatrix();
    glTranslatef(0.0F, 0.0F, -200.0F);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glEnable(GL_RESCALE_NORMAL);
    glEnable(GL_COLOR_MATERIAL);

    m_pMinecraft->m_pTextures->loadAndBindTexture(C_TERRAIN_NAME);

    int tileU = (scrollX + 288) >> 4;
    int tileV = (scrollY + 288) >> 4;
    int offsetU = (scrollX + 288) % 16;
    int offsetV = (scrollY + 288) % 16;

    Random random;

    for (int row = 0; row * 16 - offsetV < 155; ++row)
    {
        Color color = Color(Color::WHITE).mulRGB(0.6F - (tileV + row) / 25.0F * 0.3F);

        for (int col = 0; col * 16 - offsetU < 224; ++col)
        {
            random.setSeed(1234 + tileU + col);
            random.nextInt();

            int noise = random.nextInt(1 + tileV + row) + (tileV + row) / 2;
            int texture = Tile::sand->m_TextureFrame;

            if (noise <= 37 && tileV + row != 35)
            {
                if (noise == 22)
                {
                    texture = random.nextInt(2) == 0 ? Tile::diamondOre->m_TextureFrame : Tile::redstoneOre->m_TextureFrame;
                }
                else if (noise == 10)
                {
                    texture = Tile::ironOre->m_TextureFrame;
                }
                else if (noise == 8)
                {
                    texture = Tile::coalOre->m_TextureFrame;
                }
                else if (noise > 4)
                {
                    texture = Tile::stone->m_TextureFrame;
                }
                else if (noise > 0)
                {
                    texture = Tile::dirt->m_TextureFrame;
                }
            }
            else
            {
                texture = Tile::bedrock->m_TextureFrame;
            }

            blit(drawX + col * 16 - offsetU, drawY + row * 16 - offsetV,
                 texture % 16 << 4, texture >> 4 << 4, 16, 16, 256, 256, color);
        }
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_TEXTURE_2D);

    for (size_t i = 0; i < Achievements::achievements.size(); ++i)
    {
        Achievement* achievement = Achievements::achievements[i];
        if (achievement->m_pParent)
        {
            int x1 = achievement->m_x * 24 - scrollX + 11 + drawX;
            int y1 = achievement->m_y * 24 - scrollY + 11 + drawY;
            int x2 = achievement->m_pParent->m_x * 24 - scrollX + 11 + drawX;
            int y2 = achievement->m_pParent->m_y * 24 - scrollY + 11 + drawY;

            bool completed = m_pStatsCounter->hasAchievement(achievement);
            bool canUnlock = m_pStatsCounter->canUnlockAchievement(achievement);

            int alpha = (Mth::sin((getMillis() % 600) / 600.0 * M_PI * 2.0) > 0.6 ? 255 : 130);

            int color;
            if (completed)
                color = 0xFF5C5C5C;
            else if (canUnlock)
                color = 0x00FF00 + (alpha << 24);
            else
                color = 0xFF000000;

            hLine(x1, x2, y1, color);
            vLine(x2, y1, y2, color);
        }
    }

    Achievement* hovered = nullptr;

    glPushMatrix();
    glRotatef(180.0F, 1.0F, 0.0F, 0.0F);
    Lighting::turnOn();
    glPopMatrix();

    glDisable(GL_LIGHTING);
    glEnable(GL_RESCALE_NORMAL);
    glEnable(GL_COLOR_MATERIAL);
    glColor4f(1.0F, 1.0F, 1.0F, 1.0F);

    for (size_t i = 0; i < Achievements::achievements.size(); ++i)
    {
        Achievement* achievement = Achievements::achievements[i];
        int x = achievement->m_x * 24 - scrollX;
        int y = achievement->m_y * 24 - scrollY;

        if (x >= -24 && y >= -24 && x <= 224 && y <= 155)
        {
            Color color;
            if (m_pStatsCounter->hasAchievement(achievement))
                color = Color::WHITE;
            else if (m_pStatsCounter->canUnlockAchievement(achievement))
                color = Color(Color::WHITE).mulRGB(Mth::sin((getMillis() % 600) / 600.0 * M_PI * 2.0) < 0.6 ? 0.6F : 0.8F);
            else
                color = Color(Color::WHITE).mulRGB(0.3f);

            m_pMinecraft->m_pTextures->bind(backgroundTexture);

            int drawX = guiX + 16 + x;
            int drawY = guiY + 17 + y;

            if (achievement->isChallenge())
                blit(drawX - 2, drawY - 2, 26, 202, 26, 26, 256, 256, color);
            else
                blit(drawX - 2, drawY - 2, 0, 202, 26, 26, 256, 256, color);

            if (!m_pStatsCounter->canUnlockAchievement(achievement))
            {
                color = Color(Color::WHITE).mulRGB(0.1f);
            }

            glEnable(GL_LIGHTING);
            glEnable(GL_CULL_FACE);

            ItemRenderer::renderGuiItem(m_pMinecraft->m_pTextures, achievement->m_icon, drawX + 3, drawY + 3, color);

            glDisable(GL_LIGHTING);

            if (mouseX >= guiX + 16 && mouseY >= guiY + 17 &&
                mouseX < guiX + 16 + 224 && mouseY < guiY + 17 + 155 &&
                mouseX >= drawX && mouseX <= drawX + 22 &&
                mouseY >= drawY && mouseY <= drawY + 22)
            {
                hovered = achievement;
            }
        }
    }

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    m_pMinecraft->m_pTextures->bind(backgroundTexture);
    blit(guiX, guiY, 0, 0, m_imageWidth, m_imageHeight, 256, 256);

    glPopMatrix();
    zLevel = 0.0F;
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    Screen::render(mouseX, mouseY, partialTicks);

    if (hovered)
    {
        std::string title = hovered->getName();
        std::string desc = hovered->getDescription();

        int tooltipX = mouseX + 12;
        int tooltipY = mouseY - 4;

        if (m_pStatsCounter->canUnlockAchievement(hovered))
        {
            int width = Mth::max(m_pFont->width(title), 120);
            std::vector<std::string> lines = m_pFont->split(desc, width);
            int height = lines.size() * 8;

            if (m_pStatsCounter->hasAchievement(hovered))
            {
                height += 12;
            }

            //Fill gradient was used here, but for some reason the colors were equal
            fill(tooltipX - 3, tooltipY - 3, tooltipX + width + 3, tooltipY + height + 3 + 12, 0x80000000);

            m_pFont->drawWordWrap(lines, tooltipX, tooltipY + 12, 0xA0A0A0);

            if (m_pStatsCounter->hasAchievement(hovered))
            {
                m_pFont->drawShadow(Language::getInstance()->get("achievement.taken"), tooltipX, tooltipY + height + 4, 0x9090FF);
            }
        }
        else
        {
            int width = Mth::max(m_pFont->width(title), 120);
            std::string requiresStr = Language::getInstance()->get("achievement.requires", hovered->m_pParent->getName());
            std::vector<std::string> lines = m_pFont->split(requiresStr, width);
            int height = lines.size() * 8;

            fill(tooltipX - 3, tooltipY - 3, tooltipX + width + 3, tooltipY + height + 12 + 3, 0x80000000);

            m_pFont->drawWordWrap(lines, tooltipX, tooltipY + 12, 0x705050);
        }

        m_pFont->drawShadow(title, tooltipX, tooltipY, m_pStatsCounter->canUnlockAchievement(hovered)
            ? (hovered->isChallenge() ? 0xFFFF80 : 0xFFFFFF)
            : (hovered->isChallenge() ? 0x808040 : 0x808080));
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    Lighting::turnOff();
}

bool AchievementsScreen::isPauseScreen()
{
    return true;
}