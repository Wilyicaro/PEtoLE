/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Screen.hpp"

bool Screen::_isPanoramaAvailable = false;

Screen::Screen()
{
	m_width = 1;
	m_height = 1;
	field_10 = false;
	m_tabButtonIndex = 0;
	m_pClickedButton = 0;
	m_yOffset = 0;
	m_pFont = nullptr;
	m_pMinecraft = nullptr;
}

Screen::~Screen()
{
	m_pClickedButton = nullptr;
	m_buttons.clear();
}

void Screen::init(Minecraft* pMinecraft, int a3, int a4)
{
	m_width  = a3;
	m_height = a4;
	m_pMinecraft = pMinecraft;
	m_pFont = pMinecraft->m_pFont;
	init();
	updateTabButtonSelection();
}

void Screen::keyPressed(int key)
{
	if (m_pMinecraft->getOptions()->isKey(KM_MENU_CANCEL, key))
	{
		onClose();
	}

	if (m_pMinecraft->useController() && m_buttonTabList.size() > 0)
	{
		if (m_pMinecraft->getOptions()->isKey(KM_MENU_NEXT, key))
		{
			m_tabButtonIndex++;
			if (m_tabButtonIndex == int(m_buttonTabList.size()))
				m_tabButtonIndex = 0;
		}
		if (m_pMinecraft->getOptions()->isKey(KM_MENU_PREVIOUS, key))
		{
			m_tabButtonIndex--;
			if (m_tabButtonIndex == -1)
				m_tabButtonIndex = int(m_buttonTabList.size() - 1);
		}
		if (m_pMinecraft->getOptions()->isKey(KM_MENU_OK, key))
		{
			if (m_buttonTabList[m_tabButtonIndex]->m_bEnabled)
			{
				m_pMinecraft->m_pSoundEngine->playUI("random.click");
				buttonClicked(m_buttonTabList[m_tabButtonIndex]);
			}
		}

		updateTabButtonSelection();
	}
	
	for (int i = 0; i < int(m_textInputs.size()); i++)
	{
		TextInputBox* textInput = m_textInputs[i];
		textInput->keyPressed(key);
	}
}

void Screen::keyboardNewChar(char chr)
{
	for (int i = 0; i < int(m_textInputs.size()); i++)
	{
		TextInputBox* textInput = m_textInputs[i];
		textInput->charPressed(chr);
	}
}

static const char* g_panoramaList[] =
{
	"title/bg/panorama0.png",
	"title/bg/panorama1.png",
	"title/bg/panorama2.png",
	"title/bg/panorama3.png",
	"title/bg/panorama4.png",
	"title/bg/panorama5.png",
};

static float g_panoramaAngle = 0.0f;

void Screen::renderPanorama(float f)
{
	glViewport(0, 0, 256, 256);
	renderPanoramaBackground(f);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_2D);
	for (int i = 0; i < 8; i++)
		renderPanoramaBlur(f);
	glViewport(0, 0, Minecraft::width, Minecraft::height);
	Tesselator& var4 = Tesselator::instance;
	var4.begin();
	float var5 = m_width > m_height ? 120.0F / m_width : 120.0F / m_height;
	float var6 = m_height * var5 / 256.0F;
	float var7 = m_width * var5 / 256.0F;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	var4.color(1.0F, 1.0F, 1.0F, 1.0F);
	var4.vertexUV(0.0, m_height, zLevel, (0.5F - var6), (0.5F + var7));
	var4.vertexUV(m_width, m_height,zLevel, (0.5F - var6), (0.5F - var7));
	var4.vertexUV(m_width, 0.0, zLevel, (0.5F + var6), (0.5F - var7));
	var4.vertexUV(0.0, 0.0,zLevel, (0.5F + var6), (0.5F + var7));
	var4.draw();
}

