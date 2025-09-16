/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <sstream>
#include "ItemRenderer.hpp"
#include "EntityRenderDispatcher.hpp"
#include "client/renderer/TileRenderer.hpp"
#include "world/entity/ItemEntity.hpp"
#include "client/gui/GuiComponent.hpp"

TileRenderer* ItemRenderer::tileRenderer = new TileRenderer;

#ifndef ENH_3D_INVENTORY_TILES
const uint8_t g_ItemFrames[C_MAX_TILES] =
{
	0,  1,  2,  3,  4,  5,  0,  6,  0,  0,  0,  0,  7,  8,  9,  10,
	11, 12, 13, 0,  14, 15, 0,  0,  16, 0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  17, 0,  18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 0,
	0,  28, 29, 0,  0,  30, 0,  0,  31, 32, 0,  0,  33, 0,  0,  0,
	34, 35, 0,  36, 0,  0,  0,  37, 0,  38, 39, 0,  0,  0,  40, 41,
	0,  0,  42, 43, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54,
	55, 56, 57, 58, 0
};
#endif

ItemRenderer::ItemRenderer()
{
	m_shadowRadius = 0.15f;
	m_shadowStrength = 0.75f;
}

void ItemRenderer::render(Entity* pEntity, const Vec3& pos, float a, float b)
{
	m_random.setSeed(187);
	ItemEntity* pItemEntity = (ItemEntity*)pEntity;

	glPushMatrix();
	float yOffset = Mth::sin((float(pItemEntity->m_age) + b) / 10.0f + pItemEntity->m_bobOffs);
	std::shared_ptr<ItemInstance> pItemInstance = pItemEntity->m_itemInstance;

	int itemsToRender = 1;
	if (pItemInstance->m_count > 1)
		itemsToRender = 2;
	if (pItemInstance->m_count > 5)
		itemsToRender = 3;
	if (pItemInstance->m_count > 20)
		itemsToRender = 4;

	//if (pItemEntity->pickingPlayer) 
	//{
	//	float var8 = std::min(1.0f, (pItemEntity->m_pickupDelay + b) / (pItemEntity->maxPickTime + 1.0f));
	//	var8 *= var8;
	//	float var15 = Mth::Lerp(pItemEntity->pickingPlayer->m_posPrev.x, pItemEntity->pickingPlayer->m_pos.x, b);
	//	float var17 = Mth::Lerp(pItemEntity->pickingPlayer->m_posPrev.y, pItemEntity->pickingPlayer->m_pos.y, b) - 0.5;
	//	float var19 = Mth::Lerp(pItemEntity->pickingPlayer->m_posPrev.z, pItemEntity->pickingPlayer->m_pos.z, b);
	//	x += (var15 - pItemEntity->m_pos.x) * var8;
	//	y += (var17 - pItemEntity->m_pos.y) * var8;
	//	z += (var19 - pItemEntity->m_pos.z) * var8;
	//	float var30 = pItemEntity->m_pLevel->getBrightness(TilePos(x, y + (pItemEntity->m_heightOffset / 2.0F), z));
	//	glColor4f(var30, var30, var30, 1.0F);
	//}

	glTranslatef(pos.x, pos.y + 0.1f + yOffset * 0.1f, pos.z);
	glEnable(GL_RESCALE_NORMAL);

	int itemID = pItemInstance->m_itemID;
	if (itemID < C_MAX_TILES && TileRenderer::canRender(Tile::tiles[itemID]->getRenderShape()))
	{
		glRotatef(((float(pItemEntity->m_age) + b) / 20.0f + pItemEntity->m_bobOffs) * 57.296f, 0.0f, 1.0f, 0.0f);
		bindTexture(C_TERRAIN_NAME);

		float scale = 0.5f;

		if (Tile::tiles[itemID]->isCubeShaped() || pItemInstance->m_itemID == Tile::stoneSlabHalf->m_ID || Tile::tiles[itemID]->getRenderShape() == SHAPE_PISTON)
			scale = 0.25f;

		glScalef(scale, scale, scale);

		for (int i = 0; i < itemsToRender; i++)
		{
			glPushMatrix();
			if (i != 0)
			{
				glTranslatef(
					0.2f * (m_random.nextFloat() * 2.0f - 1.0f) / scale,
					0.2f * (m_random.nextFloat() * 2.0f - 1.0f) / scale,
					0.2f * (m_random.nextFloat() * 2.0f - 1.0f) / scale);
			}

			tileRenderer->renderTile(Tile::tiles[itemID], pItemInstance->getAuxValue(), getBrightness(pItemEntity, 1.0f));
			glPopMatrix();
		}
	}
	else
	{
		glScalef(0.5f, 0.5f, 0.5f);
		int icon = pItemInstance->getIcon();

		bindTexture(pItemInstance->m_itemID < C_MAX_TILES ? C_TERRAIN_NAME : C_ITEMS_NAME);

		for (int i = 0; i < itemsToRender; i++)
		{
			glPushMatrix();
			if (i != 0)
			{
				glTranslatef(
					0.2f * (m_random.nextFloat() * 2.0f - 1.0f) * 0.3f,
					0.2f * (m_random.nextFloat() * 2.0f - 1.0f) * 0.3f,
					0.2f * (m_random.nextFloat() * 2.0f - 1.0f) * 0.3f);
			}

			glRotatef(180.0f - m_pDispatcher->m_rot.y, 0.0f, 1.0f, 0.0f);

			Tesselator& t = Tesselator::instance;
			t.begin();

#ifdef ENH_SHADE_HELD_TILES
			float bright = getBrightness(pItemEntity, 1.0f);
			t.color(bright, bright, bright);
#endif
			t.normal(0.0f, 1.0f, 0.0f);
			t.vertexUV(-0.5f, -0.25f, 0.0f, float(16 * (icon % 16))     / 256.0f, float(16 * (icon / 16 + 1)) / 256.0f);
			t.vertexUV(+0.5f, -0.25f, 0.0f, float(16 * (icon % 16 + 1)) / 256.0f, float(16 * (icon / 16 + 1)) / 256.0f);
			t.vertexUV(+0.5f, +0.75f, 0.0f, float(16 * (icon % 16 + 1)) / 256.0f, float(16 * (icon / 16))     / 256.0f);
			t.vertexUV(-0.5f, +0.75f, 0.0f, float(16 * (icon % 16))     / 256.0f, float(16 * (icon / 16))     / 256.0f);
	

			t.draw();

			glPopMatrix();
		}
	}

	glDisable(GL_RESCALE_NORMAL);
	glPopMatrix();
}

