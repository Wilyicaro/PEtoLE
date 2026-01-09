/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "client/app/Minecraft.hpp"
#include "client/gui/screens/inventory/CreativeScreen.hpp"
#include "client/gui/screens/inventory/InventoryScreen.hpp"
#include "client/gui/screens/ChatScreen.hpp"
#include "client/renderer/entity/ItemRenderer.hpp"
#include "world/tile/PortalTile.hpp"

#ifdef _WIN32
#pragma warning(disable : 4244)
#endif

float Gui::InvGuiScale = 1.0f;

Gui::Gui(Minecraft* pMinecraft)
{
	field_C = "";
	field_24 = 0;
	field_28 = 0;
	field_2C = 0;
	m_tickCount = 0;
	m_overlayMessage = "";
	m_overlayMessageDuration = 0;
	m_bHasOverlayMessage = false;
	m_tbr = 1.0f;
	field_A3C = true;
	m_bRenderMessages = true;

	m_pMinecraft = pMinecraft;

	xglGenBuffers(1, &m_renderChunk.m_glID);
}

void Gui::addMessage(const std::string& s)
{
	// if the message contains a new line, add each line separately:
	if (s.find("\n") != std::string::npos)
	{
		std::stringstream ss(s);
		std::string line;
		while (std::getline(ss, line))
			addMessage(line);

		return;
	}

	std::string str = s;

	while (m_pMinecraft->m_pFont->width(str) > 320)
	{
		int i = 2;
		for (; i < int(str.size()); i++)
		{
			std::string sstr = str.substr(0, i);

			// this sucks
			if (m_pMinecraft->m_pFont->width(sstr) > 320)
				break;
		}

		std::string sstr = str.substr(0, i - 1);
		addMessage(sstr);
		str = str.substr(i - 1);
	}

	if (m_guiMessages.size() > 50)
	{
		m_guiMessages.erase(m_guiMessages.end());
	}

	m_guiMessages.insert(m_guiMessages.begin(), GuiMessage(str, 0));
}

void Gui::setNowPlaying(const std::string& str)
{
	m_overlayMessage = "Now playing: " + str;
	m_overlayMessageDuration = 60;
	m_bHasOverlayMessage = true;
}

void Gui::renderPumpkin(int var1, int var2)
{
	glDisable(GL_DEPTH_TEST);
	glDepthMask(false);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_ALPHA_TEST);

	m_pMinecraft->m_pTextures->setSmoothing(true);
	m_pMinecraft->m_pTextures->loadAndBindTexture("misc/pumpkinblur.png");
	m_pMinecraft->m_pTextures->setSmoothing(false);

	Tesselator& t = Tesselator::instance;
	t.begin();
	t.vertexUV(0.0f, var2, -90.0f, 0.0f, 1.0f);
	t.vertexUV(var1, var2, -90.0f, 1.0f, 1.0f);
	t.vertexUV(var1, 0.0f, -90.0f, 1.0f, 0.0f);
	t.vertexUV(0.0f, 0.0f, -90.0f, 0.0f, 0.0f);
	t.draw();

	glDepthMask(true);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}