void Screen::renderMenuBackground(float f)
{
	if (!m_pMinecraft->getOptions()->m_bMenuPanorama.get() || !isMenuPanoramaAvailable())
	{
		renderDirtBackground(0);
		return;
	}

	renderPanorama(f);
	fillGradient(0, 0, m_width, m_height, 0x80FFFFFF, 0x00FFFFFF);
	fillGradient(0, 0, m_width, m_height, 0x00000000, 0x80000000);
}

void Screen::renderPanoramaBackground(float f)
{
	Tesselator& t = Tesselator::instance;

	float aspectRatio;

	aspectRatio = 1.0f;
	//aspectRatio = float(m_width) / float(m_height);

	// not in 0.8
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(120.0f, aspectRatio, 0.05f, 10.0f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
	glEnable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_CULL_FACE);
	glDepthMask(false);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (int j = 0; j < 64; j++)
	{
		glPushMatrix();
		glTranslatef(((j % 8) / 8.0f - 0.5F) / 64.0F, ((j / 8) / 8.0f - 0.5F) / 64.0F, 0);
		glRotatef(Mth::sin((f + g_panoramaAngle) / 400.0f) * 25.0f + 20.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(-0.1f * (f + g_panoramaAngle), 0.0f, 1.0f, 0.0f);
		for (int i = 0; i < 6; i++)
		{
			glPushMatrix();

			float xm = 0.0f, ym = 0.0f, ang = 0.0f;
			switch (i)
			{
			case 1:
				ang = 90.0f;
				xm = 0.0f;
				ym = 1.0f;
				break;
			case 2:
				ang = 180.0f;
				xm = 0.0f;
				ym = 1.0f;
				break;
			case 3:
				ang = -90.0f;
				xm = 0.0f;
				ym = 1.0f;
				break;
			case 4:
				ang = 90.0f;
				ym = 0.0f;
				xm = 1.0f;
				break;
			case 5:
				ang = -90.0f;
				ym = 0.0f;
				xm = 1.0f;
				break;
			default:
				goto skip_rotate;
			}

			glRotatef(ang, xm, ym, 0.0f);

		skip_rotate:
			glBindTexture(GL_TEXTURE_2D, m_pMinecraft->m_pTextures->loadTexture(std::string(g_panoramaList[i])));

			t.begin();
			t.color(0xFFFFFF, 255 / (j + 1));
			t.vertexUV(-1.0f, -1.0f, 1.0f, 0.0f, 0.0f);
			t.vertexUV(+1.0f, -1.0f, 1.0f, 1.0f, 0.0f);
			t.vertexUV(+1.0f, +1.0f, 1.0f, 1.0f, 1.0f);
			t.vertexUV(-1.0f, +1.0f, 1.0f, 0.0f, 1.0f);
			t.draw();

			glPopMatrix();
		}
		glPopMatrix();
		glColorMask(true, true, true, false);
	}

	t.offset(0.0f, 0.0f, 0.0f);
	glColorMask(true, true, true, true);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glDepthMask(true);
	glEnable(GL_CULL_FACE);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_DEPTH_TEST);
}

void Screen::renderPanoramaBlur(float f)
{
	glBindTexture(GL_TEXTURE_2D, m_pMinecraft->m_pTextures->loadBlankTexture("panorama_blur", 256, 256));
#if defined(USE_GLES1_COMPATIBILITY_LAYER) && USE_GLES1_COMPATIBILITY_LAYER
	static uint32_t buffer[256 * 256 * 4];
	glReadPixels(0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 256,
		GL_RGBA, GL_UNSIGNED_BYTE, buffer);
#else
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, 256, 256);
#endif
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColorMask(true, true, true, false);
	Tesselator& var2 = Tesselator::instance;
	var2.begin();

	for (int var4 = 0; var4 < 3; ++var4)
	{
		var2.color(1.0F, 1.0F, 1.0F, 1.0F / (float)(var4 + 1));
		int var5 = m_width;
		int var6 = m_height;
		float var7 = (float)(var4 - 3 / 2) / 256.0F;
		var2.vertexUV(var5, var6, zLevel, (0.0F + var7), 0.0);
		var2.vertexUV(var5, 0.0, zLevel, (1.0F + var7), 0.0);
		var2.vertexUV(0.0, 0.0, zLevel, (1.0F + var7), 1.0);
		var2.vertexUV(0.0, var6, zLevel, (0.0F + var7), 1.0);
	}

	var2.draw();
	glColorMask(true, true, true, true);
}

