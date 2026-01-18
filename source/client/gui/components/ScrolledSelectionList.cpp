/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

// @TODO: Add keyboard based control

#include "ScrolledSelectionList.hpp"

#define C_ITEM_WIDTH C_SCROLLED_LIST_ITEM_WIDTH

ScrolledSelectionList::ScrolledSelectionList(Minecraft* minecraft, int a3, int a4, int a5, int a6, int a7) :
	m_pMinecraft(minecraft),
	m_y0(float(a5)),
	m_y1(float(a6)),
	m_itemHeight(a7),
	m_width(a3),
	field_1C(a4),
	m_x1(float(a3)),
	m_x0(0.0f),
	m_scrollAmount(0.0f),
	m_mouseYWhenPressed(-2.0f),
	m_scrollBarGrabOffset(1.0F),
	m_lastClickedIndex(-1),
	m_lastClickTime(0),
	m_bRenderSelection(true),
	m_bRenderHeader(false),
	m_headerHeight(0)
{	
}

void ScrolledSelectionList::setRenderSelection(bool b)
{
	m_bRenderSelection = b;
}

int ScrolledSelectionList::getMaxPosition()
{
	return m_headerHeight + m_itemHeight * getItemCount();
}

void ScrolledSelectionList::renderHeader(int a, int b, Tesselator& t)
{
}

void ScrolledSelectionList::renderDecorations(int a, int b)
{
}

void ScrolledSelectionList::clickedHeader(int x, int y)
{
}

int ScrolledSelectionList::getItemAtPosition(int x, int y)
{
	if (x < m_width / 2 - C_ITEM_WIDTH / 2)
		return -1;

	if (x > m_width / 2 + C_ITEM_WIDTH / 2)
		return -1;

	return getItemAtYPositionRaw(transformY(y));
}

void ScrolledSelectionList::capYPosition()
{
	float maxY = float(getMaxPosition()) - (float(m_y1 - m_y0) - 4.0f);
	if (maxY < 0.0f)
		maxY *= 0.5f;

	if (m_scrollAmount < 0.0f)
		m_scrollAmount = 0.0f;
	if (m_scrollAmount > maxY)
		m_scrollAmount = maxY;
}

void ScrolledSelectionList::onClickItem(int index, int mouseX, int mouseY, bool selected)
{
	selectItem(index, selected);
}

void ScrolledSelectionList::mouseClicked(int mouseX, int mouseY)
{
}

void ScrolledSelectionList::renderScrollBackground()
{
	Tesselator& t = Tesselator::instance;
	t.begin();
	t.color(0x202020);
	t.vertexUV(m_x0, m_y1, 0.0f, m_x0 / 32.0f, (m_y1 + float(int(m_scrollAmount))) / 32.0f);
	t.vertexUV(m_x1, m_y1, 0.0f, m_x1 / 32.0f, (m_y1 + float(int(m_scrollAmount))) / 32.0f);
	t.vertexUV(m_x1, m_y0,  0.0f, m_x1 / 32.0f, (m_y0  + float(int(m_scrollAmount))) / 32.0f);
	t.vertexUV(m_x0, m_y0,  0.0f, m_x0 / 32.0f, (m_y0  + float(int(m_scrollAmount))) / 32.0f);
	t.draw();
}

