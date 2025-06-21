/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "EntityRenderer.hpp"
#include "HumanoidMobRenderer.hpp"
#include "TripodCameraRenderer.hpp"
#include "TntRenderer.hpp"
#include "ItemRenderer.hpp"
#include "FallingTileRenderer.hpp"
#include "PigRenderer.hpp"
#include "SheepRenderer.hpp"
#include "CowRenderer.hpp"
#include "ChickenRenderer.hpp"
#include "CreeperRenderer.hpp"
#include "ArrowRenderer.hpp"

class Minecraft;
class Font;
class Level;
class Entity;
class Textures;
class ItemInHandRenderer;

class EntityRenderDispatcher
{
public:
	EntityRenderDispatcher();
	float distanceToSqr(const Vec3& pos);
	Font* getFont();
	EntityRenderer* getRenderer(Entity* pEnt);
	EntityRenderer* getRenderer(int renderType);
	void onGraphicsReset();
	void prepare(Level*, Textures*, Font*, std::shared_ptr<Mob>, Options*, float);
	void render(Entity*, float);
	void render(Entity*, const Vec3& pos, float rot, float a, bool postRender = true);
	void setLevel(Level*);
	void setMinecraft(Minecraft*);

	static EntityRenderDispatcher* getInstance();

public:
	ItemInHandRenderer* m_pItemInHandRenderer;
	HumanoidMobRenderer m_HumanoidMobRenderer;
	PigRenderer m_PigRenderer;
	SheepRenderer m_SheepRenderer;
	CowRenderer m_CowRenderer;
	ChickenRenderer m_ChickenRenderer;
	TntRenderer m_TntRenderer;
	ArrowRenderer m_arrowRenderer;
	ItemRenderer m_ItemRenderer;
	CreeperRenderer m_CreeperRenderer;
	//SpiderRenderer m_SpiderRenderer;
	//SkeletonRenderer m_SkeletonRenderer;
	//ZombieRenderer m_ZombieRenderer;
	//SheepFurRenderer m_SheepFurRenderer;
	TripodCameraRenderer m_CameraRenderer;
	Textures* m_pTextures;
	Level* m_pLevel;
	Minecraft* m_pMinecraft;
	std::shared_ptr<Mob> m_pMob;
	Vec2 m_rot;
	Options* m_pOptions;
	Vec3 m_pos;
	Font* m_pFont;

#ifdef ENH_ALLOW_SAND_GRAVITY
	FallingTileRenderer m_FallingTileRenderer;
#endif

	static EntityRenderDispatcher* instance;
	static Vec3 off;
};

