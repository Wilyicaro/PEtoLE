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
#include "client/model/SquidModel.hpp"
#include "client/model/CreeperModel.hpp"
#include "client/model/GhastModel.hpp"
#include "client/model/SlimeModel.hpp"
#include "client/model/WolfModel.hpp"
#include "client/model/ZombieModel.hpp"
#include "client/model/SkeletonModel.hpp"
#include "HumanoidMobRenderer.hpp"
#include "GiantMobRenderer.hpp"
#include "SpiderRenderer.hpp"
#include "TntRenderer.hpp"
#include "ItemRenderer.hpp"
#include "FallingTileRenderer.hpp"
#include "PigRenderer.hpp"
#include "SheepRenderer.hpp"
#include "CowRenderer.hpp"
#include "WolfRenderer.hpp"
#include "ChickenRenderer.hpp"
#include "SquidRenderer.hpp"
#include "CreeperRenderer.hpp"
#include "GhastRenderer.hpp"
#include "SlimeRenderer.hpp"
#include "ArrowRenderer.hpp"
#include "MinecartRenderer.hpp"
#include "BoatRenderer.hpp"
#include "ItemSpriteRenderer.hpp"
#include "PaintingRenderer.hpp"
#include "FishingHookRenderer.hpp"
#include "LightningBoltRenderer.hpp"

EntityRenderDispatcher* EntityRenderDispatcher::instance;
Vec3 EntityRenderDispatcher::off;

EntityRenderDispatcher::EntityRenderDispatcher()
{
	m_pItemInHandRenderer = nullptr;
	m_pTextures = nullptr;
	m_pLevel = nullptr;
	m_pMinecraft = nullptr;
	m_pMob = nullptr;
	m_rot = Vec2::ZERO;
	m_pOptions = nullptr;
	m_pFont = nullptr;

	registerRenderer(EntityType::player, new HumanoidMobRenderer(new HumanoidModel(0.0f, 0.0f), 0.5f));
	registerRenderer(EntityType::pig, new PigRenderer(new PigModel(0.0f), new PigModel(0.5f), 0.7f));
	registerRenderer(EntityType::sheep, new SheepRenderer(new SheepModel(false), new SheepModel(true), 0.7f));
	registerRenderer(EntityType::cow, new CowRenderer(new CowModel, 0.7f));
	registerRenderer(EntityType::wolf, new WolfRenderer(new WolfModel, 0.5f));
	registerRenderer(EntityType::chicken, new ChickenRenderer(new ChickenModel, 0.3f));
	registerRenderer(EntityType::squid, new SquidRenderer(new SquidModel, 0.7f));
	registerRenderer(EntityType::creeper, new CreeperRenderer(new CreeperModel, new CreeperModel(2.0f), 0.5f));
	registerRenderer(EntityType::zombie, new HumanoidMobRenderer(new ZombieModel, 0.5f));
	registerRenderer(EntityType::pigZombie, getRenderer(EntityType::zombie));
	registerRenderer(EntityType::skeleton, new HumanoidMobRenderer(new SkeletonModel, 0.5f));
	registerRenderer(EntityType::spider, new SpiderRenderer);
	registerRenderer(EntityType::giant, new GiantMobRenderer(new ZombieModel, 0.5f, 6.0f));
	registerRenderer(EntityType::slime, new SlimeRenderer(new SlimeModel(16), new SlimeModel(0), 0.25f));
	registerRenderer(EntityType::ghast, new GhastRenderer(new GhastModel, 0.5f));
	registerRenderer(EntityType::mob, new MobRenderer(new HumanoidModel, 0.5f));
	registerRenderer(EntityType::monster, getRenderer(EntityType::mob));
	registerRenderer(EntityType::arrow, new ArrowRenderer);
	registerRenderer(EntityType::snowball, new ItemSpriteRenderer(Item::snowBall->getIcon(nullptr)));
	registerRenderer(EntityType::thrownEgg, new ItemSpriteRenderer(Item::egg->getIcon(nullptr)));
	registerRenderer(EntityType::fireball, new ItemSpriteRenderer(Item::snowBall->getIcon(nullptr), 2.0f));
	registerRenderer(EntityType::item, new ItemRenderer);
	registerRenderer(EntityType::painting, new PaintingRenderer);
	registerRenderer(EntityType::primedTnt, new TntRenderer);
	registerRenderer(EntityType::fallingTile, new FallingTileRenderer);
	registerRenderer(EntityType::minecart, new MinecartRenderer);
	registerRenderer(EntityType::boat, new BoatRenderer);
	registerRenderer(EntityType::fishingHook, new FishingHookRenderer);
	registerRenderer(EntityType::lightningBolt, new LightningBoltRenderer);

}

float EntityRenderDispatcher::distanceToSqr(const Vec3& pos) const
{
	return pos.distanceToSqr(m_pos);
}

Font* EntityRenderDispatcher::getFont()
{
	return m_pFont;
}

void EntityRenderDispatcher::registerRenderer(EntityType* type, EntityRenderer* entityRenderer)
{
	m_renderers[type] = entityRenderer;
	entityRenderer->init(this);
}

EntityRenderDispatcher* EntityRenderDispatcher::getInstance()
{
	if(!instance)
		instance = new EntityRenderDispatcher;

	return instance;
}

EntityRenderer* EntityRenderDispatcher::getRenderer(const EntityType* type)
{
	if (!type) return nullptr;

	auto it = m_renderers.find(type);
	if (it != m_renderers.end())
		return it->second;

	return nullptr;
}

EntityRenderer* EntityRenderDispatcher::getRenderer(Entity* pEnt)
{
	return pEnt ? getRenderer(pEnt->getType()) : nullptr;
}

void EntityRenderDispatcher::onGraphicsReset()
{
	getRenderer(EntityType::player)->onGraphicsReset();
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
	if (!m_pMinecraft || !m_pLevel || !m_pTextures) return;
	EntityRenderer* pRenderer = getRenderer(entity);
	if (pRenderer)
	{
		pRenderer->render(entity, pos, rot, a);
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
