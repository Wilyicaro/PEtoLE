/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Button.hpp"

class SliderButton : public Button
{
public:
	SliderButton(int id, int x, int y, Options::FloatOption&, const std::string&, float initialValue);

	virtual bool clicked(Minecraft*, int xPos, int yPos) override;
	int getYImage(bool bHovered) override;
	void released(int xPos, int yPos) override;
	void renderBg(Minecraft*, int, int) override;


	Options::FloatOption& getOption() { return m_option; };

private:
	Options::FloatOption& m_option;

public:
	float m_value;
	bool m_bDragging;
};