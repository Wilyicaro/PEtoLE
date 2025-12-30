/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ChatScreen.hpp"

// @NOTE: This is unused.

ChatScreen::ChatScreen(bool slash) : m_textChat(this, 1, 0, 0), m_btnSend(2, 0, 0, "Send")
{
	if (slash)
		m_textChat.setText("/");
	m_textChat.setBordered(false);
}

void ChatScreen::buttonClicked(Button* pButton)
{
	if (pButton->m_buttonId == m_btnSend.m_buttonId)
		sendMessageAndExit();
}

void ChatScreen::init()
{
	bool touch = m_pMinecraft->isTouchscreen();
	m_btnSend.m_height = 20;
	m_btnSend.m_width = 40;
	m_textChat.m_xPos = 9;
	m_textChat.m_yPos = m_height - 12;
	m_textChat.m_width = m_width - 9 + (touch ? -m_btnSend.m_width : 0);
	m_textChat.m_height = 8;

	m_btnSend.m_yPos = m_height - 20;
	m_btnSend.m_xPos = m_textChat.m_xPos + m_textChat.m_width;
	
	// set focus directly on the chat text box
	m_textChat.init(m_pFont);
	m_textChat.setFocused(true);

	if (touch)
		m_buttons.push_back(&m_btnSend);
	m_textInputs.push_back(&m_textChat);
}

void ChatScreen::removed()
{
	// Now let them be rendered.
	m_pMinecraft->m_gui.m_bRenderMessages = true;
}

void ChatScreen::render(int mouseX, int mouseY, float f)
{
	// override the default behavior of rendering chat messages
	m_pMinecraft->m_gui.m_bRenderMessages = false;
	m_pMinecraft->m_gui.renderMessages(true);

	fill(2, m_height - 14, m_width - 2, m_height - 2, 0x80000001);
	drawString(m_pFont, ">", 4, m_height - 12, 14737632);

	Screen::render(mouseX, mouseY, f);
}

void ChatScreen::keyPressed(int keyCode)
{
	if (m_pMinecraft->getOptions()->isKey(KM_MENU_OK, keyCode))
		sendMessageAndExit();

	Screen::keyPressed(keyCode);
}

void ChatScreen::sendMessageAndExit()
{
	m_pMinecraft->sendMessage(m_textChat.getText());
	
	m_pMinecraft->setScreen(nullptr);
}

bool ChatScreen::isPauseScreen() {
	return false;
}
