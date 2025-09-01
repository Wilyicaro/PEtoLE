/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../GuiComponent.hpp"
#include "client/app/Minecraft.hpp"

class Screen;

class Button : public GuiComponent
{
private:
	void _init();
public:
	Button(int, int x, int y, int width, int height, const std::string&);
	Button(int, int x, int y, const std::string&);
	Button(int, const std::string&);

	// I can't possibly explain why Minecraft is referenced here
	virtual bool clicked(Minecraft*, int xPos, int yPos);
	virtual int  getYImage(bool bHovered);
	virtual void released(int xPos, int yPos);
	virtual void renderBg(Minecraft*, int, int);
	virtual void render(Minecraft*, int xPos, int yPos);

public:
	int m_width;
	int m_height;
	int m_xPos;
	int m_yPos;
	std::string m_text;
	int m_buttonId;
	bool m_bEnabled;
	bool m_bVisible;
	bool m_bHoveredOrFocused;

#ifndef ORIGINAL_CODE
	int m_lastX;
	int m_lastY;
#endif
};

