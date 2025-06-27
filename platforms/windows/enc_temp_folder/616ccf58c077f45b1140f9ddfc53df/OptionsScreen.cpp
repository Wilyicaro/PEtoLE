/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "OptionsScreen.hpp"
#include "StartMenuScreen.hpp"
#include "PauseScreen.hpp"

enum eOptionsButton
{
	OB_BACK = 1,
	OB_AO,
	OB_SRV_VIS,
	OB_FANCY_GFX,
	OB_INVERT_Y,
	OB_ANAGLYPHS,
	OB_VIEW_BOB,
	OB_VIEW_DIST,
	OB_DYNAMIC_HAND,
	OB_FLY_HAX,
	OB_SPLIT_CONTR,
	OB_AUTOJUMP,
	OB_BLOCK_OUT,
	OB_MIPMAPS
};

OptionsScreen::OptionsScreen()
	:m_BackButton(OB_BACK, 0, 0, 200, 20, "Done"),
	m_AOButton(OB_AO, 0, 0, 150, 20, ""),
	m_srvVisButton(OB_SRV_VIS, 0, 0, 150, 20, ""),
	m_fancyGfxButton(OB_FANCY_GFX, 0, 0, 150, 20, ""),
	m_invertYButton(OB_INVERT_Y, 0, 0, 150, 20, ""),
	m_anaglyphsButton(OB_ANAGLYPHS, 0, 0, 150, 20, ""),
	m_viewBobButton(OB_VIEW_BOB, 0, 0, 150, 20, ""),
	m_viewDistButton(OB_VIEW_DIST, 0, 0, 150, 20, ""),
	m_dynamicHandButton(OB_DYNAMIC_HAND, 0, 0, 150, 20, ""),
	m_flightHaxButton(OB_FLY_HAX, 0, 0, 150, 20, ""),
	m_splitControlsButton(OB_SPLIT_CONTR, 0, 0, 150, 20, ""),
	m_autoJumpButton(OB_AUTOJUMP, 0, 0, 150, 20, ""),
	m_blockOutlinesButton(OB_BLOCK_OUT, 0, 0, 150, 20, ""),
	m_mipmapsButton(OB_MIPMAPS, 0, 0, 150, 20, "")
{
}

#ifndef ORIGINAL_CODE
static std::string BoolOptionStr(bool b)
{
	return b ? "ON" : "OFF";
}
static std::string ViewDistanceStr(int dist)
{
	switch (dist)
	{
	case 0: return "EXTREME";
	case 1: return "FAR";
	case 2: return "NORMAL";
	case 3: return "SHORT";
	case 4: return "TINY";
	default:
	{
		std::stringstream ss;
		ss << dist;
		return ss.str();
	}
	}
}

void OptionsScreen::UpdateTexts()
{
	Options* o = m_pMinecraft->getOptions();

	m_AOButton.m_text = "Smooth lighting: " + BoolOptionStr(o->m_bAmbientOcclusion);
	m_invertYButton.m_text = "Invert Y-axis: " + BoolOptionStr(o->m_bInvertMouse);
	m_viewBobButton.m_text = "View bobbing: " + BoolOptionStr(o->m_bViewBobbing);
	m_anaglyphsButton.m_text = "3d Anaglyphs: " + BoolOptionStr(o->m_bAnaglyphs);
	m_fancyGfxButton.m_text = "Fancy graphics: " + BoolOptionStr(o->m_bFancyGraphics);
	m_flightHaxButton.m_text = "Flight hax: " + BoolOptionStr(o->m_bFlyCheat);
	m_viewDistButton.m_text = "View distance: " + ViewDistanceStr(o->m_iViewDistance);
	m_dynamicHandButton.m_text = "Dynamic Hand: " + BoolOptionStr(o->m_bDynamicHand);
	m_splitControlsButton.m_text = "Split Controls: " + BoolOptionStr(o->m_bSplitControls);
	m_blockOutlinesButton.m_text = "Block Outlines: " + BoolOptionStr(o->m_bBlockOutlines);
	m_mipmapsButton.m_text = "Mipmaps: " + BoolOptionStr(o->m_bMipmaps);
	m_autoJumpButton.m_text = "Auto-Jump: " + BoolOptionStr(o->m_bAutoJump);
	m_srvVisButton.m_text = "Server " + std::string(o->m_bServerVisibleDefault ? "visible" : "invisible") + " by default";
}
#endif

