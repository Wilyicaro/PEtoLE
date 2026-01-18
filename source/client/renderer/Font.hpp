/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Textures.hpp"
#include "RenderChunk.hpp"
#include "client/options/Options.hpp"

class Font
{
public:
	Font(Options* pOpts, const std::string& fileName, Textures* pTexs);

	void init(Options* pOpts);
	void buildChar(unsigned char chr, float x, float y);
	void draw(const std::string&, int x, int y, int color, bool bShadow = false);
	void drawSlow(const std::string&, int x, int y, int color, bool bShadow);
	void drawShadow(const std::string&, int x, int y, int color);
	void drawString(const std::string&, int x, int y, int color, bool hasShadow);

	void drawWordWrap(const std::string&, int x, int y, int color, int width, int lineHeight = 8, bool shadow = false);
	void drawWordWrap(const std::vector<std::string>&, int x, int y, int color, int lineHeight = 8, bool shadow = false);

	void onGraphicsReset();

	int width(const std::string& str);
	std::vector<std::string> split(const std::string& str, int width);
	int height(const std::string& str, int maxWidth = 0);

private:
	int m_charWidthInt[256];
	float m_charWidthFloat[256];
	RenderChunk m_bakedChars[256];
	// huge gap, don't know why it's there...
	std::string m_fileName;
	Options* m_pOptions;
	Textures* m_pTextures;
};

