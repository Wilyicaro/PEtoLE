/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "HumanoidMobRenderer.hpp"
#include "EntityRenderDispatcher.hpp"
#include "client/app/Minecraft.hpp"
#include "client/renderer/ItemInHandRenderer.hpp"
#include "client/renderer/TileRenderer.hpp"
#include "world/entity/Player.hpp"
#include "world/item/ItemInstance.hpp"

HumanoidMobRenderer::HumanoidMobRenderer(HumanoidModel* pModel, float f) : MobRenderer(pModel, f)
{
	m_pHumanoidModel = pModel;
	m_armorParts1 = new HumanoidModel(1.0f);
	m_armorParts2 = new HumanoidModel(0.5f);
}

bool HumanoidMobRenderer::prepareArmor(Mob* mob, int a, float b)
{
	if (!mob->isPlayer()) return false;
	Player* player = (Player*)mob;

	auto itemInstance = player->m_pInventory->getArmor(3 - a);
	if (itemInstance) {
		auto& texture = itemInstance->getItem()->getArmorTexture();
		if (!texture.empty()) {
			bindTexture(texture);
			HumanoidModel* armor = a == 2 ? m_armorParts2 : m_armorParts1;
			armor->m_head.m_bVisible = a == 0;
			//armor->m_hair.m_bVisible = a == 0;
			armor->m_body.m_bVisible = a == 1 || a == 2;
			armor->m_arm1.m_bVisible = a == 1;
			armor->m_arm2.m_bVisible = a == 1;
			armor->m_leg1.m_bVisible = a == 2 || a == 3;
			armor->m_leg2.m_bVisible = a == 2 || a == 3;
			setArmor(armor);
			return true;
		}
	}

	return false;
}

void HumanoidMobRenderer::additionalRendering(Mob* mob, float f)
{
	if (!mob->isPlayer()) return;

	Player* player = (Player*)mob;

	auto headGear = player->m_pInventory->getArmor(Item::HEAD);
	if (headGear && headGear->getItem()->m_itemID < 256) {
		glPushMatrix();
		m_pHumanoidModel->m_head.translateTo(0.0625F);
		if (TileRenderer::canRender(Tile::tiles[headGear->m_itemID]->getRenderShape())) {
			float s = 0.625F;
			glTranslatef(0.0F, -0.25F, 0.0F);
			glRotatef(180.0F, 0.0F, 1.0F, 0.0F);
			glScalef(s, -s, s);
		}

		m_pDispatcher->m_pItemInHandRenderer->renderItem(headGear);
		glPopMatrix();
	}

	int itemID = player->m_pInventory->getSelectedItemId();
	if (itemID <= 0)
		return;

	std::shared_ptr<ItemInstance> inst = player->m_pInventory->getSelectedItem();
	glPushMatrix();
	m_pHumanoidModel->m_arm1.translateTo(0.0625f);
	glTranslatef(-0.0625f, 0.4375f, 0.0625f);
	if (itemID < C_MAX_TILES && TileRenderer::canRender(Tile::tiles[itemID]->getRenderShape()))
	{
		glTranslatef(0.0f, 0.1875f, -0.3125f);
		glRotatef(20.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
		glScalef(0.375f, -0.375f, 0.375f);
	}
	else if (Item::items[itemID]->isHandEquipped())
	{
		glTranslatef(0.0f, 0.1875f, 0.0f);
		glScalef(0.625f, -0.625f, 0.625f);
		glRotatef(-100.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
	}
	else
	{
		glTranslatef(0.25f, 0.1875f, -0.1875f);
		glScalef(0.375f, 0.375f, 0.375f);
		glRotatef(60.0f, 0.0f, 0.0f, 1.0f);
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(20.0f, 0.0f, 0.0f, 1.0f);
	}

	m_pDispatcher->m_pItemInHandRenderer->renderItem(inst);
	glPopMatrix();
}

void HumanoidMobRenderer::render(Entity* entity, float x, float y, float z, float unused, float f)
{
	if (entity->isPlayer())
	{
		Player* player = (Player*)entity;
		auto item = player->getSelectedItem();
		m_armorParts1->m_bHoldingRightHand = m_armorParts2->m_bHoldingRightHand = m_pHumanoidModel->m_bHoldingRightHand = item != nullptr;
	}
	m_armorParts1->m_bSneaking = m_armorParts2->m_bSneaking = m_pHumanoidModel->m_bSneaking = entity->isSneaking();
	if (entity->isSneaking() && entity != m_pDispatcher->m_pMinecraft->m_pLocalPlayer.get()) y -= 0.125;
	MobRenderer::render(entity, x, y, z, unused, f);
	m_armorParts1->m_bSneaking = m_armorParts2->m_bSneaking = m_pHumanoidModel->m_bSneaking = false;
	m_armorParts1->m_bHoldingRightHand = m_armorParts2->m_bHoldingRightHand = m_pHumanoidModel->m_bHoldingRightHand = false;
}

void HumanoidMobRenderer::onGraphicsReset()
{
	m_pHumanoidModel->onGraphicsReset();
}

void HumanoidMobRenderer::renderHand()
{
	m_pHumanoidModel->m_attackTime = 0;
	m_pHumanoidModel->setBrightness(getBrightness(m_pDispatcher->m_pMinecraft->m_pMobPersp.get(), 1.0f));
	m_pHumanoidModel->setupAnim(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0625f);
	m_pHumanoidModel->m_arm1.render(0.0625f);
}