void Screen::mouseDragged(double x, double y, int button, double deltaX, double deltaY)
{
}

void Screen::mouseScrolled(double x, double y, int scroll)
{
}

void Screen::mouseClicked(int xPos, int yPos, int d)
{
	if (!d) return;
	
	for (int i = 0; i < int(m_buttons.size()); i++)
	{
		Button* button = m_buttons[i];
		if (button->clicked(m_pMinecraft, xPos, yPos))
		{
			m_pClickedButton = button;

			if (!m_pMinecraft->isTouchscreen())
			{
				m_pMinecraft->m_pSoundEngine->playUI("random.click");
				buttonClicked(button);
			}
		}
	}

#ifndef ORIGINAL_CODE
	// Iterate over focused text inputs first. This is because if changing
	// focus, the previous focus must hide the OSK, before the new focus
	// shows it.
	for (int phase = 0; phase < 2; phase++)
	{
		bool handleFocused = phase == 0;
		for (int i = 0; i < int(m_textInputs.size()); i++)
		{
			TextInputBox* textInput = m_textInputs[i];
			if (textInput->m_bFocused == handleFocused)
			{
				textInput->onClick(xPos, yPos);
			}
		}
	}

#ifdef USE_NATIVE_ANDROID
	// if the keyboard is shown:
	if (m_pMinecraft->platform()->getKeyboardUpOffset())
	{
		// if there are none focused at the moment:
		bool areAnyFocused = false;
		for (int i = 0; i < int(m_textInputs.size()); i++)
		{
			TextInputBox* textInput = m_textInputs[i];
			if (textInput->m_bFocused)
			{
				areAnyFocused = true;
				break;
			}
		}

		if (!areAnyFocused)
			m_pMinecraft->platform()->hideKeyboard();
	}
#endif
#endif
}

void Screen::mouseReleased(int xPos, int yPos, int d)
{
	if (!d) return;

	if (m_pClickedButton)
	{
		if (m_pMinecraft->isTouchscreen() && m_pClickedButton->clicked(m_pMinecraft, xPos, yPos))
		{
			m_pMinecraft->m_pSoundEngine->playUI("random.click");
			buttonClicked(m_pClickedButton);
		}
		m_pClickedButton->released(xPos, yPos);
		m_pClickedButton = nullptr;
	}
}

void Screen::render(int xPos, int yPos, float unused)
{
	for (int i = 0; i < int(m_buttons.size()); i++)
	{
		Button* button = m_buttons[i];
		button->render(m_pMinecraft, xPos, yPos);
	}

#ifndef ORIGINAL_CODE
	for (int i = 0; i < int(m_textInputs.size()); i++)
	{
		TextInputBox* textInput = m_textInputs[i];
		textInput->tick();
		textInput->render();
	}
#endif
}

void Screen::onClose()
{
	m_pMinecraft->setScreen(nullptr);
}

void Screen::tick()
{
	g_panoramaAngle++;
}

void Screen::setSize(int width, int height)
{
	m_width = width;
	m_height = height;

	// not original code. Will need to re-init again
	m_buttons.clear();
	m_buttonTabList.clear();
	m_textInputs.clear();
	init();
}

void Screen::onRender(int mouseX, int mouseY, float f)
{
	m_yOffset = getYOffset();
	if (m_yOffset != 0) {
		// push the entire screen up
		glPushMatrix();
		glTranslatef(0.0f, -float(m_yOffset), 0.0f);
	}

	render(mouseX, mouseY, f);

	if (m_yOffset != 0)
		glPopMatrix();
}

