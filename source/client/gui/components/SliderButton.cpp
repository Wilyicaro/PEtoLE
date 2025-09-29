/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SliderButton.hpp"

SliderButton::SliderButton(int id, int x, int y, Options::FloatOption& option, const std::string& message, float initialValue) : Button(id, x, y, 150, 20, message),
	m_option(option),
	m_value(initialValue),
	m_bDragging(false)
{
}

void SliderButton::pressed(Minecraft* mc, int xPos, int yPos)
{
	if (clicked(mc, xPos, yPos))
	{
		m_value = (xPos - (m_xPos + 4)) / float(m_width - 8);

		float oldValue = m_option.get();
		m_option.set(m_value = Mth::clamp(m_value, 0.0f, 1.0f));
		if (oldValue != m_option.get()) mc->saveOptions();
		m_text = mc->getOptions()->getMessage(getOption());
		m_bDragging = true;
	}
}

int SliderButton::getYImage(bool bHovered)
{
	return 0;
}

void SliderButton::released(int xPos, int yPos)
{
	m_bDragging = false;
}

void SliderButton::renderBg(Minecraft* mc, int xPos, int yPos)
{
	if (m_bVisible)
	{
		if (m_bDragging)
		{
			m_value = (xPos - (m_xPos + 4)) / float(m_width - 8);

			float oldValue = m_option.get();
			m_option.set(m_value = Mth::clamp(m_value, 0.0f, 1.0f));
			if (oldValue != m_option.get()) mc->saveOptions();
			m_text = mc->getOptions()->getMessage(getOption());
		}

		glColor4f(1.0F, 1.0F, 1.0F, 1.0F);
		blit(m_xPos + int(m_value * float(m_width - 8)), m_yPos, 0, 66, 4, 20, 256, 256);
		blit(m_xPos + int(m_value * float(m_width - 8)) + 4, m_yPos, 196, 66, 4, 20, 256, 256);
	}
}