void Gui::renderVignette(float a2, int width, int height)
{
	a2 = 1.0f - a2;
	if (a2 > 1.0f)
		a2 = 1.0f;
	if (a2 < 0.0f)
		a2 = 0.0f;

	m_tbr += ((a2 - m_tbr) * 0.01f);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(false);
	glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
	glColor4f(m_tbr, m_tbr, m_tbr, 1.0f);

	//! @BUG: No misc/vignette.png to be found in the original.
	//! This function is unused anyways
	m_pMinecraft->m_pTextures->setSmoothing(true);
	m_pMinecraft->m_pTextures->loadAndBindTexture("misc/vignette.png");
	m_pMinecraft->m_pTextures->setSmoothing(false);

	Tesselator& t = Tesselator::instance;
	t.begin();
	t.vertexUV(0.0f, height,   -90.0f, 0.0f, 1.0f);
	t.vertexUV(width,   height,   -90.0f, 1.0f, 1.0f);
	t.vertexUV(width,   0.0f, -90.0f, 1.0f, 0.0f);
	t.vertexUV(0.0f, 0.0f, -90.0f, 0.0f, 0.0f);
	t.draw();

	glDepthMask(true);
	glEnable(GL_DEPTH_TEST);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Gui::renderTp(float progress, int width, int height)
{
	progress *= progress;
	progress *= progress;
	progress = progress * 0.8F + 0.2F;
	glDisable(3008);
	glDisable(2929);
	glDepthMask(false);
	glBlendFunc(770, 771);
	glColor4f(1.0F, 1.0F, 1.0F, progress);
	m_pMinecraft->m_pTextures->loadAndBindTexture(C_TERRAIN_NAME);
	float var4 = (Tile::portal->m_TextureFrame % 16) / 16.0F;
	float var5 = (Tile::portal->m_TextureFrame / 16) / 16.0F;
	float var6 = (Tile::portal->m_TextureFrame % 16 + 1) / 16.0F;
	float var7 = (Tile::portal->m_TextureFrame / 16 + 1) / 16.0F;
	Tesselator& t = Tesselator::instance;
	t.begin();
	t.vertexUV(0.0, height, -90.0, var4, var7);
	t.vertexUV(width, height, -90.0, var6, var7);
	t.vertexUV(width, 0.0, -90.0, var6, var5);
	t.vertexUV(0.0, 0.0, -90.0, var4, var5);
	t.draw();
	glDepthMask(true);
	glEnable(2929);
	glEnable(3008);
	glColor4f(1.0F, 1.0F, 1.0F, 1.0F);
}

void Gui::inventoryUpdated()
{
	field_A3C = true;
}

void Gui::render(float f, bool bHaveScreen, int mouseX, int mouseY)
{
	Minecraft* mc = m_pMinecraft;
	GameRenderer* renderer = mc->m_pGameRenderer;

	renderer->setupGuiScreen();

	auto& player = mc->m_pPlayer;

	if (!mc->m_pLevel || !player || mc->getOptions()->m_bDontRenderGui)
		return;

	glEnable(GL_BLEND);

	int width = int(ceilf(Minecraft::width * InvGuiScale)),
		height = int(ceilf(Minecraft::height * InvGuiScale));

	if (mc->getOptions()->m_bFancyGraphics.get())
	{
		renderVignette(player->getBrightness(f), width, height);
	}
	auto headGear = player->m_pInventory->getArmor(Item::HEAD);

	if (!mc->getOptions()->m_bThirdPerson && headGear && headGear->m_itemID == Tile::pumpkin->m_ID)
		renderPumpkin(width, height);

	float var10 = m_pMinecraft->m_pPlayer->m_oPortalTime + (m_pMinecraft->m_pPlayer->m_portalTime - m_pMinecraft->m_pPlayer->m_oPortalTime) * f;
	if (var10 > 0.0F)
		renderTp(var10, width, height);

	Textures* textures = mc->m_pTextures;

	if (!mc->useSplitControls())
	{
		IInputHolder* input = mc->m_pInputHolder;
		// if needed, draw feedback

		float breakProgress = Mth::lerp(m_pMinecraft->m_pGameMode->m_lastDestroyProgress, m_pMinecraft->m_pGameMode->m_destroyProgress, f);

		// don't know about this if-structure, it feels like it'd be like
		// if (m_bFoggy >= 0.0f && breakProgress <= 0.0f)
		//     that;
		// else
		//     this;
		if (breakProgress > 0.0f || input->m_feedbackAlpha < 0.0f)
		{
			if (breakProgress > 0.0f)
			{
				float xPos = input->m_feedbackX;
				float yPos = input->m_feedbackY;

				textures->loadAndBindTexture("gui/touchscreen.png");
				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
				//glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				blit(InvGuiScale * xPos - 44.0f, InvGuiScale * yPos - 44.0f, 0, 88, 88, 88, 352, 352);

				glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);

				// note: scale starts from 4.0f
				int halfWidth = (40.0f * breakProgress + 48.0f) / 2.0f;

				blit(InvGuiScale * xPos - halfWidth, InvGuiScale * yPos - halfWidth, 88 + 44 - halfWidth, 88 + 44 - halfWidth, halfWidth * 2, halfWidth * 2, 352, 352);

				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
				//glDisable(GL_BLEND);
			}
		}
		else
		{
			float xPos = input->m_feedbackX;
			float yPos = input->m_feedbackY;

			textures->loadAndBindTexture("gui/touchscreen.png");
			glColor4f(1.0f, 1.0f, 1.0f, Mth::min(1.0f, input->m_feedbackAlpha));
			//glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			blit(InvGuiScale * xPos - 44.0f, InvGuiScale * yPos - 44.0f, 0, 88, 88, 88, 352, 352);
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			//glDisable(GL_BLEND);
		}
	}

#ifndef ENH_TRANSPARENT_HOTBAR
	glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
#else
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
#endif


	m_pMinecraft->m_pTextures->loadAndBindTexture("gui/gui.png");

	zLevel = -90.0f;

#ifdef ENH_TRANSPARENT_HOTBAR
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif

	int nSlots = getNumSlots();
	int hotbarWidth = 2 + nSlots * 20;

	// hotbar
	int cenX = width / 2;
	blit(cenX - hotbarWidth / 2, height - 22, 0, 0, Mth::min(hotbarWidth, 182), 22, 256, 256);
	if (hotbarWidth > 182)
	{
		blit(cenX - hotbarWidth / 2 + 180, height - 22, 182 - (hotbarWidth - 182 + 2), 0, hotbarWidth - 182 + 2, 22, 256, 256);
	}

	Inventory* inventory = player->m_pInventory;

	// selection mark
	blit(cenX - 1 - hotbarWidth / 2 + 20 * inventory->m_selected, height - 23, 0, 22, 24, 22, 256, 256);


	if (mc->isTouchscreen())
	{
		textures->loadAndBindTexture("gui/touchscreen.png");
		blit(cenX + hotbarWidth / 2 - 19, height - 19, 0, 0, 16, 16, 256, 256);
		blit(cenX - 8, 0, 16, 0, 16, 16, 256, 256);
	}
#ifndef ENH_TRANSPARENT_HOTBAR
	//glEnable(GL_BLEND);
#endif
	textures->loadAndBindTexture("gui/icons.png");
	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
	blit(cenX - 8, height / 2 - 8, 0, 0, 16, 16, 256, 256);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#ifndef ENH_TRANSPARENT_HOTBAR
	//glDisable(GL_BLEND);
#endif

	glDisable(GL_BLEND);

	if (mc->m_pGameMode->canHurtPlayer())
	{
		// why??
		m_random.setSeed(312871L * m_tickCount);

		bool b1 = player->m_invulnerableTime >= 10 && player->m_invulnerableTime / 3 % 2;

		int heartX = cenX - hotbarWidth / 2;
		int heartYStart = height - 32;

		int armorX = heartX + hotbarWidth - 9;

		int playerHealth = player->m_health;
		const int armor = player->m_pInventory->getArmorValue();

		for (int healthNo = 1; healthNo <= C_MAX_MOB_HEALTH; healthNo += 2)
		{
			int heartY = heartYStart;

			if (armor > 0) {
				if (healthNo < armor) {
					blit(armorX, heartY, 34, 9, 9, 9, 256, 256);
				}

				if (healthNo == armor) {
					blit(armorX, heartY, 25, 9, 9, 9, 256, 256);
				}

				if (healthNo > armor) {
					blit(armorX, heartY, 16, 9, 9, 9, 256, 256);
				}
			}

			armorX -= 8;

			if (playerHealth <= 4)
				heartY += m_random.nextInt(2);

			blit(heartX, heartY, 16 + b1 * 9, 0, 9, 9, 256, 256);

			if (b1)
			{
				if (healthNo < player->m_lastHealth)
					blit(heartX, heartY, 70, 0, 9, 9, 256, 256);
				else if (healthNo == player->m_lastHealth)
					blit(heartX, heartY, 79, 0, 9, 9, 256, 256);
			}

			if (healthNo < playerHealth)
				blit(heartX, heartY, 52, 0, 9, 9, 256, 256);
			else if (healthNo == playerHealth)
				blit(heartX, heartY, 61, 0, 9, 9, 256, 256);

			heartX += 8;

		}

		if (player->isUnderLiquid(Material::water))
		{
			int breathRaw = player->m_airCapacity;
			int breathFull  = int(ceilf((float(breathRaw - 2) * 10.0f) / 300.0f));
			int breathMeter = int(ceilf((float(breathRaw)     * 10.0f) / 300.0f)) - breathFull;

			int bubbleX = cenX - hotbarWidth / 2;
			int bubbleY = height - 41;

			for (int bubbleNo = 0; bubbleNo < breathFull + breathMeter; bubbleNo++)
			{
				if (bubbleNo < breathFull)
					blit(bubbleX, bubbleY, 16, 18, 9, 9, 256, 256);
				else
					blit(bubbleX, bubbleY, 25, 18, 9, 9, 256, 256);

				bubbleX += 8;
			}
		}
	}


	int diff = mc->isTouchscreen();

	int slotX = cenX - hotbarWidth / 2 + 3;
	for (int i = 0; i < nSlots - diff; i++)
	{
		renderSlot(i, slotX, height - 19, f);

		slotX += 20;
	}

	if (m_overlayMessageDuration > 0 && m_bHasOverlayMessage) {
		float var25 = m_overlayMessageDuration - f;
		int var16 = (int)(var25 * 256.0F / 20.0F);
		if (var16 > 255) {
			var16 = 255;
		}

		if (var16 > 0) {
			glPushMatrix();
			glTranslatef(cenX, (float)(height - 48), 0.0F);
			glEnable(3042);
			glBlendFunc(770, 771);
			m_pMinecraft->m_pFont->draw(m_overlayMessage, -(m_pMinecraft->m_pFont->width(m_overlayMessage)) / 2, -4, Mth::HSBtoRGB(var25 / 50.0F, 0.7F, 0.6F) & 16777215 + (var16 << 24));
			glDisable(3042);
			glPopMatrix();
		}
	}
	else m_bHasOverlayMessage = false;

#undef DIFF

	field_A3C = false;

	// render messages
	if (m_bRenderMessages)
	{
		renderMessages(false);
	}
}