int Screen::getYOffset()
{
#ifdef USE_NATIVE_ANDROID
	int keybOffset = m_pMinecraft->platform()->getKeyboardUpOffset();
	if (!keybOffset)
		return 0;

	int offset = 0;

	// look through every text box, see if one's open
	// and determine its offset from there
	for (int i = 0; i < int(m_textInputs.size()); i++)
	{
		TextInputBox* pBox = m_textInputs[i];

		if (!pBox->m_bFocused)
			continue;
		
		int heightLeft = m_height - int(float(keybOffset) * Gui::InvGuiScale);

		// we want to keep the center of the text box in the center of the screen
		int textCenterY = pBox->m_yPos + pBox->m_height / 2;
		int scrnCenterY = heightLeft / 2;

		int diff = textCenterY - scrnCenterY;
		
		// Prevent the difference from revealing the outside of the screen.
		if (diff > m_height - heightLeft)
			diff = m_height - heightLeft;
		if (diff < 0)
			diff = 0;

		offset = diff;
		break;
	}

	return offset;
#else
	return 0;
#endif
}

void Screen::updateEvents()
{
	if (field_10) return;

	for (int i = 1; i < MouseButtonType::BUTTON_COUNT; i++)
	{
		if (Mouse::getButtonState((MouseButtonType)i)) 
		{
			int dx, dy;
			m_pMinecraft->m_pPlatform->getMouseDiff(dx, dy);
			mouseDragged((double) m_width * Mouse::getX() / Minecraft::width, (double) m_height * Mouse::getY() / Minecraft::height + getYOffset(), i, (double)m_width * dx / Minecraft::width, (double)m_height * dy / Minecraft::height);
		}
	}

	while (Mouse::next())
		mouseEvent();


	while (Keyboard::next())
		keyboardEvent();
}

void Screen::keyboardEvent()
{
	if (Keyboard::getEventKeyState())
		keyPressed(Keyboard::getEventKey());
}

void Screen::mouseEvent()
{
	MouseAction* pAction = Mouse::getEvent();
	if (pAction->isButton())
	{
		if (Mouse::getEventButtonState())
			mouseClicked(m_width * pAction->_posX / Minecraft::width, m_height * pAction->_posY / Minecraft::height + getYOffset(), Mouse::getEventButton());
		else
			mouseReleased(m_width * pAction->_posX / Minecraft::width, m_height * pAction->_posY / Minecraft::height + getYOffset(), Mouse::getEventButton());
	} 
	else
	{
		mouseScrolled(m_width * pAction->_posX / Minecraft::width, m_height * pAction->_posY / Minecraft::height + getYOffset(), Mouse::getEventButtonState() ? -1 : 1);
	}
}

void Screen::renderBackground(int unk)
{
	if (m_pMinecraft->isLevelReady())
	{
		// draw the background offset by the Y offset so that the smaller virtual
		// keyboards don't reveal undrawn areas
		fillGradient(0, m_yOffset, m_width, m_height, 0xC0101010, 0xD0101010);
	}
	else
	{
		renderDirtBackground(unk);
	}
}

void Screen::renderBackground()
{
	renderBackground(0);
}

void Screen::renderDirtBackground(int unk)
{
	glDisable(GL_FOG);

	m_pMinecraft->m_pTextures->loadAndBindTexture("gui/background.png");
	glColor4f(0.25f, 0.25f, 0.25f, 1);
	blit(0, 0, 0, 0, m_width, m_height, 32, 32);

	glColor4f(1, 1, 1, 1);
}


void Screen::updateTabButtonSelection()
{
	if (m_pMinecraft->useController())
	{
		for (int i = 0; i < int(m_buttonTabList.size()); i++)
		{
			m_buttonTabList[i]->m_bHoveredOrFocused = m_tabButtonIndex == i;
		}
	}
}
