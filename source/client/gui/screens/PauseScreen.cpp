/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "PauseScreen.hpp"
#include "OptionsScreen.hpp"
#include "client/locale/Language.hpp"
#include "AchievementsScreen.hpp"
#include "StatsScreen.hpp"

PauseScreen::PauseScreen() :
	field_40(0),
	m_btnBack(1, Language::getInstance()->get("inGameMenu.backToGame")),
	m_btnQuit(2, ""),
	m_btnAchievements(5, 0, 0, 98, 20, ""),
	m_btnStats(6, 0, 0, 98, 20, ""),
	m_btnOptions(999, Language::getInstance()->get("menu.options"))
{
}

void PauseScreen::init()
{	
	int currY = m_height / 4, inc = 24;
	m_btnQuit.m_text = Language::getInstance()->get(m_pMinecraft->isOnlineClient() ? "inGameMenu.disconnect" : "inGameMenu.saveAndQuitToTitle");
	m_btnBack.m_yPos = currY + 8;
	m_btnQuit.m_yPos = currY + 104;
	m_btnBack.m_xPos = (m_width - 200) / 2;
	m_btnQuit.m_xPos = (m_width - 200) / 2;
	m_btnAchievements.m_text = Language::getInstance()->get("gui.achievements");
	m_btnStats.m_text = Language::getInstance()->get("gui.stats");
	m_btnAchievements.m_xPos = (m_width - 200) / 2;
	m_btnStats.m_xPos = m_width / 2 + 2;

	m_btnStats.m_yPos = m_btnAchievements.m_yPos = currY + 32;

	m_btnOptions.m_yPos = currY + 80;
	m_btnOptions.m_xPos = m_btnBack.m_xPos;

	// add the buttons to the screen:
	m_buttons.push_back(&m_btnBack);

	m_buttons.push_back(&m_btnOptions);
	m_buttons.push_back(&m_btnAchievements);
	m_buttons.push_back(&m_btnStats);
	m_buttons.push_back(&m_btnQuit);

	for (int i = 0; i < int(m_buttons.size()); i++)
		m_buttonTabList.push_back(m_buttons[i]);

#ifdef __EMSCRIPTEN__
	m_btnVisible.m_bEnabled = false;
#endif
}

void PauseScreen::tick()
{
	field_40++;
}

void PauseScreen::render(int a, int b, float c)
{
	renderBackground();

	drawCenteredString(m_pFont, Language::getInstance()->get("inGameMenu.gameMenu"), m_width / 2, 24, 0xFFFFFF);
	Screen::render(a, b, c);
}

void PauseScreen::buttonClicked(Button* pButton)
{
	if (pButton->m_buttonId == m_btnBack.m_buttonId)
		m_pMinecraft->resumeGame();

	if (pButton->m_buttonId == m_btnQuit.m_buttonId)
		m_pMinecraft->leaveGame();

	if (pButton->m_buttonId == m_btnAchievements.m_buttonId)
	{
		m_pMinecraft->setScreen(new AchievementsScreen(m_pMinecraft->m_pStatsCounter));
	}

	if (pButton->m_buttonId == m_btnStats.m_buttonId)
	{
		m_pMinecraft->setScreen(new StatsScreen(this, m_pMinecraft->m_pStatsCounter));
	}

#ifdef ENH_ADD_OPTIONS_PAUSE
	if (pButton->m_buttonId == m_btnOptions.m_buttonId)
		m_pMinecraft->setScreen(new OptionsScreen(m_pMinecraft->getOptions()));
#endif
}
