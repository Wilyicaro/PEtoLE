/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TouchscreenInput_TestFps.hpp"
#include "Multitouch.hpp"
#include "client/app/Minecraft.hpp"
#include "client/options/Options.hpp"
#include "world/entity/Player.hpp"

TouchscreenInput_TestFps::TouchscreenInput_TestFps(Minecraft* pMinecraft, Options* pOptions) :
	m_rectArea(0.0f, 0.0f, 1.0f, 1.0f),
	m_rightRectArea(0.0f, 0.0f, 1.0f, 1.0f),
	m_pOptions(pOptions),
	field_40(false),
	m_bJumpBeingHeld(false),
	m_bSneakBeingHeld(false),
	m_pMinecraft(pMinecraft),
	m_pAreaLeft(nullptr),
	m_pAreaRight(nullptr),
	m_pAreaForward(nullptr),
	m_pAreaBackward(nullptr),
	m_pAreaJump(nullptr),
	m_pAreaSneak(nullptr),
	m_pAreaForwardLeft(nullptr),
	m_pAreaForwardRight(nullptr)
{
	for (int i = 0; i < 10; i++)
		m_noNeighborUpdate[i] = 0;

	// Note! Only the first five button entries are used.
	for (int i = 0; i < 8; i++)
		m_pressedInputs[i] = 0;

	setScreenSize(Minecraft::width, Minecraft::height);
}

void TouchscreenInput_TestFps::releaseAllKeys()
{
	m_horzInput = 0.0f;
	m_vertInput = 0.0f;
	for (int i = 0; i < 5; i++)
		m_pressedInputs[i] = false;
}

void TouchscreenInput_TestFps::setKey(int eventKey, bool eventKeyState)
{
}

static void CopyCoordinateArray(int count, float* xs, float* ys, float* xd, float* yd)
{
	for (int i = 0; i < count; i++)
	{
		xd[i] = xs[i];
		yd[i] = ys[i];
	}
}

static void MultiplyCoordinateArray(int count, float* x, float* y, float xm, float ym)
{
	for (int i = 0; i < count; i++)
	{
		x[i] *= xm;
		y[i] *= ym;
	}
}

static void AddCoordinateArray(int count, float* x, float* y, float xd, float yd)
{
	for (int i = 0; i < count; i++)
	{
		x[i] += xd;
		y[i] += yd;
	}
}

static void TransformArray(int count, float* x1, float* y1, float* x2, float* y2, float xd, float yd, float xm, float ym)
{
	CopyCoordinateArray(count, x1, y1, x2, y2);
	MultiplyCoordinateArray(count, x2, y2, xm, ym);
	AddCoordinateArray(count, x2, y2, xd, yd);
}

