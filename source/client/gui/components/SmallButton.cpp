/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SmallButton.hpp"

// @NOTE: Used in the ConfirmScreen.
// I reckon this was used in the OptionsScreen as well, since the button sizes are the same.

SmallButton::SmallButton(int id, int x, int y, const std::string& str) :
	Button(id, x, y, 150, 20, str),
	m_pOption(nullptr)
{
}

SmallButton::SmallButton(int id, int x, int y, int width, int height, const std::string& str) :
	Button(id, x, y, width, height, str),
	m_pOption(nullptr)
{
}

SmallButton::SmallButton(int id, int x, int y, Options::OptionEntry* pOption, const std::string& str) :
	Button(id, x, y, 150, 20, str),
	m_pOption(pOption)
{
}

void SmallButton::pressed(Minecraft* mc, int xPos, int yPos)
{
	if (getOption())
	{
		getOption()->toggle();
		m_text = mc->getOptions()->getMessage(*getOption());
		mc->getOptions()->save();
	}
}

Options::OptionEntry* SmallButton::getOption()
{
	return m_pOption;
}
