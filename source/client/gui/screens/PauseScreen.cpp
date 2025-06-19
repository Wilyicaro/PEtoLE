/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "PauseScreen.hpp"
#include "OptionsScreen.hpp"
#include "network/ServerSideNetworkHandler.hpp"
#include "client/locale/Language.hpp"

PauseScreen::PauseScreen() :
	field_40(0),
	m_btnBack(1, Language::getInstance()->get("inGameMenu.backToGame")),
	m_btnQuit(2, ""),
	m_btnVisible(4, ""),
	m_btnOptions(999, Language::getInstance()->get("menu.options"))
{
}

void PauseScreen::init()
{
	bool bAddVisibleButton = m_pMinecraft->m_pRakNetInstance && m_pMinecraft->m_pRakNetInstance->m_bIsHost;
	
	int nButtons = 2;

	if (bAddVisibleButton)
		nButtons++;

#ifdef ENH_ADD_OPTIONS_PAUSE
	nButtons++;
#endif

	int currY = m_height / 4, inc = 24;
	m_btnQuit.m_text = Language::getInstance()->get(m_pMinecraft->isOnlineClient() ? "inGameMenu.disconnect" : "inGameMenu.saveAndQuitToTitle");
	m_btnBack.m_yPos = currY + 8;
	m_btnQuit.m_yPos = currY + 104;
	m_btnBack.m_xPos = (m_width - 200) / 2;
	m_btnQuit.m_xPos = (m_width - 200) / 2;
	m_btnVisible.m_xPos = (m_width - 200) / 2;

	m_btnVisible.m_yPos = currY + 32;

	m_btnOptions.m_yPos = currY + 80;
	m_btnOptions.m_xPos = m_btnBack.m_xPos;

	// add the buttons to the screen:
	m_buttons.push_back(&m_btnBack);

	m_buttons.push_back(&m_btnOptions);

	if (bAddVisibleButton)
	{
		updateServerVisibilityText();
		m_buttons.push_back(&m_btnVisible);
	}

	m_buttons.push_back(&m_btnQuit);

	for (int i = 0; i < int(m_buttons.size()); i++)
		m_buttonTabList.push_back(m_buttons[i]);

#ifdef __EMSCRIPTEN__
	m_btnVisible.m_bEnabled = false;
#endif
}

void PauseScreen::updateServerVisibilityText()
{
	if (!m_pMinecraft->m_pRakNetInstance) return;
	if (!m_pMinecraft->m_pRakNetInstance->m_bIsHost) return;

	ServerSideNetworkHandler* pSSNH = (ServerSideNetworkHandler*)m_pMinecraft->m_pNetEventCallback;

	if (pSSNH->m_bAllowIncoming)
		m_btnVisible.m_text = "Server is visible";
	else
		m_btnVisible.m_text = "Server is invisible";
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
		m_pMinecraft->leaveGame(false);

	if (pButton->m_buttonId == m_btnVisible.m_buttonId)
	{
		if (m_pMinecraft->m_pRakNetInstance && m_pMinecraft->m_pRakNetInstance->m_bIsHost)
		{
			ServerSideNetworkHandler* pSSNH = (ServerSideNetworkHandler*)m_pMinecraft->m_pNetEventCallback;
			pSSNH->allowIncomingConnections(!pSSNH->m_bAllowIncoming);

			updateServerVisibilityText();
		}
	}

#ifdef ENH_ADD_OPTIONS_PAUSE
	if (pButton->m_buttonId == m_btnOptions.m_buttonId)
		m_pMinecraft->setScreen(new OptionsScreen);
#endif
}