void TouchscreenInput_TestFps::setScreenSize(int width, int height)
{
	int scaledWidth = ceilf(width * Gui::InvGuiScale);
	int scaledHeight = ceilf(height * Gui::InvGuiScale);
	m_touchAreaModel.clear();

	float widthM = Mth::min(64, scaledWidth / 10);
	float heightM = widthM;

	float x1[4], y1[4], x2[4], y2[4];

	x1[0] = 0.0f; x1[1] = widthM; x1[2] = widthM;  x1[3] = 0.0f;
	y1[0] = 0.0f; y1[1] = 0.0f;   y1[2] = heightM; y1[3] = heightM;
	x2[0] = 0.0f; x2[1] = 0.0f;   x2[2] = 0.0f;    x2[3] = 0.0f;
	y2[0] = 0.0f; y2[1] = 0.0f;   y2[2] = 0.0f;    y2[3] = 0.0f;

	float middleX = 0 + widthM;
	float middleY = scaledHeight - heightM * 2 - 0;

	float left, right, top, bottom;
	left = middleX - widthM;
	top = middleY - heightM;
	right = middleX + widthM * 2;
	bottom = middleY + heightM * 2;

	float rightSideX = scaledWidth - widthM - 8;

	m_rectArea = RectangleArea(left / Gui::InvGuiScale, top / Gui::InvGuiScale, right / Gui::InvGuiScale, bottom / Gui::InvGuiScale);
	m_rightRectArea = RectangleArea(rightSideX / Gui::InvGuiScale, middleY / Gui::InvGuiScale, (rightSideX + widthM) / Gui::InvGuiScale, (middleY + heightM) / Gui::InvGuiScale);

	TransformArray(4, x1, y1, x2, y2, middleX, middleY - heightM, 1.0f, 1.0f);
	m_pAreaForward = new PolygonArea(4, x2, y2);
	m_touchAreaModel.addArea(100 + INPUT_FORWARD, m_pAreaForward);

	TransformArray(4, x1, y1, x2, y2, middleX, middleY, 1.0f, 1.0f);
	m_pAreaJump = new PolygonArea(4, x2, y2);
	m_touchAreaModel.addArea(100 + INPUT_JUMP, m_pAreaJump);

	TransformArray(4, x1, y1, x2, y2, middleX, middleY + heightM, 1.0f, 1.0f);
	m_pAreaBackward = new PolygonArea(4, x2, y2);
	m_touchAreaModel.addArea(100 + INPUT_BACKWARD, m_pAreaBackward);

	TransformArray(4, x1, y1, x2, y2, middleX - widthM, middleY, 1.0f, 1.0f);
	m_pAreaLeft = new PolygonArea(4, x2, y2);
	m_touchAreaModel.addArea(100 + INPUT_LEFT, m_pAreaLeft);

	TransformArray(4, x1, y1, x2, y2, middleX + widthM, middleY, 1.0f, 1.0f);
	m_pAreaRight = new PolygonArea(4, x2, y2);
	m_touchAreaModel.addArea(100 + INPUT_RIGHT, m_pAreaRight);

	TransformArray(4, x1, y1, x2, y2, rightSideX, middleY, 1.0f, 1.0f);
	m_pAreaSneak = new PolygonArea(4, x2, y2);
	m_touchAreaModel.addArea(100 + INPUT_SNEAK, m_pAreaSneak);

	TransformArray(4, x1, y1, x2, y2, middleX - widthM, middleY - heightM, 1.0f, 1.0f);
	m_pAreaForwardLeft = new PolygonArea(4, x2, y2);
	m_touchAreaModel.addArea(100 + INPUT_FORWARDLEFT, m_pAreaForwardLeft);

	TransformArray(4, x1, y1, x2, y2, middleX + widthM, middleY - heightM, 1.0f, 1.0f);
	m_pAreaForwardRight = new PolygonArea(4, x2, y2);
	m_touchAreaModel.addArea(100 + INPUT_FORWARDRIGHT, m_pAreaForwardRight);

	// NOTE: We are not leaking memory! Since by default IArea's constructor sets
	// m_vertices to true, TouchAreaModel owns the pointers, so when it's destroyed,
	// so are these areas we allocated.
}

void TouchscreenInput_TestFps::tick(Player* pPlayer)
{
	if (m_pMinecraft->m_pScreen) return;

	m_horzInput = 0.0f;
	m_vertInput = 0.0f;
	m_bJumping = false;

	for (bool& value : m_pressedInputs)
		value = false;
	
	const int* activePointers;
	int activePointerCount = Multitouch::getActivePointerIds(&activePointers);

	bool bJumpPressed = false, bForwardPressed = false, bSneakPressed = false;

	for (int i = 0; i < activePointerCount; i++)
	{
		int finger = activePointers[i];
		int x = Multitouch::getX(finger);
		int y = Multitouch::getY(finger);
		int pointerId = m_touchAreaModel.getPointerId(x * Gui::InvGuiScale, y * Gui::InvGuiScale, finger);

		if (pointerId > 99)
			m_pressedInputs[pointerId - 100] = true;

		if (pointerId == 100 + INPUT_SNEAK) // sneak
		{
			bSneakPressed = true;
		}

		if (pointerId == 100 + INPUT_JUMP) // jump
		{
			if (pPlayer->wasInWater())
				m_bJumping = true;
			else if (Multitouch::isPressed(finger) || pPlayer->getAbilities().m_bFlying)
				m_bJumping = true;
			else if (field_40)
			{
				pointerId = 100; // forward
				bJumpPressed = true;
				m_vertInput += 1.0f;
			}
		}

		switch (pointerId)
		{
			case 100 + INPUT_FORWARD:
				if (pPlayer->wasInWater())
					m_bJumping = true;
				else
					bForwardPressed = true;

				m_vertInput += 1.0f;
				break;

			case 100 + INPUT_FORWARDLEFT:
				if (field_40)
				{
					bForwardPressed = true;
					m_vertInput += 1.0f;
					m_horzInput += 1.0f;
				}
				break;
			
			case 100 + INPUT_FORWARDRIGHT:
				if (field_40)
				{
					bForwardPressed = true;
					m_vertInput += 1.0f;
					m_horzInput -= 1.0f;
				}
				break;

			case 100 + INPUT_BACKWARD:
				m_vertInput -= 1.0f;
				break;

			case 100 + INPUT_LEFT:
				m_horzInput += 1.0f;
				break;

			case 100 + INPUT_RIGHT:
				m_horzInput -= 1.0f;
				break;
		}
	}

	field_40 = bForwardPressed;

	if (bJumpPressed)
	{
		// Allow endless jumping while the button is held
		m_bJumping = true;
		m_bJumpBeingHeld = true;
	}
	else
	{
		m_bJumpBeingHeld = false;
	}

	if (bSneakPressed)
	{
		// Toggle sneak on button press
		if (!m_bSneakBeingHeld)
			m_bSneaking = !m_bSneaking;

		m_bSneakBeingHeld = true;
	}
	else
	{
		m_bSneakBeingHeld = false;
		if (pPlayer->getAbilities().m_bFlying)
			m_bSneaking = false;
	}

	IMoveInput::tick(pPlayer);
}

