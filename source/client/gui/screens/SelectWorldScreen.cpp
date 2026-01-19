/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SelectWorldScreen.hpp"
#include "DeleteWorldScreen.hpp"
#include "EditWorldScreen.hpp"
#include "CreateWorldScreen.hpp"
#include "TitleScreen.hpp"
#include "common/Util.hpp"
#include <set>

SelectWorldScreen::SelectWorldScreen() :
	m_btnDelete(1, 0, 0, 70, 20, "Delete"),
	m_btnCreateNew(2, 0, 0, 150, 20, "Create new"),
	m_btnCancel(3, 0, 0, 150, 20, "Back"),
	m_btnSelect(4, 0, 0, 150, 20, "Select"),
	m_btnRename(6, 0, 0, 70, 20, "Rename"),
	m_pWorldSelectionList(nullptr),
	m_selectedIndex(-1)
{
	m_btnSelect.m_bEnabled = false;
	m_btnDelete.m_bEnabled = false;
	m_btnRename.m_bEnabled = false;
}

SelectWorldScreen::~SelectWorldScreen()
{
	SAFE_DELETE(m_pWorldSelectionList);
}

void SelectWorldScreen::init()
{
	SAFE_DELETE(m_pWorldSelectionList);

	m_title = Language::getInstance()->get("selectWorld.title");
	m_pWorldSelectionList = new WorldSelectionList(this);
	loadLevelSource();

	m_btnSelect.m_yPos = m_btnCreateNew.m_yPos = m_height - 52;
	m_btnRename.m_yPos = m_btnDelete.m_yPos = m_btnCancel.m_yPos = m_height - 28;

	m_btnSelect.m_xPos = m_btnRename.m_xPos = m_width / 2 - 154;
	m_btnDelete.m_xPos = m_width / 2 - 74;
	m_btnCreateNew.m_xPos = m_btnCancel.m_xPos = m_width / 2 + 4;

	m_btnSelect.m_text = Language::getInstance()->get("selectWorld.select");
	m_btnDelete.m_text = Language::getInstance()->get("selectWorld.delete");
	m_btnCreateNew.m_text = Language::getInstance()->get("selectWorld.create");
	m_btnCancel.m_text = Language::getInstance()->get("gui.cancel");

	m_buttons.push_back(&m_btnSelect);
	m_buttons.push_back(&m_btnCreateNew);
	m_buttons.push_back(&m_btnCancel);
	m_buttons.push_back(&m_btnDelete);
	m_buttons.push_back(&m_btnRename);

	for (Button* button : m_buttons)
		m_buttonTabList.push_back(button);
}

bool SelectWorldScreen::isInGameScreen()
{
	return true;
}

void SelectWorldScreen::keyPressed(int code)
{
	if (m_pMinecraft->getOptions()->getKey(KM_MENU_OK) == code)
		m_pWorldSelectionList->selectItem(m_selectedIndex, true);

	Screen::keyPressed(code);
}

static char g_SelectWorldFilterArray[] = { '/','\n','\r','\x09','\0','\xC','`','?','*','\\','<','>','|','"',':'};

void SelectWorldScreen::render(int mouseX, int mouseY, float f)
{
	m_pWorldSelectionList->render(mouseX, mouseY, f);

	Screen::render(mouseX, mouseY, f);

	drawCenteredString(m_pMinecraft->m_pFont, m_title, m_width / 2, 8, 0xFFFFFFFF);
}

void SelectWorldScreen::buttonClicked(Button* pButton)
{
	if (pButton->m_buttonId == m_btnCreateNew.m_buttonId)
	{
		m_pMinecraft->setScreen(new CreateWorldScreen);
	}

	if (pButton->m_buttonId == m_btnDelete.m_buttonId)
	{
		LevelSummary& summary = m_levels[m_selectedIndex];
		m_pMinecraft->setScreen(new ConfirmScreen(this,
			Language::getInstance()->get("selectWorld.deleteQuestion"),
			"\'" + summary.m_levelName + "\' " + Language::getInstance()->get("selectWorld.deleteWarning"), 
			Language::getInstance()->get("selectWorld.deleteButton"),
			Language::getInstance()->get("gui.cancel"),
			m_selectedIndex));
	}

	if (pButton->m_buttonId == m_btnRename.m_buttonId)
	{
		m_pMinecraft->setScreen(new EditWorldScreen(this, m_levels[m_selectedIndex]));
	}

	if (pButton->m_buttonId == m_btnCancel.m_buttonId)
	{
		m_pMinecraft->setScreen(new TitleScreen);
	}

	if (pButton->m_buttonId == m_btnSelect.m_buttonId)
	{
		m_pWorldSelectionList->selectItem(m_selectedIndex, true);
	}
}

void SelectWorldScreen::confirmResult(bool b, int i)
{
	if (b)
	{
		m_pMinecraft->getLevelSource()->deleteLevel(m_levels[i].m_fileName);
	}
	m_pMinecraft->setScreen(this);
}

void SelectWorldScreen::loadLevelSource()
{
	m_levels.clear();

	m_pMinecraft->getLevelSource()->getLevelList(m_levels);

	std::sort(m_levels.begin(), m_levels.end());
}
