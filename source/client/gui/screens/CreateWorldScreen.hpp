/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../Screen.hpp"

#ifndef ORIGINAL_CODE

class CreateWorldScreen : public Screen
{
public:
	CreateWorldScreen();

	enum WorldSize
	{
		INFINITY_SIZE,
		OLD_SIZE,
		CLASSIC_SIZE,
		SMALL_SIZE,
		MEDIUM_SIZE,
		LARGE_SIZE,
		ALL_WORLD_SIZES

	};

	void init() override;
	void onTextBoxUpdated(int id) override;
	void buttonClicked(Button* pButton) override;
	void render(int mouseX, int mouseY, float f) override;

public:
	std::string m_levelId;
	GameType m_gameType;
	WorldSize m_worldSize;
	bool m_bIsFlat;
	bool m_bMoreOptions;
	TextInputBox m_textName;
	TextInputBox m_textSeed;
	Button m_btnBack;
	Button m_btnMoreWorldOptions;
	Button m_btnCreate;
	Button m_gamemodeButton;
	Button m_btnWorldSize;
	Button m_btnWorldType;

	static const constexpr char* worldSizeNames[ALL_WORLD_SIZES] = { "selectWorld.worldSize.infinity", "selectWorld.worldSize.old", "selectWorld.worldSize.classic", "selectWorld.worldSize.small", "selectWorld.worldSize.medium", "selectWorld.worldSize.large" };
};

#endif