static void RenderTouchButton(Tesselator* t, PolygonArea* pArea, int srcX, int srcY)
{
	float tc[8];

	tc[0] = float(srcX) / 256.0f;
	tc[1] = float(srcY) / 256.0f;
	tc[2] = tc[0] + 48.0f / 256.0f;
	tc[3] = tc[1];
	tc[4] = tc[2];
	tc[5] = tc[1] + 48.0f / 256.0f;
	tc[6] = tc[0];
	tc[7] = tc[5];

	for (int i = 0; i < pArea->m_count; i++)
	{
		t->vertexUV(
			pArea->m_xPos[i],
			pArea->m_yPos[i],
			0.0f,
			tc[(2 * i) % 8],
			tc[(2 * i + 1) % 8]
		);
	}
}

void TouchscreenInput_TestFps::render(float f)
{
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_pMinecraft->m_pTextures->loadAndBindTexture("gui/touchscreen.png");

	Tesselator& t = Tesselator::instance;
	t.begin();

	if (field_40 && !isButtonDown(100 + INPUT_JUMP)) 
	{
		t.color(isButtonDown(100 + INPUT_FORWARDLEFT) ? 0xC0C0C0 : 0xFFFFFF, 0x80);
		RenderTouchButton(&t, m_pAreaForwardLeft, 96, 176);
		
		t.color(isButtonDown(100 + INPUT_FORWARDRIGHT) ? 0xC0C0C0 : 0xFFFFFF, 0x80);
		RenderTouchButton(&t, m_pAreaForwardRight, 144, 176);
	}

	t.color(isButtonDown(100 + INPUT_LEFT) ? 0xC0C0C0 : 0xFFFFFF, 0x80);
	RenderTouchButton(&t, m_pAreaLeft, 48, 128);

	t.color(isButtonDown(100 + INPUT_RIGHT) ? 0xC0C0C0 : 0xFFFFFF, 0x80);
	RenderTouchButton(&t, m_pAreaRight, 144, 128);

	t.color(isButtonDown(100 + INPUT_FORWARD) ? 0xC0C0C0 : 0xFFFFFF, 0x80);
	RenderTouchButton(&t, m_pAreaForward, 0, 128);

	t.color(isButtonDown(100 + INPUT_BACKWARD) ? 0xC0C0C0 : 0xFFFFFF, 0x80);
	RenderTouchButton(&t, m_pAreaBackward, 96, 128);

	t.color(isButtonDown(100 + INPUT_JUMP) ? 0xC0C0C0 : 0xFFFFFF, 0x80);
	RenderTouchButton(&t, m_pAreaJump, 0, 176);

	t.color(m_bSneaking ? 0xC0C0C0 : 0xFFFFFF, 0x80);
	RenderTouchButton(&t, m_pAreaSneak, 48, 176);

	t.draw();

	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
}

void TouchscreenInput_TestFps::addRectangleArea(std::vector<RectangleArea>& touchArea)
{
	touchArea.push_back(m_rectArea);
	touchArea.push_back(m_rightRectArea);
}

bool TouchscreenInput_TestFps::isButtonDown(int key)
{
	return m_pressedInputs[key - 100];
}
