/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "CreateWorldScreen.hpp"
#include "SelectWorldScreen.hpp"
#include "ProgressScreen.hpp"
#include "common/Util.hpp"
#include <stdio.h>
#include <inttypes.h>

CreateWorldScreen::CreateWorldScreen() :
	m_textName(this, 1, 0, 0, 0, 0, "", Language::getInstance()->get("selectWorld.newWorld")),
	m_textSeed(this, 2, 0, 0, 0, 0, ""),
	m_btnBack(3, "Cancel"),
	m_btnCreate(4, "Create New World"),
	m_btnMoreWorldOptions(5, "More World Options"),
	m_gamemodeButton(6, ""),
	m_btnWorldSize(7, ""),
	m_btnWorldType(8, ""),
	m_bMoreOptions(false),
	m_bIsFlat(false),
	m_gameType(GameType::GAME_TYPE_SURVIVAL),
	m_worldSize(INFINITY_SIZE)
{
}

#define CRAMPED() (100 + 32 + 58 > m_height)

void CreateWorldScreen::init()
{
	bool hasMoreOptions = m_pMinecraft->getOptions()->m_moreWorldOptions.get();
	m_textName.m_width  = m_textSeed.m_width  = 200;
	m_textName.m_height = m_textSeed.m_height = 20;
	m_textName.m_xPos   = m_textSeed.m_xPos   = (m_width - 200) / 2;
	m_textName.m_yPos = 60;
	m_textSeed.m_yPos = hasMoreOptions ? 60 : 116;

	m_btnCreate.m_text = Language::getInstance()->get("selectWorld.create");
	m_btnBack.m_text = Language::getInstance()->get("gui.cancel");

	if (hasMoreOptions)
	{
		m_btnCreate.m_yPos = m_btnBack.m_yPos = m_height - 28;
		m_btnBack.m_width = m_btnCreate.m_width = 150;

		m_btnBack.m_xPos = m_width / 2 + 5;
		m_btnCreate.m_xPos = m_width / 2 - 155;
	}
	else
	{
		m_btnCreate.m_yPos = m_height - 56;
		m_btnBack.m_yPos = m_height - 30;
		m_btnBack.m_width = m_btnCreate.m_width = 200;

		m_btnBack.m_xPos = m_width / 2 - 200 / 2;
		m_btnCreate.m_xPos = m_width / 2 - 200 / 2;
	}

	if (!hasMoreOptions || !m_bMoreOptions)
		m_textInputs.push_back(&m_textName);
	if (!hasMoreOptions || m_bMoreOptions)
		m_textInputs.push_back(&m_textSeed);
	m_buttons.push_back(&m_btnBack);
	m_buttons.push_back(&m_btnCreate);
	m_buttonTabList.push_back(&m_btnBack);
	m_buttonTabList.push_back(&m_btnCreate);

	if (hasMoreOptions)
	{
		m_gamemodeButton.m_width = m_btnMoreWorldOptions.m_width = m_btnWorldSize.m_width = m_btnWorldType.m_width = 150;
		m_gamemodeButton.m_xPos = m_btnMoreWorldOptions.m_xPos = m_width / 2 - 75;
		m_gamemodeButton.m_yPos = m_btnWorldSize.m_yPos = m_btnWorldType.m_yPos = 100;
		m_btnMoreWorldOptions.m_yPos = 172;
		m_btnMoreWorldOptions.m_text = Language::getInstance()->get(m_bMoreOptions ? "gui.done" : "selectWorld.moreWorldOptions");
		m_buttons.push_back(&m_btnMoreWorldOptions);
		m_buttonTabList.push_back(&m_btnMoreWorldOptions);

		if (m_bMoreOptions)
		{
			m_btnWorldSize.m_text = Language::getInstance()->get("selectWorld.worldSize", Language::getInstance()->get(worldSizeNames[m_worldSize]));
			m_btnWorldSize.m_xPos = m_width / 2 - 155;
			m_btnWorldType.m_text = Language::getInstance()->get("selectWorld.worldType", Language::getInstance()->get(m_bIsFlat ? "selectWorld.worldType.flat" : "selectWorld.worldType.default"));
			m_btnWorldType.m_xPos = m_width / 2 + 5;
			m_buttons.push_back(&m_btnWorldSize);
			m_buttonTabList.push_back(&m_btnWorldSize);
			m_buttons.push_back(&m_btnWorldType);
			m_buttonTabList.push_back(&m_btnWorldType);
		}
		else
		{
			m_gamemodeButton.m_text = Language::getInstance()->get("selectWorld.gameMode", Language::getInstance()->get(m_gameType == GameType::GAME_TYPE_SURVIVAL ? "selectWorld.gameMode.survival" : "selectWorld.gameMode.creative"));
			m_buttons.push_back(&m_gamemodeButton);
			m_buttonTabList.push_back(&m_gamemodeButton);
		}
	}

	m_textName.init(m_pFont);
	m_textSeed.init(m_pFont);

	// 100 - yPos of textSeed
	// 32  - offset + height of "Leave blank for random" text
	// 58  - approximately the Y position of the create button
	bool crampedMode = CRAMPED();
	if (crampedMode)
	{
		// crush everything down as much as we can
		m_textName.m_yPos -= 20;
		m_textSeed.m_yPos -= 20;
		m_btnCreate.m_yPos += 10;
		m_btnBack.m_yPos += 5;
	}
	onTextBoxUpdated(m_textName.getKey());
}

