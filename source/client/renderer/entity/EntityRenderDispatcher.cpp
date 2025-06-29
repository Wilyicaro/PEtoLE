/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "EntityRenderDispatcher.hpp"
#include "client/app/Minecraft.hpp"
#include "../ItemInHandRenderer.hpp"

#include "client/model/PigModel.hpp"
#include "client/model/SheepModel.hpp"
#include "client/model/CowModel.hpp"
#include "client/model/ChickenModel.hpp"
#include "client/model/CreeperModel.hpp"

EntityRenderDispatcher* EntityRenderDispatcher::instance;
Vec3 EntityRenderDispatcher::off;

EntityRenderDispatcher::EntityRenderDispatcher() :
	m_HumanoidMobRenderer(new HumanoidModel(0.0f, 0.0f), 0.5f),
	m_PigRenderer(new PigModel(0.0f), /*new PigModel(0.5f),*/ 0.7f),
	m_SheepRenderer(new SheepModel(false), new SheepModel(true), 0.7f),
	m_CowRenderer(new CowModel, 0.7f),
	m_ChickenRenderer(new ChickenModel, 0.3f),
	m_CreeperRenderer(new CreeperModel, 0.5f),
	m_arrowRenderer()
{
	m_pItemInHandRenderer = nullptr;
	m_pTextures = nullptr;
	m_pLevel = nullptr;
	m_pMinecraft = nullptr;
	m_pMob = nullptr;
	m_rot = Vec2::ZERO;
	m_pOptions = nullptr;
	m_pFont = nullptr;

	m_HumanoidMobRenderer.init(this);
	m_PigRenderer.init(this);
	m_SheepRenderer.init(this);
	m_CowRenderer.init(this);
	m_ChickenRenderer.init(this);
	m_CreeperRenderer.init(this);
	m_arrowRenderer.init(this);
	
	// TODO

	m_TntRenderer.init(this);
	m_CameraRenderer.init(this);
	m_ItemRenderer.init(this);
	m_FallingTileRenderer.init(this);
}

float EntityRenderDispatcher::distanceToSqr(const Vec3& pos)
{
	return pos.distanceToSqr(m_pos);
}

Font* EntityRenderDispatcher::getFont()
{
	return m_pFont;
}

EntityRenderDispatcher* EntityRenderDispatcher::getInstance()
{
	if(!instance)
		instance = new EntityRenderDispatcher;

	return instance;
}

EntityRenderer* EntityRenderDispatcher::getRenderer(int renderType)
{
	switch (renderType)
	{
		case RENDER_TNT:
			return &m_TntRenderer;
		case RENDER_HUMANOID:
			return &m_HumanoidMobRenderer;
		case RENDER_ITEM:
			return &m_ItemRenderer;
		case RENDER_CAMERA:
			return &m_CameraRenderer;
		case RENDER_CHICKEN:
			return &m_ChickenRenderer;
		case RENDER_COW:
			return &m_CowRenderer;
		case RENDER_PIG:
			return &m_PigRenderer;
		case RENDER_SHEEP:
			return &m_SheepRenderer;
		case RENDER_CREEPER:
			return &m_CreeperRenderer;
		case RENDER_FALLING_TILE:
			return &m_FallingTileRenderer;
		case RENDER_ARROW:
			return &m_arrowRenderer;
	}

	return nullptr;
}

EntityRenderer* EntityRenderDispatcher::getRenderer(Entity* pEnt)
{
	int renderType = pEnt->m_renderType;
	if (renderType == RENDER_DYNAMIC)
		renderType = pEnt->queryEntityRenderer();

	return getRenderer(renderType);
}

void EntityRenderDispatcher::onGraphicsReset()
{
	m_HumanoidMobRenderer.onGraphicsReset();
}

void EntityRenderDispatcher::prepare(Level* level, Textures* textures, Font* font, std::shared_ptr<Mob> mob, Options* options, float f)
{
	m_pLevel = level;
	m_pTextures = textures;
	m_pMob = mob;
	m_pFont = font;
	m_pOptions = options;
	m_rot = mob->m_rotPrev + (mob->m_rot - mob->m_rotPrev) * f;
	m_pos = mob->m_posPrev + (mob->m_pos - mob->m_posPrev) * f;
}

void EntityRenderDispatcher::render(Entity* entity, float a)
{
	Vec3 pos = Vec3(entity->m_posPrev + (entity->m_pos - entity->m_posPrev) * a);
	float yaw = entity->m_rotPrev.y + a * (entity->m_rot.y - entity->m_rotPrev.y);

	float bright = entity->getBrightness(1.0f);
	glColor4f(bright, bright, bright, 1.0f);

	render(entity, pos - off, yaw, a);
}

void EntityRenderDispatcher::render(Entity* entity, const Vec3& pos, float rot, float a, bool postRender)
{
	EntityRenderer* pRenderer = getRenderer(entity);
	if (pRenderer)
	{
		pRenderer->render(entity, pos.x, pos.y, pos.z, rot, a);
		if (postRender) pRenderer->postRender(entity, pos, rot, a);
	}
}

void EntityRenderDispatcher::setLevel(Level* level)
{
	m_pLevel = level;
}

void EntityRenderDispatcher::setMinecraft(Minecraft* mc)
{
	m_pMinecraft = mc;
}
