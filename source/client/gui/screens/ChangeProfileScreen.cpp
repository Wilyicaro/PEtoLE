#include "ChangeProfileScreen.hpp"
#include "TitleScreen.hpp"

ChangeProfileScreen::ChangeProfileScreen(const std::string& initialName) :
	m_nameBox(this, 1, 0, 0, 200, 20, "", initialName),
	m_btnDone(2, "Done")
{}

void ChangeProfileScreen::init()
{
	m_nameBox.m_width = 200;
	m_nameBox.m_height = 20;

	m_btnDone.m_width = 200;
	m_btnDone.m_height = 20;

	m_nameBox.m_xPos = (m_width / 2) - (m_nameBox.m_width / 2);
	m_nameBox.m_yPos = m_height / 2 - 20;

	m_btnDone.m_xPos = (m_width / 2) - (m_btnDone.m_width / 2);
	m_btnDone.m_yPos = m_height - 55;
	m_btnDone.m_bEnabled = false;

	m_textInputs.push_back(&m_nameBox);
	m_buttons.push_back(&m_btnDone);

	for (int i = 0; i < int(m_buttons.size()); i++)
		m_buttonTabList.push_back(m_buttons[i]);

	m_nameBox.init(m_pFont);
}

void ChangeProfileScreen::render(int x, int y, float f)
{
	renderBackground();
	Screen::render(x, y, f);

	drawCenteredString(m_pFont, "Change Profile", m_width / 2, 30, 0xFFFFFF);
	drawString(m_pFont, "Username", m_nameBox.m_xPos, m_nameBox.m_yPos - 15, 0x999999);
}

void ChangeProfileScreen::buttonClicked(Button* pButton)
{
	if (pButton->m_buttonId == m_btnDone.m_buttonId)
	{
		if (m_nameBox.getText() == m_pMinecraft->getOptions()->m_playerName.get())
			return;

		m_pMinecraft->getOptions()->m_playerName.set(m_nameBox.getText());
		m_pMinecraft->m_pUser->m_username = m_nameBox.getText();
		m_pMinecraft->initStatsCounter();
		m_pMinecraft->getOptions()->save();
		m_pMinecraft->setScreen(new TitleScreen);
	}
}

void ChangeProfileScreen::onTextBoxUpdated(int id)
{
	if (id == m_nameBox.getKey())
	{
		if (m_nameBox.getText() == m_pMinecraft->getOptions()->m_playerName.get())
			m_btnDone.m_bEnabled = false;
		else
			m_btnDone.m_bEnabled = true;
	}
}