void ScrolledSelectionList::checkInput(int mouseX, int mouseY)
{
	if (Mouse::isButtonDown(BUTTON_LEFT))
	{
		if (m_mouseYWhenPressed == -1.0f)
		{
			bool isValidClickArea = true;

			if (mouseY >= m_y0 && mouseY <= m_y1)
			{
				int scrollLeft = m_width / 2 + 124;
				int scrollRight = scrollLeft + 6;
				int listLeft = m_width / 2 - 110;
				int listRight = m_width / 2 + 110;
				int relativeY = transformY(mouseY);
				int hoveredItemIndex = relativeY / m_itemHeight;

				if (mouseX >= listLeft && mouseX <= listRight &&
					hoveredItemIndex >= 0 && relativeY >= 0 && hoveredItemIndex < getItemCount())
				{
					bool isDoubleClick = (hoveredItemIndex == m_lastClickedIndex &&
						getMillis() - m_lastClickTime < 250);

					onClickItem(hoveredItemIndex, mouseX - listLeft, mouseY - m_y0 + (int)m_scrollAmount - 4, isDoubleClick);
					m_lastClickedIndex = hoveredItemIndex;
					m_lastClickTime = getMillis();
				}
				else if (mouseX >= listLeft && mouseX <= listRight && relativeY < 0)
				{
					mouseClicked(mouseX - listLeft, mouseY - m_y0 + (int)m_scrollAmount - 4);
					isValidClickArea = false;
				}

				if (mouseX >= scrollLeft && mouseX <= scrollRight)
				{
					m_scrollBarGrabOffset = -1.0f;

					int totalContentHeight = getMaxPosition() - (m_y1 - m_y0 - 4);
					if (totalContentHeight < 1) totalContentHeight = 1;

					int thumbHeight = (int)((float)(m_y1 - m_y0) * (m_y1 - m_y0) / (float)getMaxPosition());
					if (thumbHeight < 32) thumbHeight = 32;
					if (thumbHeight > m_y1 - m_y0 - 8) thumbHeight = m_y1 - m_y0 - 8;

					m_scrollBarGrabOffset /= float(m_y1 - m_y0 - thumbHeight) / totalContentHeight;
				}
				else
				{
					m_scrollBarGrabOffset = 1.0f;
				}

				if (isValidClickArea)
				{
					m_mouseYWhenPressed = (float)mouseY;
				}
				else
				{
					m_mouseYWhenPressed = -2.0f;
				}
			}
			else
			{
				m_mouseYWhenPressed = -2.0f;
			}
		}
		else if (m_mouseYWhenPressed >= 0.0f)
		{
			m_scrollAmount -= (mouseY - m_mouseYWhenPressed) * m_scrollBarGrabOffset;
			m_mouseYWhenPressed = mouseY;
		}
	}
	else
	{
		m_mouseYWhenPressed = -1.0f;
	}
}

