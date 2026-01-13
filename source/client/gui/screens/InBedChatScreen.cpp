#include "InBedChatScreen.hpp"

InBedChatScreen::InBedChatScreen() : ChatScreen(false), m_btnWake(1, 0, 0, "Wake")
{
}

void InBedChatScreen::init()
{
	ChatScreen::init();
	m_btnWake.m_text = Language::getInstance()->get("multiplayer.stopSleeping");
	m_btnWake.m_xPos = m_width / 2 - 100;
	m_btnWake.m_yPos = m_height - 40;

	m_buttons.push_back(&m_btnWake);
}

void InBedChatScreen::buttonClicked(Button* button)
{
	ChatScreen::buttonClicked(button);

	if (button->m_buttonId == m_btnWake.m_buttonId)
		sendWakeUp();
}

void InBedChatScreen::onClose()
{
	ChatScreen::onClose();
	sendWakeUp();
}

void InBedChatScreen::tick()
{
	if (!m_pMinecraft->m_pPlayer->isSleeping())
		m_pMinecraft->setScreen(nullptr);
}

void InBedChatScreen::sendWakeUp()
{
	if (m_pMinecraft->isOnlineClient())
		m_pMinecraft->m_pRakNetInstance->send(new PlayerCommandPacket(m_pMinecraft->m_pPlayer->m_EntityID, 3));
	else
		m_pMinecraft->m_pPlayer->stopSleepInBed(false, true, true);
}
