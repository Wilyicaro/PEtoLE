#include "EditWorldScreen.hpp"

EditWorldScreen::EditWorldScreen(Screen* parent, const LevelSummary& summary) :
	m_nameBox(this, 1, 0, 0, 200, 20, "", summary.m_levelName),
	m_btnDone(2, "Rename"),
	m_btnCancel(3, "Cancel"),
	m_pParent(parent),
	m_summary(summary)
{
	m_bDeletePrevious = false;
}

void EditWorldScreen::init()
{
	m_nameBox.m_xPos = m_btnDone.m_xPos = m_btnCancel.m_xPos = (m_width / 2) - (200 / 2);
	m_nameBox.m_yPos = 60;

	m_btnDone.m_text = Language::getInstance()->get("selectWorld.renameButton");
	m_btnDone.m_yPos = m_height / 4 + 108;
	m_btnCancel.m_text = Language::getInstance()->get("gui.cancel");
	m_btnCancel.m_yPos = m_height / 4 + 132;

	m_textInputs.push_back(&m_nameBox);
	m_buttons.push_back(&m_btnDone);
	m_buttons.push_back(&m_btnCancel);

	for (int i = 0; i < int(m_buttons.size()); i++)
		m_buttonTabList.push_back(m_buttons[i]);

	m_nameBox.init(m_pFont);
}

void EditWorldScreen::render(int x, int y, float f)
{
	renderBackground();
	Screen::render(x, y, f);

	drawCenteredString(m_pFont, Language::getInstance()->get("selectWorld.renameTitle"), m_width / 2, 30, 0xFFFFFF);
	drawString(m_pFont, Language::getInstance()->get("selectWorld.enterName"), m_nameBox.m_xPos, m_nameBox.m_yPos - 13, 0x999999);
}

void EditWorldScreen::onClose()
{
	m_pMinecraft->setScreen(m_pParent);
}

void EditWorldScreen::buttonClicked(Button* pButton)
{
	if (pButton->m_buttonId == m_btnDone.m_buttonId)
	{
		m_pMinecraft->getLevelSource()->renameLevel(m_summary.m_fileName, Util::stringTrim(m_nameBox.getText()));
		onClose();
	}

	if (pButton->m_buttonId == m_btnCancel.m_buttonId)
	{
		onClose();
	}
}
