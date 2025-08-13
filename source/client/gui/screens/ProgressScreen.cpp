/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ProgressScreen.hpp"

bool ProgressScreen::isInGameScreen()
{
	return false;
}

void ProgressScreen::init(Minecraft* minecraft, int w, int h)
{
	Screen::init(minecraft, w, h);
	minecraft->m_progress.progressStart(Util::EMPTY_STRING);
	minecraft->m_progress.progressStage(Util::EMPTY_STRING);
	minecraft->m_progress.progressStagePercentage(0);
}

void ProgressScreen::render(int a, int b, float c)
{
	renderDirtBackground(0);

	m_pMinecraft->m_pTextures->loadAndBindTexture("gui/background.png");

	//! why not use the screen stuff

	int yPos = m_height / 2;

	if (m_pMinecraft->m_progress.m_percentage >= 0)
	{
		float lX = float(m_width) / 2 - 50, rX = float(m_width) / 2 + 50;
		float prog = float(m_pMinecraft->m_progress.m_percentage);

		// gray background
		fill(lX, yPos + 16, rX, yPos + 18, 0xFF808080);
		// the green stuff
		fill(lX, yPos + 16, lX + prog, yPos + 18, 0xFF80FF80);
	}

	int width = m_pFont->width(m_pMinecraft->m_progress.m_title);
	m_pFont->drawShadow(m_pMinecraft->m_progress.m_title, (m_width - width) / 2, yPos - 20, 0xFFFFFF);

	width = m_pFont->width(m_pMinecraft->m_progress.m_status);
	m_pFont->drawShadow(m_pMinecraft->m_progress.m_status, (m_width - width) / 2, yPos + 4, 0xFFFFFF);

#ifdef ORIGINAL_CODE
	sleepMs(50);
#endif
}

void ProgressScreen::updateEvents()
{
	if (m_pMinecraft->m_async.empty())
	{
		m_pMinecraft->setScreen(nullptr);
		return;
	}



	Screen::updateEvents();
}
