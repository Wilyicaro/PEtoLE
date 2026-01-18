/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../Screen.hpp"
#include "../components/WorldSelectionList.hpp"

class SelectWorldScreen : public Screen
{
public:
	SelectWorldScreen();
	~SelectWorldScreen();

	void init() override;
	bool isInGameScreen() override;
	void keyPressed(int code) override;
	void render(int mouseX, int mouseY, float f) override;
	void buttonClicked(Button* pButton) override;

	virtual void confirmResult(bool b, int i) override;
	void loadLevelSource();

public:
	Button m_btnDelete;
	Button m_btnRename;
	Button m_btnCreateNew;
	Button m_btnCancel;
	Button m_btnSelect;
	WorldSelectionList* m_pWorldSelectionList;
	std::vector<LevelSummary> m_levels;
	int m_selectedIndex;
	std::string m_title;
};