void OptionsScreen::init()
{
	m_pMinecraft->platform()->showDialog(AppPlatform::DLG_OPTIONS);
	m_pMinecraft->platform()->createUserInput();
	m_BackButton.m_xPos = m_width / 2 - m_BackButton.m_width / 2;
	m_BackButton.m_yPos = m_height - 33;
	m_BackButton.m_height = 20;
	m_buttons.push_back(&m_BackButton);

	m_buttons.push_back(&m_AOButton);
	m_buttons.push_back(&m_srvVisButton);
	m_buttons.push_back(&m_fancyGfxButton);
	m_buttons.push_back(&m_invertYButton);
	m_buttons.push_back(&m_anaglyphsButton);
	m_buttons.push_back(&m_viewBobButton);
	m_buttons.push_back(&m_viewDistButton);
	m_buttons.push_back(&m_flightHaxButton);
	m_buttons.push_back(&m_splitControlsButton);
	m_buttons.push_back(&m_autoJumpButton);
	m_buttons.push_back(&m_blockOutlinesButton);
	if (m_pMinecraft->m_pTextures->hasTerrainMipmap())
		m_buttons.push_back(&m_mipmapsButton);

	m_buttonTabList.push_back(&m_AOButton);
	m_buttonTabList.push_back(&m_srvVisButton);
	m_buttonTabList.push_back(&m_fancyGfxButton);
	m_buttonTabList.push_back(&m_viewDistButton);
	m_buttonTabList.push_back(&m_invertYButton);
	m_buttonTabList.push_back(&m_anaglyphsButton);
	m_buttonTabList.push_back(&m_viewBobButton);
	m_buttonTabList.push_back(&m_flightHaxButton);
	m_buttonTabList.push_back(&m_splitControlsButton);
	m_buttonTabList.push_back(&m_autoJumpButton);
	m_buttonTabList.push_back(&m_blockOutlinesButton);
	if (m_pMinecraft->m_pTextures->hasTerrainMipmap())
		m_buttonTabList.push_back(&m_mipmapsButton);

	for (int i = 0; i < m_buttonTabList.size(); i++)
	{
		m_buttonTabList[i]->m_xPos = m_width / 2 - 155 + i % 2 * 160;
		m_buttonTabList[i]->m_yPos = m_height / 6 + 24 * (i >> 1);
	}


	m_buttonTabList.push_back(&m_BackButton);

	UpdateTexts();
}

void OptionsScreen::render(int a, int b, float c)
{
	renderBackground();

	if (m_pMinecraft->m_pPlatform->getUserInputStatus() >= 0)
	{
		m_pMinecraft->setScreen(new StartMenuScreen);
	}

	drawCenteredString(m_pFont, "Options", m_width / 2, 20, 0xFFFFFF);

	Screen::render(a, b, c);
}

void OptionsScreen::removed()
{
	m_pMinecraft->saveOptions();
}

#ifndef ORIGINAL_CODE

void OptionsScreen::buttonClicked(Button* pButton)
{
	Options* o = m_pMinecraft->getOptions();

	switch (pButton->m_buttonId)
	{
	case OB_BACK:
		if (m_pMinecraft->isLevelGenerated())
			m_pMinecraft->setScreen(new PauseScreen);
		else
			m_pMinecraft->setScreen(new StartMenuScreen);
		return;

	case OB_AO:
		o->m_bAmbientOcclusion ^= 1;
		Minecraft::useAmbientOcclusion = o->m_bAmbientOcclusion;
		m_pMinecraft->m_pLevelRenderer->allChanged();
		UpdateTexts();
		return;
	case OB_FANCY_GFX:
		o->m_bFancyGraphics ^= 1;
		m_pMinecraft->m_pLevelRenderer->allChanged();
		UpdateTexts();
		return;
	case OB_VIEW_DIST:
		o->m_iViewDistance = (o->m_iViewDistance + 1) % 4;
		UpdateTexts();
		return;

	case OB_ANAGLYPHS:
		o->m_bAnaglyphs ^= 1;
		break;
	case OB_INVERT_Y:
		o->m_bInvertMouse ^= 1;
		break;
	case OB_SRV_VIS:
		o->m_bServerVisibleDefault ^= 1;
		break;
	case OB_VIEW_BOB:
		o->m_bViewBobbing ^= 1;
		break;
	case OB_DYNAMIC_HAND:
		o->m_bDynamicHand ^= 1;
		break;
	case OB_FLY_HAX:
		o->m_bFlyCheat ^= 1;
		break;
	case OB_SPLIT_CONTR:
		o->m_bSplitControls ^= 1;
		break;
	case OB_AUTOJUMP:
		o->m_bAutoJump ^= 1;
		break;
	case OB_BLOCK_OUT:
		o->m_bBlockOutlines ^= 1;
		break;
	case OB_MIPMAPS:
		o->m_bMipmaps ^= 1;
		m_pMinecraft->m_pLevelRenderer->allChanged();
		break;
	}

	UpdateTexts();
}

#endif
