/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ItemInHandRenderer.hpp"
#include "client/app/Minecraft.hpp"
#include "common/Mth.hpp"
#include "Lighting.hpp"
#include "entity/HumanoidMobRenderer.hpp"
#include "world/item/MapItem.hpp"

ItemInHandRenderer::ItemInHandRenderer(Minecraft* pMC) :
	m_pMinecraft(pMC),
    m_mapRenderer(pMC->m_pFont, pMC->getOptions())
{
    m_selectedItem = nullptr;
	m_lastSlot = -1;
	field_18 = 0;
	m_height = 0.0f;
	m_oHeight = 0.0f;
}

// This and itemUsed are probably leftovers from Minecraft Classic
void ItemInHandRenderer::itemPlaced()
{
	m_height = 0;
}

void ItemInHandRenderer::itemUsed()
{
	m_height = 0;
}

#ifndef USE_GL_NORMAL_LIGHTING
#define SHADE_IF_NEEDED(col) t.color(col*bright,col*bright,col*bright,1.0f)
#else
#define SHADE_IF_NEEDED(col)
#endif

void ItemInHandRenderer::renderItem(Mob* mob, std::shared_ptr<ItemInstance> inst)
{
#ifndef ORIGINAL_CODE
    if (inst->isNull())
        return;
#endif
    
    glPushMatrix();
#ifdef ENH_SHADE_HELD_TILES
    float bright = mob->getBrightness(1.0f);
#endif
    
    if (inst->m_itemID < C_MAX_TILES && TileRenderer::canRender(Tile::tiles[inst->m_itemID]->getRenderShape()))
    {
        
        m_pMinecraft->m_pTextures->loadAndBindTexture(C_TERRAIN_NAME);
        
#ifdef ENH_SHADE_HELD_TILES
#	define ARGPATCH , bright
#else
#	define ARGPATCH
#endif
        
        m_tileRenderer.renderTile(Tile::tiles[inst->m_itemID], inst->getAuxValue() ARGPATCH);
        
#ifdef ARGPATCH
#	undef ARGPATCH
#endif
        
    }
    else
    {
        std::string toBind;
        if (inst->m_itemID < C_MAX_TILES)
            toBind = C_TERRAIN_NAME;
        else
            toBind = C_ITEMS_NAME;
        m_pMinecraft->m_pTextures->loadAndBindTexture(toBind);
        
        constexpr float C_RATIO     = 1.0f / 256.0f;
        constexpr float C_RATIO_2   = 1.0f / 512.0f;
        constexpr float C_ONE_PIXEL = 1.0f / 16.0f;
        
        int icon = mob->getItemIcon(inst.get());
        int textureX = icon % 16 * 16;
        int textureY = icon / 16 * 16;
        
        float texU_1 = C_RATIO * float(textureX + 0.0f);
        float texU_2 = C_RATIO * float(textureX + 15.99f);
        float texV_1 = C_RATIO * float(textureY + 0.0f);
        float texV_2 = C_RATIO * float(textureY + 15.99f);
        
        Tesselator& t = Tesselator::instance;
        glTranslatef(-0.0f, -0.3f, 0.0f);
        glScalef(1.5f, 1.5f, 1.5f);
        glRotatef(50.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(335.0f, 0.0f, 0.0f, 1.0f);
        glTranslatef(-0.9375f, -0.0625f, 0.0f);
        
        t.begin();
        SHADE_IF_NEEDED(1.0f);
        
        t.normal(0.0f, 0.0f, 1.0f);
        t.vertexUV(0.0f, 0.0f, 0.0f,         texU_2, texV_2);
        t.vertexUV(1.0f, 0.0f, 0.0f,         texU_1, texV_2);
        t.vertexUV(1.0f, 1.0f, 0.0f,         texU_1, texV_1);
        t.vertexUV(0.0f, 1.0f, 0.0f,         texU_2, texV_1);
        
        t.normal(0.0f, 0.0f, -1.0f);
        t.vertexUV(0.0f, 1.0f, -C_ONE_PIXEL, texU_2, texV_1);
        t.vertexUV(1.0f, 1.0f, -C_ONE_PIXEL, texU_1, texV_1);
        t.vertexUV(1.0f, 0.0f, -C_ONE_PIXEL, texU_1, texV_2);
        t.vertexUV(0.0f, 0.0f, -C_ONE_PIXEL, texU_2, texV_2);
        
        SHADE_IF_NEEDED(0.8f);
        t.normal(-1.0f, 0.0f, 0.0f);
        for (int i = 0; i < 16; i++)
        {
            t.vertexUV(i * C_ONE_PIXEL, 0.0f, -C_ONE_PIXEL, Mth::lerp(texU_2, texU_1, i * C_ONE_PIXEL) - C_RATIO_2, texV_2);
            t.vertexUV(i * C_ONE_PIXEL, 0.0f, 0.0f,         Mth::lerp(texU_2, texU_1, i * C_ONE_PIXEL) - C_RATIO_2, texV_2);
            t.vertexUV(i * C_ONE_PIXEL, 1.0f, 0.0f,         Mth::lerp(texU_2, texU_1, i * C_ONE_PIXEL) - C_RATIO_2, texV_1);
            t.vertexUV(i * C_ONE_PIXEL, 1.0f, -C_ONE_PIXEL, Mth::lerp(texU_2, texU_1, i * C_ONE_PIXEL) - C_RATIO_2, texV_1);
        }
        for (int i = 0; i < 16; i++)
        {
            t.vertexUV((i + 1) * C_ONE_PIXEL, 1.0f, -C_ONE_PIXEL, Mth::lerp(texU_2, texU_1, i * C_ONE_PIXEL) - C_RATIO_2, texV_1);
            t.vertexUV((i + 1) * C_ONE_PIXEL, 1.0f, 0.0f,         Mth::lerp(texU_2, texU_1, i * C_ONE_PIXEL) - C_RATIO_2, texV_1);
            t.vertexUV((i + 1) * C_ONE_PIXEL, 0.0f, 0.0f,         Mth::lerp(texU_2, texU_1, i * C_ONE_PIXEL) - C_RATIO_2, texV_2);
            t.vertexUV((i + 1) * C_ONE_PIXEL, 0.0f, -C_ONE_PIXEL, Mth::lerp(texU_2, texU_1, i * C_ONE_PIXEL) - C_RATIO_2, texV_2);
        }
        
        SHADE_IF_NEEDED(0.6f);
        for (int i = 0; i < 16; i++)
        {
            t.vertexUV(0.0f, (i + 1) * C_ONE_PIXEL, 0.0f,         texU_2, Mth::lerp(texV_2, texV_1, i * C_ONE_PIXEL));
            t.vertexUV(1.0f, (i + 1) * C_ONE_PIXEL, 0.0f,         texU_1, Mth::lerp(texV_2, texV_1, i * C_ONE_PIXEL));
            t.vertexUV(1.0f, (i + 1) * C_ONE_PIXEL, -C_ONE_PIXEL, texU_1, Mth::lerp(texV_2, texV_1, i * C_ONE_PIXEL));
            t.vertexUV(0.0f, (i + 1) * C_ONE_PIXEL, -C_ONE_PIXEL, texU_2, Mth::lerp(texV_2, texV_1, i * C_ONE_PIXEL));
        }
        for (int i = 0; i < 16; i++)
        {
            t.vertexUV(1.0f, i * C_ONE_PIXEL, 0.0f,         texU_1, Mth::lerp(texV_2, texV_1, i * C_ONE_PIXEL));
            t.vertexUV(0.0f, i * C_ONE_PIXEL, 0.0f,         texU_2, Mth::lerp(texV_2, texV_1, i * C_ONE_PIXEL));
            t.vertexUV(0.0f, i * C_ONE_PIXEL, -C_ONE_PIXEL, texU_2, Mth::lerp(texV_2, texV_1, i * C_ONE_PIXEL));
            t.vertexUV(1.0f, i * C_ONE_PIXEL, -C_ONE_PIXEL, texU_1, Mth::lerp(texV_2, texV_1, i * C_ONE_PIXEL));
        }
        
        t.draw();
    }
    
	glPopMatrix();
}

void ItemInHandRenderer::render(float f)
{
	auto& pLP = m_pMinecraft->m_pPlayer;

	float h = m_oHeight + (m_height - m_oHeight) * f;
	glPushMatrix();
	float rotX = pLP->m_rotPrev.x + (pLP->m_rot.x - pLP->m_rotPrev.x) * f;
	glRotatef(rotX, 1.0f, 0.0f, 0.0f);
	glRotatef(pLP->m_rotPrev.y + (pLP->m_rot.y - pLP->m_rotPrev.y) * f, 0.0f, 1.0f, 0.0f);
    Lighting::turnOn(); // must be called before glPopMatrix()
	glPopMatrix();

	if (m_pMinecraft->getOptions()->m_bDynamicHand.get() && m_pMinecraft->m_pCameraEntity == pLP)
	{
		float rYaw   = Mth::lerp(pLP->m_lastRenderArmRot.y, pLP->m_renderArmRot.y, f);
		float rPitch = Mth::lerp(pLP->m_lastRenderArmRot.x, pLP->m_renderArmRot.x, f);
		glRotatef((pLP->m_rot.x - rPitch) * 0.1f, 1.0f, 0.0f, 0.0f);
		glRotatef((pLP->m_rot.y - rYaw) * 0.1f, 0.0f, 1.0f, 0.0f);
	}

    std::shared_ptr<ItemInstance> pItem = m_selectedItem;

    float fBright = pLP->getBrightness(1.0f);
    glColor4f(fBright, fBright, fBright, 1.0f);
    
    glPushMatrix();
    
    float swing2, swing3;
    float fAnim = pLP->getAttackAnim(f);
    constexpr float d = 0.8f;
    
	if (pItem && pItem->m_itemID == Item::map->m_itemID)
	{
		glPushMatrix();
		float var14 = 0.8F;
		float var9 = Mth::sin(fAnim * M_PI);
		float var10 = Mth::sin(Mth::sqrt(fAnim) * M_PI);
		glTranslatef(-var10 * 0.4F, Mth::sin(Mth::sqrt(fAnim) * M_PI * 2.0F) * 0.2F, -var9 * 0.2F);
		fAnim = Mth::clamp(1.0F - rotX / 45.0F + 0.1F, 0.0f, 1.0f);

		fAnim = -Mth::cos(fAnim * M_PI) * 0.5F + 0.5F;
		glTranslatef(0.0F, 0.0F * var14 - (1.0F - h) * 1.2F - fAnim * 0.5F + 0.04F, -0.9F * var14);
		glRotatef(90.0F, 0.0F, 1.0F, 0.0F);
		glRotatef(fAnim * -85.0F, 0.0F, 0.0F, 1.0F);
		glEnable(GL_RESCALE_NORMAL);
		m_pMinecraft->m_pTextures->loadAndBindTexture("mob/char.png");

		for (int var17 = 0; var17 < 2; ++var17)
		{
			int var21 = var17 * 2 - 1;
			glPushMatrix();
			glTranslatef(-0.0F, -0.6F, 1.1F * (float)var21);
			glRotatef((float)(-45 * var21), 1.0F, 0.0F, 0.0F);
			glRotatef(-90.0F, 0.0F, 0.0F, 1.0F);
			glRotatef(59.0F, 0.0F, 0.0F, 1.0F);
			glRotatef((float)(-65 * var21), 0.0F, 1.0F, 0.0F);
			HumanoidMobRenderer* pRenderer = (HumanoidMobRenderer*)EntityRenderDispatcher::getInstance()->getRenderer(pLP.get());
			float var13 = 1.0F;
			glScalef(var13, var13, var13);
			pRenderer->renderHand();
			glPopMatrix();
		}

		var9 = pLP->getAttackAnim(f);
		var10 = Mth::sin(var9 * var9 * M_PI);
		float var18 = Mth::sin(Mth::sqrt(var9) * M_PI);
		glRotatef(-var10 * 20.0F, 0.0F, 1.0F, 0.0F);
		glRotatef(-var18 * 20.0F, 0.0F, 0.0F, 1.0F);
		glRotatef(-var18 * 80.0F, 1.0F, 0.0F, 0.0F);
		var9 = 0.38F;
		glScalef(var9, var9, var9);
		glRotatef(90.0F, 0.0F, 1.0F, 0.0F);
		glRotatef(180.0F, 0.0F, 0.0F, 1.0F);
		glTranslatef(-1.0F, -1.0F, 0.0F);
		var10 = 0.015625F;
		glScalef(var10, var10, var10);
		m_pMinecraft->m_pTextures->loadAndBindTexture("misc/mapbg.png");
		Tesselator& var19 = Tesselator::instance;
		glNormal3f(0.0F, 0.0F, -1.0F);
		var19.begin();
		uint8_t var20 = 7;
		var19.vertexUV((0 - var20), (128 + var20), 0.0, 0.0, 1.0);
		var19.vertexUV((128 + var20), (128 + var20), 0.0, 1.0, 1.0);
		var19.vertexUV((128 + var20), (0 - var20), 0.0, 1.0, 0.0);
		var19.vertexUV((0 - var20), (0 - var20), 0.0, 0.0, 0.0);
		var19.draw();
		m_mapRenderer.render(m_pMinecraft->m_pPlayer, m_pMinecraft->m_pTextures, Item::map->getMapSavedData(pItem, m_pMinecraft->m_pLevel));
		glPopMatrix();
	}
	else if (pItem)
	{
        glTranslatef(-0.4f * Mth::sin(float(M_PI) * Mth::sqrt(fAnim)), 0.2f * Mth::sin(2.0f * float(M_PI) * Mth::sqrt(fAnim)), -0.2f * Mth::sin(float(M_PI) * fAnim));
        glTranslatef(0.7f * d, -0.65f * d - (1.0f - h) * 0.6f, -0.9f * d);
        glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
        glEnable(GL_RESCALE_NORMAL);

        swing3 = Mth::sin(float(M_PI) * fAnim * fAnim);
        swing2 = Mth::sin(float(M_PI) * Mth::sqrt(fAnim));

        glRotatef(swing3 * -20.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(swing2 * -20.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(swing2 * -80.0f, 1.0f, 0.0f, 0.0f);
        glScalef(0.4f, 0.4f, 0.4f);

        if (pItem->getItem()->isMirroredArt())
            glRotatef(180.0f, 0.0f, 1.0f, 0.0f);

        renderItem(m_pMinecraft->m_pPlayer.get(), pItem);
	}
	else
	{
        glTranslatef(-0.3f * Mth::sin(float(M_PI) * Mth::sqrt(fAnim)), 0.4f * Mth::sin(2.0f * float(M_PI) * Mth::sqrt(fAnim)), -0.4f * Mth::sin(float(M_PI) * fAnim));
        glTranslatef(0.8f * d, -0.75f * d - (1.0f - h) * 0.6f, -0.9f * d);
        glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
        glEnable(GL_RESCALE_NORMAL);

        glRotatef(Mth::sin(float(M_PI) * Mth::sqrt(fAnim)) * 70.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(Mth::sin(float(M_PI) * fAnim * fAnim) * -20.0f, 0.0f, 0.0f, 1.0f);

        m_pMinecraft->m_pTextures->loadAndBindTexture("mob/char.png");
        glTranslatef(-1.0f, 3.6f, 3.5f);
        glRotatef(120.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(200.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(-135.0f, 0.0f, 1.0f, 0.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        glTranslatef(5.6f, 0.0f, 0.0f);

        HumanoidMobRenderer* pRenderer = (HumanoidMobRenderer*)EntityRenderDispatcher::getInstance()->getRenderer(pLP.get());
        swing2 = 1.0f;
        glScalef(swing2, swing2, swing2);
        pRenderer->renderHand();
	}

    glPopMatrix();
	glDisable(GL_RESCALE_NORMAL);
	Lighting::turnOff();
}

void ItemInHandRenderer::renderFire(float f)
{
	glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (int i = 0; i < 2; i++)
	{
		glPushMatrix();
		int texture = Tile::fire->m_TextureFrame + i * 16;

		float texX = 16.0f * float(texture % 16), texY = 16.0f * float(texture / 16);
		float texU_1 =  texX           / 256.0f;
		float texU_2 = (texX + 15.99f) / 256.0f;
		float texV_1 =  texY           / 256.0f;
		float texV_2 = (texY + 15.99f) / 256.0f;

		glTranslatef(float(-(i * 2 - 1)) * 0.24f, -0.3f, 0.0f);
		glRotatef(float(-(i * 2 - 1)) * 10.0f, 0.0f, 1.0f, 0.0f);

		Tesselator& t = Tesselator::instance;
		t.begin();
		t.vertexUV(-0.5f, -0.5f, -0.5f, texU_2, texV_2);
		t.vertexUV(+0.5f, -0.5f, -0.5f, texU_1, texV_2);
		t.vertexUV(+0.5f, +0.5f, -0.5f, texU_1, texV_1);
		t.vertexUV(-0.5f, +0.5f, -0.5f, texU_2, texV_1);
		t.draw();

		glPopMatrix();
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_BLEND);
}

void ItemInHandRenderer::renderTex(float f, int texture)
{
	//float brightness = m_pMinecraft->m_pLocalPlayer->getBrightness(f);
	glColor4f(0.1f, 0.1f, 0.1f, 0.5f);
	glPushMatrix();

	// @BUG: The texture x/y isn't multiplied by 16. This causes some weird textures to show up instead of the correct ones.
#ifdef ORIGINAL_CODE
#  define MULT
#else
#  define MULT 16 *
#endif
	float texX = MULT float(texture % 16), texY = MULT float(texture / 16);
	float texU_1 =  texX           / 256.0f - 1 / 128.0f;
	float texU_2 = (texX + 15.99f) / 256.0f + 1 / 128.0f;
	float texV_1 =  texY           / 256.0f - 1 / 128.0f;
	float texV_2 = (texY + 15.99f) / 256.0f + 1 / 128.0f;

	Tesselator& t = Tesselator::instance;
	t.begin();
	t.vertexUV(-1.0f, -1.0f, -0.5f, texU_2, texV_2);
	t.vertexUV(+1.0f, -1.0f, -0.5f, texU_1, texV_2);
	t.vertexUV(+1.0f, +1.0f, -0.5f, texU_1, texV_1);
	t.vertexUV(-1.0f, +1.0f, -0.5f, texU_2, texV_1);
	t.draw();

	glPopMatrix();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void ItemInHandRenderer::tick()
{
	m_oHeight = m_height;

	auto item = m_pMinecraft->m_pPlayer->m_pInventory->getSelectedItem();

    bool canChange = m_pMinecraft->m_pPlayer->m_pInventory->m_selected == m_lastSlot && item == m_selectedItem;

    if (!item && !m_selectedItem) canChange = true;

    if (item && m_selectedItem && item != m_selectedItem && item->m_itemID == m_selectedItem->m_itemID && item->getAuxValue() == m_selectedItem->getAuxValue())
    {
        canChange = true;
        m_selectedItem = item;
    }

    float b = canChange ? 1.0f : 0.0f;

	float a = b - m_height;
	if (a < -0.4f)
		a = -0.4f;
	if (a >= 0.4f)
		a  = 0.4f;

	m_height += a;

    if (m_height < 0.1f)
    {
        m_selectedItem = item;
        m_lastSlot = m_pMinecraft->m_pPlayer->m_pInventory->m_selected;
    }
}

void ItemInHandRenderer::turn(const Vec2& rot)
{
}

void ItemInHandRenderer::renderScreenEffect(float f)
{
	glDisable(GL_ALPHA_TEST);

	auto& player = m_pMinecraft->m_pPlayer;
	Textures* textures = m_pMinecraft->m_pTextures;
	Level* level = m_pMinecraft->m_pLevel;

	if (player->isOnFire())
	{
		textures->loadAndBindTexture(C_TERRAIN_NAME);
		renderFire(f);
	}

	if (player->isInWall() && !m_pMinecraft->getOptions()->m_bFlyCheat)
	{
		textures->loadAndBindTexture(C_TERRAIN_NAME);
		
		Tile* pTile = Tile::tiles[level->getTile(player->m_pos.add(0, player->m_heightOffset, 0))];
		if (pTile)
		{
			int texture = pTile->getTexture(Facing::NORTH);
			renderTex(f, texture);
		}
	}

	glEnable(GL_ALPHA_TEST);
}