void Gui::tick()
{
	if (m_overlayMessageDuration > 0)
		m_overlayMessageDuration--;

	m_tickCount++;

	for (int i = 0; i < int(m_guiMessages.size()); i++)
	{
		GuiMessage& msg = m_guiMessages[i];
		msg.field_18++;
	}
}

void Gui::renderSlot(int slot, int x, int y, float f)
{
	Inventory* pInv = m_pMinecraft->m_pPlayer->m_pInventory;

	std::shared_ptr<ItemInstance> pInst = pInv->getItem(slot);
	if (!pInst || pInst->m_itemID <= 0)
		return;

    float var6 = ((float)pInst->m_popTime) - f;
    if (var6 > 0.0f)
    {
        glPushMatrix();
        float var7 = 1.0f + var6 / 5.0f;
        glTranslatef(x + 8, y + 12, 0.0f);
        glScalef(1.0f / var7, (var7 + 1.0f) / 2.0f, 1.0f);
        glTranslatef(-(x + 8), -(y + 12), 0.0f);
    }

	ItemRenderer::renderGuiItem(m_pMinecraft->m_pTextures, pInst, x, y);
    if (var6 > 0.0f)
        glPopMatrix();

    ItemRenderer::renderGuiItemDecorations(m_pMinecraft->m_pFont, m_pMinecraft->m_pTextures, pInst, x, y);
}