void ScrolledSelectionList::render(int mouseX, int mouseY, float f)
{
	renderBackground(f);

	int nItems = getItemCount();
	Tesselator& t = Tesselator::instance;

	checkInput(mouseX, mouseY);

	capYPosition();

	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);

	m_pMinecraft->m_pTextures->loadAndBindTexture("gui/background.png");
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	renderScrollBackground();

	int itemX = m_width / 2 - (C_ITEM_WIDTH - 4) / 2;
	int scrollY = int(m_y0 + 4 - float(int(m_scrollAmount)));

	if (m_bRenderHeader)
		renderHeader(itemX, scrollY, t);

	// Note, X/Y are the lower left's X/Y coordinates, not the upper left's.
	int lowerY = Minecraft::height - int(m_y1 / Gui::InvGuiScale);
	int upperY = Minecraft::height - int(m_y0  / Gui::InvGuiScale);
	glScissor(0, lowerY, Minecraft::width, upperY - lowerY);
	glEnable(GL_SCISSOR_TEST);

	for (int i = 0; i < nItems; i++)
	{
		float itemY = float(m_headerHeight + scrollY + i * m_itemHeight);
		if (m_y1 < itemY)
			continue;

		float lowerY = itemY + m_itemHeight - 4;
		if (lowerY < m_y0)
			continue;

		if (m_bRenderSelection && isSelectedItem(i))
		{
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glDisable(GL_TEXTURE_2D);
			t.begin();
			t.color(0x808080);
			t.vertexUV(float(m_width) / 2.0f - C_ITEM_WIDTH / 2.0f, lowerY + 2.0f, 0.0f, 0.0f, 1.0f);
			t.vertexUV(float(m_width) / 2.0f + C_ITEM_WIDTH / 2.0f, lowerY + 2.0f, 0.0f, 1.0f, 1.0f);
			t.vertexUV(float(m_width) / 2.0f + C_ITEM_WIDTH / 2.0f, itemY  - 2.0f, 0.0f, 1.0f, 0.0f);
			t.vertexUV(float(m_width) / 2.0f - C_ITEM_WIDTH / 2.0f, itemY  - 2.0f, 0.0f, 0.0f, 0.0f);
			t.color(0x000000);
			t.vertexUV(float(m_width) / 2.0f - C_ITEM_WIDTH / 2.0f + 1, lowerY + 1.0f, 0.0f, 0.0f, 1.0f);
			t.vertexUV(float(m_width) / 2.0f + C_ITEM_WIDTH / 2.0f - 1, lowerY + 1.0f, 0.0f, 1.0f, 1.0f);
			t.vertexUV(float(m_width) / 2.0f + C_ITEM_WIDTH / 2.0f - 1, itemY  - 1.0f, 0.0f, 1.0f, 0.0f);
			t.vertexUV(float(m_width) / 2.0f - C_ITEM_WIDTH / 2.0f + 1, itemY  - 1.0f, 0.0f, 0.0f, 0.0f);
			t.draw();
			glEnable(GL_TEXTURE_2D);
		}

		renderItem(i, itemX, int(itemY), int(m_itemHeight - 4.0f), t);
	}
	
	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_DEPTH_TEST);

	renderHoleBackground(0.0f, m_y0, 255, 255);
	renderHoleBackground(m_y1, float(field_1C), 255, 255);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_ALPHA_TEST);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_TEXTURE_2D);

	t.begin();
	t.color(0, 0);
	t.vertexUV(m_x0, m_y0 + 4.0f, 0.0f, 0.0f, 1.0f);
	t.vertexUV(m_x1, m_y0 + 4.0f, 0.0f, 1.0f, 1.0f);
	t.color(0, 255);
	t.vertexUV(m_x1, m_y0, 0.0f, 1.0f, 0.0f);
	t.vertexUV(m_x0, m_y0, 0.0f, 0.0f, 0.0f);
	t.draw();

	t.begin();
	t.color(0, 255);
	t.vertexUV(m_x0, m_y1, 0.0f, 0.0f, 1.0f);
	t.vertexUV(m_x1, m_y1, 0.0f, 1.0f, 1.0f);
	t.color(0, 0);
	t.vertexUV(m_x1, m_y1 - 4.0f, 0.0f, 1.0f, 0.0f);
	t.vertexUV(m_x0, m_y1 - 4.0f, 0.0f, 0.0f, 0.0f);
	t.draw();



	int var19 = getMaxPosition() - (m_y1 - m_y0 - 4);
	if (var19 > 0)
	{
		int var13 = (m_y1 - m_y0) * (m_y1 - m_y0) / getMaxPosition();
		if (var13 < 32) {
			var13 = 32;
		}

		if (var13 > m_y1 - m_y0 - 8) {
			var13 = m_y1 - m_y0 - 8;
		}

		int var14 = (int)m_scrollAmount * (m_y1 - m_y0 - var13) / var19 + m_y0;
		if (var14 < m_y0) {
			var14 = m_y0;
		}

		int var5 = m_width / 2 + 124;
		int var6 = var5 + 6;

		t.begin();
		t.color(0, 255);
		t.vertexUV(var5, m_y1, 0.0, 0.0, 1.0);
		t.vertexUV(var6, m_y1, 0.0, 1.0, 1.0);
		t.vertexUV(var6, m_y0, 0.0, 1.0, 0.0);
		t.vertexUV(var5, m_y0, 0.0, 0.0, 0.0);
		t.draw();
		t.begin();
		t.color(8421504, 255);
		t.vertexUV(var5, (var14 + var13), 0.0, 0.0, 1.0);
		t.vertexUV(var6, (var14 + var13), 0.0, 1.0, 1.0);
		t.vertexUV(var6, var14, 0.0, 1.0, 0.0);
		t.vertexUV(var5, var14, 0.0, 0.0, 0.0);
		t.draw();
		t.begin();
		t.color(12632256, 255);
		t.vertexUV(var5, (var14 + var13 - 1), 0.0, 0.0, 1.0);
		t.vertexUV((var6 - 1), (var14 + var13 - 1), 0.0, 1.0, 1.0);
		t.vertexUV((var6 - 1), var14, 0.0, 1.0, 0.0);
		t.vertexUV(var5, var14, 0.0, 0.0, 0.0);
		t.draw();
	}

	renderDecorations(mouseX, mouseY);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);
	glEnable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
}

void ScrolledSelectionList::renderHoleBackground(float a, float b, int c, int d)
{
	m_pMinecraft->m_pTextures->loadAndBindTexture("gui/background.png");

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	Tesselator& t = Tesselator::instance;
	t.begin();
	t.color(0x404040, d);
	t.vertexUV(0.0f,            b, 0.0f, 0.0f,             b / 32.0f);
	t.vertexUV(float(m_width), b, 0.0f, m_width / 32.0f, b / 32.0f);
	t.color(0x404040, c);
	t.vertexUV(float(m_width), a, 0.0f, m_width / 32.0f, a / 32.0f);
	t.vertexUV(0.0f,            a, 0.0f, 0.0f,             a / 32.0f);
	t.draw();
}

void ScrolledSelectionList::setRenderHeader(bool b, int i)
{
	m_bRenderHeader = b;
	if (!b)
		i = 0;
	m_headerHeight = i;
}
