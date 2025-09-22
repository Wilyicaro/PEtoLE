/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "IngameBlockSelectionScreen.hpp"
#include "PauseScreen.hpp"
#include "client/app/Minecraft.hpp"
#include "client/renderer/entity/ItemRenderer.hpp"
#include "world/tile/Sapling.hpp"

std::string g_sNotAvailableInDemoVersion = "Not available in the demo version";

IngameBlockSelectionScreen::IngameBlockSelectionScreen() :
	m_btnPause(0, "Pause")
{
	m_selectedSlot = 0;

	addCreativeItem(Tile::stone->m_ID);
	addCreativeItem(Tile::cobblestone->m_ID);
	addCreativeItem(Tile::sandStone->m_ID);
	addCreativeItem(Tile::wood->m_ID);
	addCreativeItem(Tile::treeTrunk->m_ID);
	addCreativeItem(Tile::goldBlock->m_ID);
	addCreativeItem(Tile::ironBlock->m_ID);
	addCreativeItem(Tile::diamondBlock->m_ID);
	addCreativeItem(Tile::brick->m_ID);
	addCreativeItem(Tile::leaves->m_ID);
	for (int i = 0; i < 16; i++)
	{
		addCreativeItem(Tile::cloth->m_ID, i);
	}
	addCreativeItem(Tile::glass->m_ID);
	addCreativeItem(Tile::stairsWood->m_ID);
	addCreativeItem(Tile::stairsStone->m_ID);
	addCreativeItem(Tile::stoneSlabHalf->m_ID);
	addCreativeItem(Tile::sand->m_ID);
	addCreativeItem(Tile::ladder->m_ID);
	addCreativeItem(Tile::torch->m_ID);
	addCreativeItem(Tile::flower->m_ID);
	addCreativeItem(Tile::rose->m_ID);
	addCreativeItem(Tile::mushroom1->m_ID);
	addCreativeItem(Tile::mushroom2->m_ID);
	addCreativeItem(Tile::reeds->m_ID);
	addCreativeItem(Tile::obsidian->m_ID);
	addCreativeItem(Tile::dirt->m_ID);

	addCreativeItem(Tile::grass->m_ID);
	addCreativeItem(Tile::tnt->m_ID);
	addCreativeItem(Tile::gravel->m_ID);
	addCreativeItem(Tile::mossStone->m_ID);
	addCreativeItem(Tile::bookshelf->m_ID);
	addCreativeItem(Tile::lapisBlock->m_ID);
	addCreativeItem(Tile::sponge->m_ID);
	addCreativeItem(Tile::sapling->m_ID);
	addCreativeItem(Tile::craftingTable->m_ID);
	addCreativeItem(Tile::furnace->m_ID);
	addCreativeItem(Tile::chest->m_ID);
	addCreativeItem(Tile::cactus->m_ID);
	addCreativeItem(Tile::coalOre->m_ID);
	addCreativeItem(Tile::ironOre->m_ID);
	addCreativeItem(Tile::goldOre->m_ID);
	addCreativeItem(Tile::diamondOre->m_ID);
	addCreativeItem(Tile::lapisOre->m_ID);
	addCreativeItem(Tile::redstoneOre->m_ID);
	addCreativeItem(Tile::pumpkin->m_ID);
	addCreativeItem(Tile::pumpkinLantern->m_ID);
	addCreativeItem(Tile::netherrack->m_ID);
	addCreativeItem(Tile::soulSand->m_ID);
	addCreativeItem(Tile::glowstone->m_ID);
	addCreativeItem(Tile::fence->m_ID);
	addCreativeItem(Tile::web->m_ID);
	addCreativeItem(Tile::ice->m_ID);
	addCreativeItem(Tile::topSnow->m_ID);
	addCreativeItem(Tile::mobSpawner->m_ID);

	addCreativeItem(Tile::water->m_ID);
	addCreativeItem(Tile::lava->m_ID);
	addCreativeItem(Tile::fire->m_ID);

	addCreativeItem(Item::woodDoor->m_itemID);
	addCreativeItem(Item::ironDoor->m_itemID);
}

Inventory* IngameBlockSelectionScreen::getInventory()
{
	return m_pMinecraft->m_pPlayer->m_pInventory;
}

int IngameBlockSelectionScreen::getBottomY()
{
	// -1 for some reason, -2 to make it align between top of screen and top of hotbar instead
	return (m_height - 22 * (getSlotsHeight() - 2)) / 2;
}

int IngameBlockSelectionScreen::getSelectedSlot(int x, int y)
{
	int slotsHeight = getSlotsHeight();
	int bottom = m_height - getBottomY();
	int top = bottom - slotsHeight * 22;
	int left = m_width / 2 - 87;

	if (y < top)
		return -1;
	if (x < left)
		return -1;

	int idx = (x - left) / 20;
	if (idx > 8)
		return -1;

	return idx + 9 * slotsHeight - 9 * ((y - top) / 22);
}

int IngameBlockSelectionScreen::getSlotPosX(int x)
{
	return m_width / 2 - 88 + 20 * x;
}

