/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "OptionsScreen.hpp"
#include "StartMenuScreen.hpp"
#include "PauseScreen.hpp"
#include "VideoSettingsScreen.hpp"

OptionsScreen::OptionsScreen(Options* o)
	: m_BackButton(1, 0, 0, 200, 20, "Done"), m_videoButton(2, 0, 0, 200, 20, "Video Settings..."), m_title("Options")
{
	Options::OptionEntry* entries[] = { &o->m_fMusicVolume, &o->m_fMasterVolume, &o->m_bInvertMouse, &o->m_fSensitivity, &o->m_difficulty, &o->m_moreWorldOptions };
	for (auto e : entries)
		e->addButton(m_configButtons, o->getMessage(*e));
}

OptionsScreen::~OptionsScreen()
{
	for (auto b : m_configButtons)
		delete b;
}

void OptionsScreen::init()
{
	m_pMinecraft->platform()->showDialog(AppPlatform::DLG_OPTIONS);
	m_pMinecraft->platform()->createUserInput();
	m_title = Language::getInstance()->get("options.title");
	m_BackButton.m_text = Language::getInstance()->get("gui.done");
	m_BackButton.m_xPos = m_videoButton.m_xPos = m_width / 2 - m_BackButton.m_width / 2;
	m_BackButton.m_yPos = m_height / 6 + 168;
	m_videoButton.m_text = Language::getInstance()->get("options.video");
	m_videoButton.m_yPos = m_height / 6 + 108;
	m_buttons.push_back(&m_BackButton);
	m_buttons.push_back(&m_videoButton);

	Options* o = m_pMinecraft->getOptions();


	for (int i = 0; i < m_configButtons.size(); i++)
	{
		m_buttons.push_back(m_configButtons[i]);
		m_buttonTabList.push_back(m_configButtons[i]);
		m_configButtons[i]->m_xPos = m_width / 2 - 155 + i % 2 * 160;
		m_configButtons[i]->m_yPos = m_height / 6 + 24 * (i >> 1);
	}

	m_buttonTabList.push_back(&m_BackButton);
}

void OptionsScreen::render(int a, int b, float c)
{
	renderBackground();

	if (m_pMinecraft->m_pPlatform->getUserInputStatus() >= 0)
	{
		m_pMinecraft->setScreen(new StartMenuScreen);
	}

	drawCenteredString(m_pFont, m_title, m_width / 2, 20, 0xFFFFFF);

	Screen::render(a, b, c);
}

void OptionsScreen::buttonClicked(Button* pButton)
{
	if (pButton->m_buttonId == 1)
	{
		if (m_pMinecraft->isLevelReady())
			m_pMinecraft->setScreen(new PauseScreen);
		else
			m_pMinecraft->setScreen(new StartMenuScreen);
	}
	else if (pButton->m_buttonId == 2)
	{
		m_pMinecraft->setScreen(new VideoSettingsScreen(m_pMinecraft->getOptions()));
	}

}
