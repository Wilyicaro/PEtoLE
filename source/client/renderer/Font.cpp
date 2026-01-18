/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Font.hpp"
#include "Tesselator.hpp"

constexpr char COLOR_START_CHAR = '\xa7';

Font::Font(Options* pOpts, const std::string& fileName, Textures* pTexs) :
	m_fileName(fileName), m_pOptions(pOpts), m_pTextures(pTexs)
{
	init(pOpts);
}

void Font::init(Options* pOpts)
{
	GLuint texID = m_pTextures->loadTexture(m_fileName, true);
	Texture* pTexture = m_pTextures->getTemporaryTextureData(texID);
	if (!pTexture) return;

	for (int i = 0; i < 256; i++) // character number
	{
		// note: the 'widthMax' behavior is assumed. It might not be like that exactly
		int widthMax = 0;

		if (i == 32) // space
		{
			widthMax = 2;
		}
		else
		{
			for (int j = 7; j >= 0; j--) // x position
			{
				int x = (i % 16), y = (i / 16);
				int pixelDataIndex = pTexture->m_width * 8 * y + 8 * x + j;

				for (int k = 0; k < 8; k++)
				{
					if ((uint8_t)pTexture->m_pixels[pixelDataIndex] != 0)
					{
						if (widthMax < j)
							widthMax = j;
					}

					pixelDataIndex += pTexture->m_width;
				}
			}
		}

		m_charWidthInt[i] = widthMax + 2;
		m_charWidthFloat[i] = float (widthMax) + 2;
		GLuint id;
		xglGenBuffers(1, &id);
		Tesselator& t = Tesselator::instance;
		t.begin();
		buildChar(i, 0, 0);
		m_bakedChars[i] = t.end(id);
	}
}

void Font::buildChar(unsigned char chr, float x, float y)
{
	Tesselator& t = Tesselator::instance;

	float u = float((chr % 16) * 8);
	float v = float((chr / 16) * 8);

	constexpr float D128 = (1.0f / 128.0f);

#define CO (7.99f)

	t.vertexUV(x,      y + CO, 0.0f,  u       * D128, (v + CO) * D128);
	t.vertexUV(x + CO, y + CO, 0.0f, (u + CO) * D128, (v + CO) * D128);
	t.vertexUV(x + CO, y,      0.0f, (u + CO) * D128,  v       * D128);
	t.vertexUV(x,      y,      0.0f,  u       * D128,  v       * D128);

#undef CO
}

void Font::drawShadow(const std::string& str, int x, int y, int color)
{
	draw(str, x + 1, y + 1, color, true);
	draw(str, x, y, color, false);
}

void Font::drawString(const std::string& str, int x, int y, int color, bool hasShadow)
{
	if (hasShadow)
		drawShadow(str, x, y, color);
	else
		draw(str, x, y, color);
		
}

void Font::drawWordWrap(const std::string& str, int x, int y, int color, int width, int lineHeight, bool shadow)
{
	drawWordWrap(split(str, width), x, y, color, lineHeight, shadow);
}

void Font::drawWordWrap(const std::vector<std::string>& lines, int x, int y, int color, int lineHeight, bool shadow)
{
	for (const std::string& line : lines)
	{
		drawString(line, x, y, color, shadow);
		y += lineHeight;
	}
}

void Font::draw(const std::string& str, int x, int y, int colorI, bool bShadow)
{
	if (str.empty()) return;

	uint32_t color = colorI;

	if (bShadow)
		color = (color & 0xFF000000U) + ((color & 0xFCFCFCu) >> 2);

	m_pTextures->loadAndBindTexture(m_fileName);

	uint32_t red = (color >> 16) & 0xFF;
	uint32_t grn = (color >> 8) & 0xFF;
	uint32_t blu = (color >> 0) & 0xFF;
	uint32_t alp = (color >> 24) & 0xFF;

	float alpf = float(alp) / 255.0f;
	if (alpf == 0.0f)
		alpf = 1.0f;

	glColor4f(float(red) / 255.0f, float(grn) / 255.0f, float(blu) / 255.0f, alpf);
	glPushMatrix();


	glTranslatef(float(x), float(y), 0.0f);

	float xOff = 0.0f;

	for (int i = 0; i < int(str.size()); i++)
	{
		if (str[i] == '\n')
		{
			glTranslatef(-xOff, 12.0f, 0.0f);
			xOff = 0.0f;
			continue;
		}

		uint8_t x = uint8_t(str[i]);

		drawArrayVT(m_bakedChars[x].m_glID, 16);

		xOff += m_charWidthFloat[x];
		glTranslatef(m_charWidthFloat[x], 0.0f, 0.0f);
	}

	glPopMatrix();
}

