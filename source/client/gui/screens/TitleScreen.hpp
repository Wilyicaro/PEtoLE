/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../Screen.hpp"

class TitleScreen : public Screen
{
public:
	TitleScreen();

	void init() override;
	void buttonClicked(Button*) override;
	bool isInGameScreen() override;
	void render(int, int, float) override;

	void drawSplash();
	void drawTitle();

	std::string getSplashString();

	void onClose() override;

protected:
	Button m_startButton;
	Button m_joinButton;
	Button m_optionsButton;
	Button m_exitButton;
	std::string m_mojangText;
	int m_mojangTextWidth;
	std::string m_versionText;
	int m_versionTextWidth;

	int m_chosenSplash;

	TileRenderer m_tileRenderer;
	Random m_random;
};

