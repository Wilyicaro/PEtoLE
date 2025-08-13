/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <unordered_map>
#include "EntityRenderer.hpp"
#include "world/phys/Vec2.hpp"

class Minecraft;
class Font;
class Level;
class Entity;
class EntityType;
class Textures;
class ItemInHandRenderer;

class EntityRenderDispatcher
{
public:
	EntityRenderDispatcher();
	float distanceToSqr(const Vec3& pos) const;
	Font* getFont();
	void registerRenderer(EntityType*, EntityRenderer*);
	EntityRenderer* getRenderer(Entity* pEnt);
	EntityRenderer* getRenderer(const EntityType*);
	void onGraphicsReset();
	void prepare(Level*, Textures*, Font*, std::shared_ptr<Mob>, Options*, float);
	void render(Entity*, float);
	void render(Entity*, const Vec3& pos, float rot, float a, bool postRender = true);
	void setLevel(Level*);
	void setMinecraft(Minecraft*);

	static EntityRenderDispatcher* getInstance();

public:
	std::unordered_map<const EntityType*, EntityRenderer*> m_renderers;
	ItemInHandRenderer* m_pItemInHandRenderer;
	Textures* m_pTextures;
	Level* m_pLevel;
	Minecraft* m_pMinecraft;
	std::shared_ptr<Mob> m_pMob;
	Vec2 m_rot;
	Options* m_pOptions;
	Vec3 m_pos;
	Font* m_pFont;


	static EntityRenderDispatcher* instance;
	static Vec3 off;
};

