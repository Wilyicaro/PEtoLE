/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../Screen.hpp"

class OptionsScreen : public Screen
{
public:
	OptionsScreen(Options*);
	~OptionsScreen();
	void init() override;
	void render(int, int, float) override;

	void buttonClicked(Button* pButton) override;
	void updateServerVisibilityText();

protected:
	std::string m_title;

private:
	Button m_BackButton;
	Button m_videoButton;
	Button m_serverVisibleButton;
	std::vector<Button*> m_configButtons;
};