int IngameBlockSelectionScreen::getSlotPosY(int y)
{
	return m_height - getBottomY() - 22 * y;
}

int IngameBlockSelectionScreen::getSlotsHeight()
{
	return (m_items.size() + 8) / 9;
}

bool IngameBlockSelectionScreen::isAllowed(int slot)
{
	return slot >= 0 && slot < m_items.size();
}

void IngameBlockSelectionScreen::init()
{
	m_btnPause.m_width = 40;
	m_btnPause.m_xPos = 0;
	m_btnPause.m_yPos = 0;
#if TARGET_OS_IPHONE != 0
	if (m_pMinecraft->isTouchscreen())
		m_buttons.push_back(&m_btnPause);
#endif
	
	if (getInventory()->getSelectedItem()) {
		int nItems = m_items.size();

		for (int i = 0; i < nItems; i++)
		{
			if (m_items.at(i)->matches(*getInventory()->getSelectedItem()))
			{
				m_selectedSlot = i;
				break;
			}
		}

		if (!isAllowed(m_selectedSlot))
			m_selectedSlot = 0;
	}
}

void IngameBlockSelectionScreen::renderSlot(int index, int x, int y, float f)
{
	std::shared_ptr<ItemInstance> pItem = m_items.at(index);

	if (pItem->isNull())
		return;

	ItemRenderer::renderGuiItem(m_pMinecraft->m_pTextures, pItem, x, y);
	ItemRenderer::renderGuiItemDecorations(m_pMinecraft->m_pFont, m_pMinecraft->m_pTextures, pItem, x, y);
}

void IngameBlockSelectionScreen::renderSlots()
{
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	m_pMinecraft->m_pTextures->loadAndBindTexture("gui/gui.png");

	for (int y = 0; y != -22 * getSlotsHeight(); y -= 22)
		blit(m_width / 2 - 182 / 2, m_height - 3 - getBottomY() + y, 0, 0, 182, 22, 256, 256);

	if (m_selectedSlot >= 0)
	{
		blit(m_width / 2 - 92 + 20 * (m_selectedSlot % 9), m_height - 4 - getBottomY() - 22 * (m_selectedSlot / 9), 0, 22, 24, 24, 256, 256);
	}

	for (int y = 0, index = 0; y < getSlotsHeight(); y++)
	{
		int posY = getSlotPosY(y);
		for (int x = 0; x < 9; x++)
		{
			if (index >= m_items.size()) break;
			int posX = getSlotPosX(x);
			renderSlot(index++, posX, posY, 0.0f);
		}
	}
}

void IngameBlockSelectionScreen::renderDemoOverlay()
{
	fill(getSlotPosX(0) - 3, getSlotPosY(1) - 3, getSlotPosX(9), getSlotPosY(-1), 0xA0000000);
	
	int x = (getSlotPosX(4) + getSlotPosX(5)) / 2;
	int y = (getSlotPosY(0) + getSlotPosY(1)) / 2 + 5;

	drawCenteredString(m_pMinecraft->m_pFont, g_sNotAvailableInDemoVersion, x, y, 0xFFFFFFFF);
}

void IngameBlockSelectionScreen::render(int x, int y, float f)
{
	Screen::render(x, y, f);
	glDisable(GL_DEPTH_TEST);

	fill(0, 0, m_width, m_height, 0x80000000);

	glEnable(GL_BLEND);
	renderSlots();

#ifdef DEMO
	renderDemoOverlay();
#endif

	glEnable(GL_DEPTH_TEST);
}

void IngameBlockSelectionScreen::buttonClicked(Button* pButton)
{
	if (pButton->m_buttonId == m_btnPause.m_buttonId)
		m_pMinecraft->setScreen(new PauseScreen);
}

void IngameBlockSelectionScreen::mouseClicked(int x, int y, int type)
{
	Screen::mouseClicked(x, y, type);
	
	// not a left click
	if (type != 1)
		return;

	int slot = getSelectedSlot(x, y);
	if (isAllowed(slot))
		m_selectedSlot = slot;
}

void IngameBlockSelectionScreen::mouseReleased(int x, int y, int type)
{
	Screen::mouseReleased(x, y, type);
	
	// not a left click
	if (type != 1)
		return;

	int slot = getSelectedSlot(x, y);
	if (isAllowed(slot) && slot == m_selectedSlot)
		selectSlotAndClose();
}

void IngameBlockSelectionScreen::removed()
{
	m_pMinecraft->m_gui.inventoryUpdated();
}

bool IngameBlockSelectionScreen::isPauseScreen() {
	return false; 
}

void IngameBlockSelectionScreen::addCreativeItem(int itemID, int auxValue)
{
	m_items.push_back(std::make_shared<ItemInstance>(itemID, 1, auxValue));
}

void IngameBlockSelectionScreen::selectSlotAndClose()
{
	Inventory* pInv = getInventory();
	
	pInv->setItem(pInv->m_selected, std::make_shared<ItemInstance>(*m_items.at(m_selectedSlot)));

	m_pMinecraft->m_pSoundEngine->playUI("random.click");
	m_pMinecraft->setScreen(nullptr);
}
