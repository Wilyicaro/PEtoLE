/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "VideoSettingsScreen.hpp"
#include "OptionsScreen.hpp"

VideoSettingsScreen::VideoSettingsScreen(Options* o)
	: m_BackButton(1, 0, 0, 200, 20, "Done"), m_title("Video Settings")
{
	Options::OptionEntry* entries[] = { &o->m_bFancyGraphics, &o->m_iViewDistance, &o->m_bAmbientOcclusion, &o->m_limitFramerate, &o->m_bAnaglyphs, &o->m_bViewBobbing, &o->m_guiScale, &o->m_bMipmaps, &o->m_bBlockOutlines, &o->m_bDynamicHand, &o->m_bMenuPanorama, &o->m_vSync };
	for (auto e : entries)
	{
		bool mipmaps = e->getKey() == o->m_bMipmaps.getKey(), panorama = e->getKey() == o->m_bMenuPanorama.getKey();
		if ((!mipmaps && !panorama) || (mipmaps && o->m_pMinecraft->m_pTextures->hasTerrainMipmap()) || (panorama && isMenuPanoramaAvailable()))
			e->addButton(m_configButtons, o->getMessage(*e));
	}
}

VideoSettingsScreen::~VideoSettingsScreen()
{
	for (auto b : m_configButtons)
		delete b;
}

void VideoSettingsScreen::init()
{
	m_title = Language::getInstance()->get("options.videoTitle");
	m_BackButton.m_text = Language::getInstance()->get("gui.done");
	m_BackButton.m_xPos = m_width / 2 - m_BackButton.m_width / 2;
	m_BackButton.m_yPos = m_height / 6 + 168;
	m_buttons.push_back(&m_BackButton);

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

void VideoSettingsScreen::render(int a, int b, float c)
{
	renderBackground();
	drawCenteredString(m_pFont, m_title, m_width / 2, 20, 0xFFFFFF);

	Screen::render(a, b, c);
}

void VideoSettingsScreen::buttonClicked(Button* pButton)
{
	if (pButton->m_buttonId == 1)
		m_pMinecraft->setScreen(new OptionsScreen(m_pMinecraft->getOptions()));
}
