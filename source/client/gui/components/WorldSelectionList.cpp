/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "WorldSelectionList.hpp"
#include "client/gui/screens/SelectWorldScreen.hpp"
#include "common/Utils.hpp"
#include "stats/StatFormatter.hpp"

WorldSelectionList::WorldSelectionList(SelectWorldScreen* parent) : ScrolledSelectionList(parent->m_pMinecraft, parent->m_width, parent->m_height, 32, parent->m_height - 64, 36),
	m_pParent(parent)
{
}

int WorldSelectionList::getItemCount()
{
	return m_pParent->m_levels.size();
}

void WorldSelectionList::selectItem(int index, bool b)
{
	m_pParent->m_selectedIndex = index;
	bool valid = index >= 0 && index < getItemCount();
	m_pParent->m_btnSelect.m_bEnabled = valid;
	m_pParent->m_btnRename.m_bEnabled = valid;
	m_pParent->m_btnDelete.m_bEnabled = valid;
	if (b && valid)
	{
		m_pParent->m_pMinecraft->m_bUsingScreen = true;
		m_pParent->m_pMinecraft->selectLevel(m_pParent->m_levels[index].m_fileName);
	}
}

bool WorldSelectionList::isSelectedItem(int index)
{
	return m_pParent->m_selectedIndex == index;
}


void WorldSelectionList::renderItem(int index, int xPos, int yPos, int width, Tesselator& t)
{
	LevelSummary& summary = m_pParent->m_levels[index];
	std::string levelName = summary.m_levelName;
	if (levelName.empty())
		levelName = Language::getInstance()->get("selectWorld.world");

	std::string desc = summary.m_fileName + " (" + getFormattedDate(summary.m_lastPlayed) + ", " + DecimalFormatter::INSTANCE.format((summary.m_sizeOnDisk / 1024L * 100L / 1024L) / 100.0F) + " MB)";

	m_pMinecraft->m_pFont->drawShadow(levelName, xPos + 2, yPos + 1, 16777215);
	m_pMinecraft->m_pFont->drawShadow(desc, xPos + 2, yPos + 12, 8421504);
	//Conversion warning, but we don't have conversion for now :(
	// if (summary.m_bNeedsConversion)
	//	m_pMinecraft->m_pFont->drawShadow(conversionWarning, xPos + 2, yPos + 12 + 10, 8421504);
}

void WorldSelectionList::renderBackground(float f)
{
	m_pParent->renderBackground();
}