void ItemRenderer::renderGuiItemDecorations(Font* font, Textures* textures, std::shared_ptr<ItemInstance> item, int x, int y) {
	if (!item) return;

	if (item->m_count > 1) {
		std::string amount = std::to_string(item->m_count);
		//glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		font->drawShadow(amount, x + 19 - 2 - font->width(amount), y + 6 + 3, 0xFFFFFF);
		//glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
	}

	if (item->isDamaged()) {
		int p = Mth::round(13.0 - (double)item->getDamageValue() * 13.0 / (double)item->getMaxDamage());
		int cc = Mth::round(255.0 - (double)item->getDamageValue() * 255.0 / (double)item->getMaxDamage());

		//glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);

		int ca = ((255 - cc) << 16) | (cc << 8);
		int cb = (((255 - cc) / 4) << 16) | 0x3F00;

		GuiComponent::drawRect(x + 2, y + 13, 13, 2, 0);
		GuiComponent::drawRect(x + 2, y + 13, 12, 1, cb);
		GuiComponent::drawRect(x + 2, y + 13, p, 1, ca);

		//glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
		glColor4f(1.0F, 1.0F, 1.0F, 1.0F);
	}
}

void ItemRenderer::renderGuiItem(Textures* textures, std::shared_ptr<ItemInstance> instance, int x, int y)
{
	// As if that actually works due to us blocking t.begin() and t.draw() calls...
	if (!instance)
		return;

	int itemID = instance->m_itemID;

	// @BUG: This is one of the reasons you can't actually hold items in early Minecraft.
	// There's an attempt to index `Tile::tiles` out of bounds, which of course fails, and likely crashes the game. :(
	// If only they'd placed the g_ItemFrames[itemID] check before the TileRenderer::canRender check...
#ifdef ORIGINAL_CODE
#define COND_PRE
#else
#define COND_PRE (0 <= itemID && itemID < C_MAX_TILES) && 
#endif

	bool bCanRenderAsIs = false;

#ifdef ENH_3D_INVENTORY_TILES
	// We don't need to care about g_ItemFrames at all since blocks will get 3D rendered and 2D props will use the terrain.png as the texture.
	if (COND_PRE(TileRenderer::canRender(Tile::tiles[itemID]->getRenderShape())))
	{
		bCanRenderAsIs = true;
	}
#else
	if (COND_PRE(TileRenderer::canRender(Tile::tiles[itemID]->getRenderShape()) || g_ItemFrames[itemID] != 0))
	{
		bCanRenderAsIs = true;
	}
#endif
	
	if (itemID < C_MAX_TILES && bCanRenderAsIs)
	{

		textures->loadAndBindTexture(C_TERRAIN_NAME);

		glPushMatrix();
		glTranslatef(x - 2, y + 3, 0.0F);
		glScalef(10.0F, 10.0F, 10.0F);
		glTranslatef(1.0F, 0.5F, 8.0F);
		glScalef(1.0F, 1.0F, -1.0F);
		glRotatef(210.0F, 1.0F, 0.0F, 0.0F);
		glRotatef(45.0F, 0.0F, 1.0F, 0.0F);
		glColor4f(1.0F, 1.0F, 1.0F, 1.0F);
		glScalef(1.0F, 1.0F, 1.0F);
		
		tileRenderer->renderTile(Tile::tiles[itemID], instance->getAuxValue(), 1.0f, true);
		#undef PARM_HACK

		glPopMatrix();

	}
	else if (instance->getIcon() >= 0)
	{
		// @BUG: The last bound texture will be the texture that ALL items will take. This is because begin and end calls
		// have been void'ed by a  t.voidBeginAndEndCalls call in Gui::render.
		if (instance->m_itemID <= 255)
			textures->loadAndBindTexture(C_TERRAIN_NAME);
		else
			textures->loadAndBindTexture(C_ITEMS_NAME);

		GuiComponent::blit(x, y, 0, 16 * (instance->getIcon() % 16), 16 * (instance->getIcon() / 16), 16, 16, 256, 256);
	}
}
