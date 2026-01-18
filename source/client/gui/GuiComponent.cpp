/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "GuiComponent.hpp"

#ifdef _WIN32
#pragma warning (disable : 4244)
#endif

GuiComponent::GuiComponent() : zLevel (0)
{
}



void GuiComponent::blit(int x, int y, int z, float u, float v, int width, int height, int textureWidth, int textureHeight, const Color& color)
{
	Tesselator& t = Tesselator::instance;

	if (!textureHeight) textureHeight = height;
	if (!textureWidth) textureWidth = width;

	t.begin();
	t.color(color);
	t.vertexUV(x, y + height, z, u / textureWidth, (v + height) / textureHeight);
	t.vertexUV(x + width, y + height, z, (u + width) / textureWidth, (v + height) / textureHeight);
	t.vertexUV(x + width, y, z, (u + width) / textureWidth, v / textureHeight);
	t.vertexUV(x, y, z, u / textureWidth, v / textureHeight);
	t.draw();
}

void GuiComponent::drawCenteredString(Font* pFont, const std::string& str, int cx, int cy, int color)
{
	int width = pFont->width(str);
	pFont->drawShadow(str, cx - width / 2, cy, color);
}

void GuiComponent::drawString(Font* pFont, const std::string& str, int cx, int cy, int color)
{
	pFont->drawShadow(str, cx, cy, color);
}

void GuiComponent::fill(int a2, int a3, int a4, int a5, int a6, int zLevel)
{
	glEnable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(float(GET_RED(a6)) / 255.0f, float(GET_GREEN(a6)) / 255.0f, float(GET_BLUE(a6)) / 255.0f, float(GET_ALPHA(a6)) / 255.0f);

	Tesselator& t = Tesselator::instance;
	t.begin();

	t.vertex(a2, a5, zLevel);
	t.vertex(a4, a5, zLevel);
	t.vertex(a4, a3, zLevel);
	t.vertex(a2, a3, zLevel);

	t.draw();

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void GuiComponent::drawRect(int a2, int a3, int a4, int a5, int a6, int zLevel)
{
	glDisable(GL_TEXTURE_2D);

	Tesselator& t = Tesselator::instance;
	t.begin();
	t.color(a6);
	t.vertex(a2, a3 + a5, zLevel);
	t.vertex(a2 + a4, a3 + a5, zLevel);
	t.vertex(a2 + a4, a3, zLevel);
	t.vertex(a2, a3, zLevel);

	t.draw();

	glEnable(GL_TEXTURE_2D);
}

void GuiComponent::fillGradient(int a2, int a3, int a4, int a5, int a6, int a7, int zLevel)
{
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel(GL_SMOOTH);

	Tesselator& t = Tesselator::instance;
	t.begin();

	// note: for some stupid reason OG uses the float overload.
	t.color(float(GET_RED(a6)) / 255.0f, float(GET_GREEN(a6)) / 255.0f, float(GET_BLUE(a6)) / 255.0f, float(GET_ALPHA(a6)) / 255.0f);
	t.vertex(a4, a3, zLevel);
	t.vertex(a2, a3, zLevel);
	t.color(float(GET_RED(a7)) / 255.0f, float(GET_GREEN(a7)) / 255.0f, float(GET_BLUE(a7)) / 255.0f, float(GET_ALPHA(a7)) / 255.0f);
	t.vertex(a2, a5, zLevel);
	t.vertex(a4, a5, zLevel);

	t.draw();

	glShadeModel(GL_FLAT);
	glDisable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_TEXTURE_2D);
}

void GuiComponent::hLine(int x0, int x1, int y, int color)
{
	if (x1 < x0)
		std::swap(x0, x1);

	fill(x0, y, x1 + 1, y + 1, color);
}

void GuiComponent::vLine(int x, int y0, int y1, int color)
{
	if (y1 < y0)
		std::swap(y0, y1);

	fill(x, y0 + 1, x + 1, y1, color);
}
