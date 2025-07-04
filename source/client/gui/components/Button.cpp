/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Button.hpp"

void Button::_init()
{
	m_width = 0;
	m_height = 0;
	m_xPos = 0;
	m_yPos = 0;
	m_text = "";
	m_bEnabled = true;
	m_bVisible = true;
	m_bHoveredOrFocused = false;

#ifndef ORIGINAL_CODE
	m_lastX = 0;
	m_lastY = 0;
#endif
}

Button::Button(int buttonId, int xPos, int yPos, int btnWidth, int btnHeight, const std::string& text)
{
	_init();

	m_buttonId = buttonId;
	m_xPos = xPos;
	m_yPos = yPos;
	m_text = text;
	m_width  = btnWidth;
	m_height = btnHeight;
}

Button::Button(int buttonId, int xPos, int yPos, const std::string& text)
{
	_init();

	m_buttonId = buttonId;
	m_xPos = xPos;
	m_yPos = yPos;
	m_text = text;
	m_width  = 200;
	m_height = 20;
}

Button::Button(int buttonId, const std::string& text)
{
	_init();

	m_buttonId = buttonId;
	m_text = text;
	m_width  = 200;
	m_height = 20;
}

bool Button::clicked(Minecraft* pMinecraft, int xPos, int yPos)
{
	if (!m_bEnabled) return false;
	if (xPos < m_xPos) return false;
	if (yPos < m_yPos) return false;
	if (xPos >= m_xPos + m_width) return false;
	if (yPos >= m_yPos + m_height) return false;

	return true;
}

int Button::getYImage(bool bHovered)
{
	if (!m_bEnabled) return 0;
	if (bHovered) return 2;
	return 1;
}

void Button::released(int xPos, int yPos)
{

}

void Button::renderBg(Minecraft*, int, int)
{

}

void Button::render(Minecraft* pMinecraft, int xPos, int yPos)
{
	if (!m_bVisible) return;

	if (!pMinecraft->useController())
		m_bHoveredOrFocused = clicked(pMinecraft, xPos, yPos);

	Font* pFont = pMinecraft->m_pFont;
	Textures* pTexs = pMinecraft->m_pTextures;

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	int v = getYImage(m_bHoveredOrFocused);
	pTexs->loadAndBindTexture("gui/gui.png");
	blit(m_xPos, m_yPos, 0, 46 + v * 20, m_width / 2, m_height, 256, 256);
	blit(m_xPos + m_width / 2, m_yPos, 200 - m_width / 2, 46 + v * 20, m_width / 2, m_height, 256, 256);

	renderBg(pMinecraft, xPos, yPos);

	int textColor;
	if (!m_bEnabled)
		textColor = int(0xFFA0A0A0U);
	else if (m_bHoveredOrFocused)
		textColor = int(0xFFFFA0U);
	else
		textColor = int(0xE0E0E0U);

	drawCenteredString(pFont, m_text, m_xPos + m_width / 2, m_yPos + (m_height - 8) / 2, textColor);
}
