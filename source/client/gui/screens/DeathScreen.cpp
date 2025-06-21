#include "DeathScreen.hpp"
#include <client/locale/Language.hpp>

DeathScreen::DeathScreen() :
	m_btnRespawn(1, Language::getInstance()->get("guiGameOver.respawn")),
	m_btnTitle(2, Language::getInstance()->get("guiGameOver.titleMenu"))
{
	m_tickCounter = 0;
}

void DeathScreen::init()
{
	m_btnRespawn.m_xPos = m_btnTitle.m_xPos = m_width / 2 - (m_btnRespawn.m_width = m_btnTitle.m_width = 200) / 2;

	m_btnRespawn.m_yPos = m_height / 4 + 72;
	m_btnTitle.m_yPos = m_height / 4 + 96;

	m_buttons.push_back(&m_btnRespawn);
	m_buttons.push_back(&m_btnTitle);
	m_buttonTabList.push_back(&m_btnRespawn);
	m_buttonTabList.push_back(&m_btnTitle);
}

void DeathScreen::buttonClicked(Button* pButton)
{
	if (pButton->m_buttonId == m_btnRespawn.m_buttonId)
	{
		m_pMinecraft->m_pLocalPlayer->respawn();
		m_pMinecraft->setScreen(nullptr);
	}
	if (pButton->m_buttonId == m_btnTitle.m_buttonId)
	{
		m_pMinecraft->leaveGame(false);
	}
}

void DeathScreen::tick()
{
	m_tickCounter++;
}

void DeathScreen::keyPressed(int key)
{
}

void DeathScreen::render(int x, int y, float f)
{
	fillGradient(0, 0, m_width, m_height, 0x60500000, 0xA0803030);

	glPushMatrix();
	glScalef(2.0f, 2.0f, 2.0f);
	drawCenteredString(m_pFont, Language::getInstance()->get("guiGameOver.death"), m_width / 4, m_height / 8, 0xFFFFFF);
	glPopMatrix();

	Screen::render(x, y, f);
}

bool DeathScreen::isPauseScreen()
{
	return false;
}