void Font::drawSlow(const std::string& str, int x, int y, int colorI, bool bShadow)
{
	if (str.empty()) return;

	uint32_t color = colorI;

	if (bShadow)
		color = (color & 0xFF000000U) + ((color & 0xFCFCFCu) >> 2);

	m_pTextures->loadAndBindTexture(m_fileName);

	uint32_t red = (color >> 16) & 0xFF;
	uint32_t grn = (color >>  8) & 0xFF;
	uint32_t blu = (color >>  0) & 0xFF;
	uint32_t alp = (color >> 24) & 0xFF;

	float alpf = float(alp) / 255.0f;
	if (alpf == 0.0f)
		alpf = 1.0f;

	glColor4f(float(red) / 255.0f, float(grn) / 255.0f, float(blu) / 255.0f, alpf);
	glPushMatrix();

	Tesselator& t = Tesselator::instance;
	t.begin();

	glTranslatef(float(x), float(y), 0.0f);

	float cXPos = 0.0f, cYPos = 0.0f;

	for (int i = 0; i < int(str.size()); i++)
	{
		if (str[i] == '\n')
		{
			cYPos += 12.0f;
			cXPos = 0;
			continue;
		}

		uint8_t x = uint8_t(str[i]);

		buildChar(x, cXPos, cYPos);

		cXPos += m_charWidthFloat[x];
	}

	t.draw();

	glPopMatrix();
}

void Font::onGraphicsReset()
{
	init(m_pOptions);
}

int Font::height(const std::string& str, int maxWidth)
{
	return split(str, maxWidth).size() * 8;
}

int Font::width(const std::string& str)
{
	int maxLineWidth = 0, currentLineWidth = 0;

	for (int i = 0; i < int(str.size()); i++)
	{
		char chr = str[i];

		if (chr == COLOR_START_CHAR)
		{
			// skip the color code as well
			i++;
			continue;
		}
		if (chr == '\n')
		{
			if (maxLineWidth < currentLineWidth)
				maxLineWidth = currentLineWidth;
			currentLineWidth = 0;
		}

		currentLineWidth += m_charWidthInt[uint8_t(str[i])];
	}

	if (maxLineWidth < currentLineWidth)
		maxLineWidth = currentLineWidth;

	return maxLineWidth;
}

std::vector<std::string> Font::split(const std::string& text, int maxWidth)
{
	std::vector<std::string> lines;

	std::vector<std::string> paragraphs;
	size_t start = 0;
	size_t newlinePos = text.find('\n');
	while (newlinePos != std::string::npos)
	{
		paragraphs.push_back(text.substr(start, newlinePos - start));
		start = newlinePos + 1;
		newlinePos = text.find('\n', start);
	}
	paragraphs.push_back(text.substr(start));

	for (const auto& paragraph : paragraphs)
	{
		if (paragraph.empty())
		{
			lines.emplace_back("");
			continue;
		}

		std::string currentLine;
		std::istringstream iss(paragraph);
		std::string word;

		while (iss >> word) {
			std::string testLine = currentLine.empty() ? word : currentLine + " " + word;

			if (width(testLine) <= maxWidth)
				currentLine = testLine;
			else
			{
				if (!currentLine.empty())
				{
					lines.push_back(currentLine);
					currentLine.clear();
				}

				while (!word.empty() && width(word) > maxWidth)
				{
					size_t breakPos = 0;
					for (size_t j = 1; j <= word.length(); ++j)
					{
						if (width(word.substr(0, j)) <= maxWidth)
							breakPos = j;
						else
							break;
					}

					if (breakPos == 0) breakPos = 1;

					std::string chunk = word.substr(0, breakPos);
					lines.push_back(chunk);
					word = word.substr(breakPos);
				}

				currentLine = word;
			}
		}

		if (!currentLine.empty())
			lines.push_back(currentLine);
	}

	while (!lines.empty() && lines.back().empty())
		lines.pop_back();

	if (lines.empty())
		lines.emplace_back("");

	return lines;
}