static char g_CreateWorldFilterArray[] = { '/','\n','\r','\x09','\0','\xC','`','?','*','\\','<','>','|','"',':' };

static std::string GetUniqueLevelName(LevelStorageSource* pSource, const std::string& in)
{
	std::set<std::string> maps;

	std::vector<LevelSummary> vls;
	pSource->getLevelList(vls);

	for (int i = 0; i < int(vls.size()); i++)
	{
		const LevelSummary& ls = vls[i];
		maps.insert(ls.m_fileName);
	}

	std::string out = in;
	while (maps.find(out) != maps.end())
		out += "-";

	return out;
}

void CreateWorldScreen::onTextBoxUpdated(int id)
{
	if (id == m_textName.getKey())
	{
		m_levelId = Util::stringTrim(m_textName.getText());

		for (int i = 0; i < sizeof(g_CreateWorldFilterArray); i++)
		{
			std::string str;
			str.push_back(g_CreateWorldFilterArray[i]);
			Util::stringReplace(m_levelId, str, "");
		}

		m_levelId = GetUniqueLevelName(m_pMinecraft->m_pLevelStorageSource, m_levelId);
	}

}

void CreateWorldScreen::buttonClicked(Button* pButton)
{
	if (pButton->m_buttonId == m_btnBack.m_buttonId)
	{
		m_pMinecraft->setScreen(new SelectWorldScreen);
	}

	if (pButton->m_buttonId == m_btnCreate.m_buttonId)
	{
		std::string seedStr = m_textSeed.getText();

		std::string levelNickname = Util::stringTrim(m_textName.getText());

		int64_t seed = getEpochTimeS();

		std::string seedThing = Util::stringTrim(seedStr);
		if (!seedThing.empty())
		{
			int64_t num;
			if (sscanf(seedThing.c_str(), "%" PRId64, &num) > 0)
				seed = num, LOG_I("%" PRId64, seed);
			else
				seed = Util::hashCode(seedThing);
		}

		m_pMinecraft->selectLevel(m_levelId, [levelNickname, seed, this](LevelData& data) {
			data.setLevelName(levelNickname);
			data.setSeed(seed);
			data.setGameType(this->m_gameType);
			data.setFlat(m_bIsFlat);
			switch (this->m_worldSize)
			{
			case CLASSIC_SIZE:
				data.setLimit(0, DimensionLimit(ChunkPos(27, 27)));
				data.setLimit(-1, DimensionLimit(ChunkPos(9, 9)));
				break;
			case SMALL_SIZE:
				data.setLimit(0, DimensionLimit(ChunkPos(32, 32)));
				data.setLimit(-1, DimensionLimit(ChunkPos(11, 11)));
				break;
			case MEDIUM_SIZE:
				data.setLimit(0, DimensionLimit(ChunkPos(96, 96)));
				data.setLimit(-1, DimensionLimit(ChunkPos(16, 16)));
				break;
			case LARGE_SIZE:
				data.setLimit(0, DimensionLimit(ChunkPos(160, 160)));
				data.setLimit(-1, DimensionLimit(ChunkPos(20, 20)));
				break;
			default:
				break;
			}
			});
	}

	if (pButton->m_buttonId == m_btnMoreWorldOptions.m_buttonId)
	{
		m_bMoreOptions ^= 1;
		setSize(m_width, m_height);
	}

	if (pButton->m_buttonId == m_gamemodeButton.m_buttonId)
	{
		m_gameType = m_gameType == GameType::GAME_TYPE_SURVIVAL ? GameType::GAME_TYPE_CREATIVE : GameType::GAME_TYPE_SURVIVAL;
		setSize(m_width, m_height);
	}

	if (pButton->m_buttonId == m_btnWorldSize.m_buttonId)
	{
		m_worldSize = WorldSize((m_worldSize + 1) % 5);
		setSize(m_width, m_height);
	}

	if (pButton->m_buttonId == m_btnWorldType.m_buttonId)
	{
		m_bIsFlat ^= 1;
		setSize(m_width, m_height);
	}
}

void CreateWorldScreen::render(int mouseX, int mouseY, float f)
{
	renderBackground();
	Screen::render(mouseX, mouseY, f);

	bool hasMoreOptions = m_pMinecraft->getOptions()->m_moreWorldOptions.get();

	drawCenteredString(m_pFont, Language::getInstance()->get("selectWorld.create"), m_width / 2, CRAMPED() ? 10 : hasMoreOptions ? 20 : m_height / 4 - 40, 0xFFFFFF);
	if (!hasMoreOptions || !m_bMoreOptions)
	{
		drawString(m_pFont, Language::getInstance()->get("selectWorld.enterName"), m_textName.m_xPos, m_textName.m_yPos - 13, 0xA0A0A0);
		drawString(m_pFont, Language::getInstance()->get("selectWorld.resultFolder", m_levelId), m_textName.m_xPos, m_textName.m_yPos + 25, 0xA0A0A0);
	}
	if (!hasMoreOptions || m_bMoreOptions)
	{
		drawString(m_pFont, Language::getInstance()->get("selectWorld.enterSeed"), m_textSeed.m_xPos, m_textSeed.m_yPos - 13, 0xA0A0A0);
		drawString(m_pFont, Language::getInstance()->get("selectWorld.seedInfo"), m_textSeed.m_xPos, m_textSeed.m_yPos + 25, 0xA0A0A0);
	}
}
