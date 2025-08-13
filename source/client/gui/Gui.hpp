/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "GuiComponent.hpp"
#include "client/player/input/RectangleArea.hpp"
#include "client/app/Minecraft.hpp"
#include "common/Random.hpp"
#include "common/Utils.hpp"

class Minecraft; // in case we're included from Minecraft.hpp

struct GuiMessage
{
	std::string msg;
	int field_18;

	GuiMessage(const std::string& x, int a) : msg(x), field_18(a) {}
};

class Gui : public GuiComponent
{
private:
	void renderPumpkin(int, int);
	void renderVignette(float, int, int);
	void renderTp(float, int, int);

public:
	Gui(Minecraft* pMinecraft);

	void addMessage(const std::string& str);
	void inventoryUpdated();
	void setNowPlaying(const std::string& str);
	void render(float f, bool bHaveScreen, int mouseX, int mouseY);
	void tick();
	void renderSlot(int slot, int x, int y, float f);
	int  getSlotIdAt(int mx, int my);
	bool isInside(int mx, int my);
	void handleClick(int id, int mx, int my);
	void handleKeyPressed(int keyCode);
	void renderMessages(bool bShowAll);
	int getNumSlots();					  // Gets the number of slots in the inventory. Includes the '...' if in touch mode.
	int getNumUsableSlots();			  // Gets the number of usable slots in the inventory. Does not include the '...' if in touch mode.
	RectangleArea getRectangleArea(bool b);

public:
	static float InvGuiScale;

public:
	float m_progress;
	std::string field_C;
	std::vector<GuiMessage> m_guiMessages;
	int field_24;
	int field_28;
	int field_2C;
	Random m_random;
	Minecraft* m_pMinecraft;
	int m_tickCount;
	std::string m_overlayMessage;
	int m_overlayMessageDuration;
	bool m_bHasOverlayMessage;
	float m_tbr;
	RenderChunk m_renderChunk;
	bool field_A3C;
	bool m_bRenderMessages;
};