int Gui::getSlotIdAt(int mouseX, int mouseY)
{
	int scaledY = int(InvGuiScale * mouseY);
	int scaledHeight = int(InvGuiScale * Minecraft::height);

	if (scaledY >= scaledHeight)
		return -1;
	if (scaledY < scaledHeight - 19)
		return -1;

	int hotbarOffset = getNumSlots() * 20 / 2 - 2;

	int slotX = (int(InvGuiScale * mouseX) - int(InvGuiScale * Minecraft::width) / 2 + hotbarOffset + 20) / 20;

	if (slotX >= 0)
		slotX--;

	if (slotX >= getNumSlots())
		slotX = -1;

	return slotX;
}

bool Gui::isInside(int mouseX, int mouseY)
{
	return getSlotIdAt(mouseX, mouseY) != -1;
}

void Gui::handleClick(int clickID, int mouseX, int mouseY)
{
	if (clickID != 1)
		return;

	int width = int(ceilf(Minecraft::width * InvGuiScale)),
		height = int(ceilf(Minecraft::height * InvGuiScale));

	int cenX = width / 2;

	if (m_pMinecraft->isTouchscreen() && int(InvGuiScale * mouseX) >= cenX - 8 && int(InvGuiScale * mouseX) < cenX + 8 && int(InvGuiScale * mouseY) < 16)
		m_pMinecraft->setScreen(new ChatScreen());

	int slot = getSlotIdAt(mouseX, mouseY);
	if (slot == -1)
		return;

	if (m_pMinecraft->isTouchscreen() && slot == getNumSlots() - 1)
		if (m_pMinecraft->m_pGameMode->isSurvivalType()) m_pMinecraft->setScreen(new InventoryScreen(m_pMinecraft->m_pPlayer.get()));
		else m_pMinecraft->setScreen(new CreativeScreen(m_pMinecraft->m_pPlayer->m_pInventory));
	else
		m_pMinecraft->m_pPlayer->m_pInventory->selectSlot(slot);
		
}

