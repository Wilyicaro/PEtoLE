/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../entity/Entity.hpp"
#include "client/renderer/Tesselator.hpp"

enum eParticleTexture
{
	PT_PARTICLES,
	PT_TERRAIN,
	PT_ITEM,
	PT_PARTICLES3,
};

enum eParticleTextureIndex
{
	PTI_BUBBLE = 32,
	PTI_FLAME = 48,
	PTI_LAVA,
	PTI_NOTE = 64
};

class Particle : public Entity
{
private:
	void _init();
public:
	Particle() { _init(); }
	Particle(Level*, const Vec3& pos, const Vec3& dir);

	virtual void render(Tesselator&, float, float, float, float, float, float);
	virtual int getParticleTexture();
	//TODO: addAdditonalSaveData
	//TODO: readAdditionalSaveData
	//TODO: _defineEntityData

	void tick() override;

	Particle* scale(float);
	Particle* setPower(float);

public:
	int m_tex;
	float m_uo;
	float m_vo;
	int m_age;
	int m_lifetime;
	float m_size;
	float m_gravity;
	float m_rCol;
	float m_gCol;
	float m_bCol;
	bool m_bIsUnlit;

	static Vec3 off;
};

class TerrainParticle : public Particle
{
private:
	void _init(Tile* tile);
public:
	TerrainParticle(Level*, const Vec3& pos, Tile*);
	TerrainParticle(Level*, const Vec3& pos, const Vec3& dir, Tile*);

	void render(Tesselator&, float, float, float, float, float, float) override;
	int getParticleTexture() override;
	TerrainParticle* init(const TilePos& tilePos);
	TerrainParticle* init(const TilePos& tilePos, int data);

public:
	Tile* m_pTile;
};

class BubbleParticle : public Particle
{
public:
	BubbleParticle(Level*, const Vec3& pos, const Vec3& dir);
	void tick() override;
};

class SmokeParticle : public Particle
{
public:
	SmokeParticle(Level*, const Vec3& pos, const Vec3& dir, float a9 = 1.0f);
	void tick() override;
	void render(Tesselator&, float, float, float, float, float, float) override;

public:
	float m_oSize;
};

class RedDustParticle : public Particle
{
public:
	RedDustParticle(Level*, const Vec3& pos, const Vec3& dir);
	void tick() override;
	void render(Tesselator&, float, float, float, float, float, float) override;

public:
	float m_oSize;
};

class PickupParticle : public Particle
{
public:
	PickupParticle(Level*, std::shared_ptr<Entity> throwed, std::shared_ptr<Entity> thrower, float vel);
	void tick() override;
	int getParticleTexture() override;
	void render(Tesselator&, float, float, float, float, float, float) override;
	std::shared_ptr<Entity> throwed;
	std::shared_ptr<Entity> thrower;
	float vel;
};

class ExplodeParticle : public Particle
{
public:
	ExplodeParticle(Level*, const Vec3& pos, const Vec3& dir);
	void tick() override;
};

class FlameParticle : public Particle
{
public:
	FlameParticle(Level*, const Vec3& pos, const Vec3& dir);
	void tick() override;
	void render(Tesselator&, float, float, float, float, float, float) override;
	float getBrightness(float f) const override;

public:
	float m_oSize;
};

class LavaParticle : public Particle
{
public:
	LavaParticle(Level*, const Vec3& pos);
	void tick() override;
	void render(Tesselator&, float, float, float, float, float, float) override;
	float getBrightness(float f) const override;

public:
	float m_oSize;
};

class NoteParticle : public Particle
{
public:
	NoteParticle(Level*, const Vec3& pos, const Vec3& dir, float scale = 2.0);
	void tick() override;
	void render(Tesselator&, float, float, float, float, float, float) override;

public:
	float m_oSize;
};

class WaterDropParticle : public Particle
{
public:
	WaterDropParticle(Level*, const Vec3& pos);
	void tick() override;
};

class SplashParticle : public WaterDropParticle
{
public:
	SplashParticle(Level*, const Vec3& pos, const Vec3& dir);
};

class PortalParticle : public Particle
{
public:
	PortalParticle(Level*, const Vec3& pos, const Vec3& dir);
	void tick() override;
	void render(Tesselator&, float, float, float, float, float, float) override;
	float getBrightness(float f) const override;

public:
	float m_oSize;
	Vec3 m_startPos;
};

class BreakingItemParticle : public Particle
{
public:
	BreakingItemParticle(Level* level, const Vec3& pos, Item* item);
	int getParticleTexture() override;
	void render(Tesselator&, float, float, float, float, float, float) override;
};

