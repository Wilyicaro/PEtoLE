/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "client/renderer/Tesselator.hpp"
#include "client/renderer/Font.hpp"


// @TODO: GuiElement (virtuals) inheriting GuiComponent
class GuiComponent
{
public:
	GuiComponent();
	virtual ~GuiComponent() {}

	static void blit(int x, int y, int z, float u, float v, int width, int height, int textureWidth, int textureHeight);
	void blit(int x, int y, float u, float v, int width, int height, int textureWidth, int textureHeight) const {
		blit(x, y, zLevel, u, v, width, height, textureWidth, textureHeight);
	}
	void blit(int x, int y, float u, float v, int width, int height) const {
		blit(x, y, u, v, width, height, width, height);
	}
	void drawCenteredString(Font*, const std::string&, int cx, int cy, int color);
	void drawString(Font*, const std::string&, int cx, int cy, int color);
	static void fill(int left, int top, int right, int bottom, int color, int zLevel);
	static void drawRect(int x, int y, int width, int height, int color, int zLevel = 0);
	static void fillGradient(int left, int top, int right, int bottom, int colorUp, int colorDown, int zLevel);
	void fill(int left, int top, int right, int bottom, int color) const {
		fill(left, top, right, bottom, color, zLevel);
	}
	void fillGradient(int left, int top, int right, int bottom, int colorUp, int colorDown) const {
		fillGradient(left, top, right, bottom, colorUp, colorDown, zLevel);
	}

public:
	float zLevel;
};