void Gui::handleKeyPressed(int keyCode)
{
	Options* options = m_pMinecraft->getOptions();

	if (options->isKey(KM_CRAFTING, keyCode))
	{
		if (m_pMinecraft->m_pGameMode->isSurvivalType()) m_pMinecraft->setScreen(new InventoryScreen(m_pMinecraft->m_pPlayer.get()));
		else m_pMinecraft->setScreen(new CreativeScreen(m_pMinecraft->m_pPlayer->m_pInventory));
		return;
	}

	if (options->isKey(KM_INVENTORY, keyCode))
	{
		m_pMinecraft->setScreen(new InventoryScreen(m_pMinecraft->m_pPlayer.get()));
		return;
	}

	bool slotL = options->isKey(KM_SLOT_L, keyCode);
	bool slotR = options->isKey(KM_SLOT_R, keyCode);
	if (slotL || slotR)
	{
		int maxItems = getNumSlots();
		if (m_pMinecraft->isTouchscreen())
			maxItems--;

		int selected = m_pMinecraft->m_pPlayer->m_pInventory->m_selected;
	
		m_pMinecraft->m_pPlayer->m_pInventory->selectSlot((selected + (slotR ? 1 : -1) + maxItems) % maxItems);
		return;
	}

	if (options->isKey(KM_CHAT, keyCode) || options->isKey(KM_CHAT_CMD, keyCode))
	{
		if (!m_pMinecraft->m_pScreen)
			m_pMinecraft->setScreen(new ChatScreen(m_pMinecraft->getOptions()->isKey(KM_CHAT_CMD, keyCode)));
	}
}

void Gui::renderMessages(bool bShowAll)
{
	//int width = Minecraft::width * InvGuiScale,
	int height = int(ceilf(Minecraft::height * InvGuiScale));

	int topEdge = height - 49;

	for (int i = 0; i < int(m_guiMessages.size()); i++)
	{
		GuiMessage& msg = m_guiMessages[i];
		if (!bShowAll && msg.field_18 > 199)
			continue;

		int bkgdColor = 0x7F000000, textColor = 0xFFFFFFFF;

		float fade = 1.0f;

		if (!bShowAll)
		{
			fade = 10.0f * (1.0f - (float(msg.field_18) / 200.0f));
			if (fade <= 0.0f)
				continue;

			if (fade < 1.0f)
			{
				int x = int(fade * fade * 255.0f);
				if (x == 0)
					continue;

				bkgdColor = (x / 2) << 24;
				textColor = (x << 24) + 0xFFFFFF;
			}
		}

		fill(2, topEdge, 322, topEdge + 9, bkgdColor);
		glEnable(GL_BLEND);
		m_pMinecraft->m_pFont->drawShadow(msg.msg, 2, topEdge + 1, textColor);

		topEdge -= 9;
	}

	glDisable(GL_BLEND);
}

int Gui::getNumSlots()
{
	return 9 + m_pMinecraft->isTouchscreen();
}

int Gui::getNumUsableSlots()
{
	return getNumSlots() - m_pMinecraft->isTouchscreen();
}

RectangleArea Gui::getRectangleArea(bool b)
{
	float centerX = Minecraft::width / 2;
	float hotbarWidthHalf = (10 * getNumSlots() + 5) / InvGuiScale;
	return RectangleArea(
		b ? (centerX - hotbarWidthHalf) : 0,
		Minecraft::height - 24.0f / InvGuiScale,
		centerX + hotbarWidthHalf,
		Minecraft::height);
}
