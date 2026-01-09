/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Material.hpp"

Material::Material(MapColor* mapColor) :
	m_pMapColor(mapColor),
	m_bFlammable(false),
	m_bReplaceable(false),
	m_bMineable(true),
	m_bTranslucent(false),
	m_pushReaction(0)
{
}

Material::~Material()
{
}

Material
* Material::air,
* Material::dirt,
* Material::grass,
* Material::wood,
* Material::stone,
* Material::metal,
* Material::water,
* Material::lava,
* Material::leaves,
* Material::plant,
* Material::sponge,
* Material::cloth,
* Material::fire,
* Material::sand,
* Material::decoration,
* Material::glass,
* Material::explosive,
* Material::coral,
* Material::ice,
* Material::topSnow,
* Material::snow,
* Material::cactus,
* Material::clay,
* Material::vegetable,
* Material::portal,
* Material::cake,
* Material::web,
* Material::piston;

void Material::initMaterials()
{
	air  = new GasMaterial(MapColor::air);
	grass = new Material(MapColor::grass);
	dirt = new Material(MapColor::dirt);
	wood = (new Material(MapColor::wood))->setFlammable();
	stone = (new Material(MapColor::stone))->setNonMineable();
	metal = (new Material(MapColor::metal))->setNonMineable();
	water = (new LiquidMaterial(MapColor::water))->destroyOnPush();
	lava = (new LiquidMaterial(MapColor::red))->destroyOnPush();
	leaves = (new Material(MapColor::foliage))->setFlammable()->setTranslucent()->destroyOnPush();
	plant = (new DecorationMaterial(MapColor::foliage))->destroyOnPush();
	sponge = new Material(MapColor::cloth);
	cloth = (new Material(MapColor::cloth))->setFlammable();
	fire = (new GasMaterial(MapColor::air))->destroyOnPush();
	sand = new Material(MapColor::sand);
	decoration = (new DecorationMaterial(MapColor::air))->destroyOnPush();
	glass = (new Material(MapColor::air))->setTranslucent();
	explosive = (new Material(MapColor::red))->setFlammable()->setTranslucent();
	coral = (new Material(MapColor::foliage))->destroyOnPush();
	ice = (new Material(MapColor::ice))->setTranslucent();
	topSnow = (new DecorationMaterial(MapColor::snow))->setReplaceable()->setNonMineable()->destroyOnPush();
	snow = (new Material(MapColor::snow))->setNonMineable()->setTranslucent();
	cactus = (new Material(MapColor::foliage))->setTranslucent()->destroyOnPush();
	clay = new Material(MapColor::clay);
	vegetable = (new Material(MapColor::foliage))->destroyOnPush();;
	portal = new Material(MapColor::air);
	cake = (new Material(MapColor::air))->destroyOnPush();
	web = (new Material(MapColor::cloth))->setNonMineable()->destroyOnPush();
	piston = (new Material(MapColor::stone))->notPushable();
}

void Material::teardownMaterials()
{
	SAFE_DELETE(air);
	SAFE_DELETE(dirt);
	SAFE_DELETE(wood);
	SAFE_DELETE(stone);
	SAFE_DELETE(metal);
	SAFE_DELETE(water);
	SAFE_DELETE(lava);
	SAFE_DELETE(leaves);
	SAFE_DELETE(plant);
	SAFE_DELETE(sponge);
	SAFE_DELETE(cloth);
	SAFE_DELETE(fire);
	SAFE_DELETE(sand);
	SAFE_DELETE(decoration);
	SAFE_DELETE(glass);
	SAFE_DELETE(explosive);
	SAFE_DELETE(coral);
	SAFE_DELETE(ice);
	SAFE_DELETE(topSnow);
	SAFE_DELETE(snow);
	SAFE_DELETE(cactus);
	SAFE_DELETE(clay);
	SAFE_DELETE(vegetable);
	SAFE_DELETE(portal);
	SAFE_DELETE(cake);
	SAFE_DELETE(web);
	SAFE_DELETE(piston);
}

bool Material::isLiquid() const
{
	return false;
}

bool Material::isReplaceable() const
{
	return false;
}

bool Material::isMineable() const
{
	return m_bMineable;
}

bool Material::isOpaque() const
{
	return !m_bTranslucent && isSolid();
}

bool Material::isSolid() const
{
	return true;
}

bool Material::isFlammable() const
{
	return m_bFlammable;
}

int Material::getPushReaction() const
{
	return m_pushReaction;
}

Material* Material::setReplaceable()
{
	m_bReplaceable = true;
	return this;
}

Material* Material::setFlammable()
{
	m_bFlammable = true;
	return this;
}

Material* Material::setNonMineable()
{
	m_bMineable = false;
	return this;
}

Material* Material::setTranslucent()
{
	m_bTranslucent = true;
	return this;
}

Material* Material::destroyOnPush()
{
	m_pushReaction = 1;
	return this;
}

Material* Material::notPushable()
{
	m_pushReaction = 2;
	return this;
}

bool Material::blocksLight() const
{
	return true;
}

bool Material::blocksMotion() const
{
	return true;
}

GasMaterial::GasMaterial(MapColor* mapColor) : Material(mapColor)
{
	setReplaceable();
}

bool GasMaterial::isSolid() const
{
	return false;
}

bool GasMaterial::blocksLight() const
{
	return false;
}

bool GasMaterial::blocksMotion() const
{
	return false;
}

DecorationMaterial::DecorationMaterial(MapColor* mapColor) : Material(mapColor)
{
}

bool DecorationMaterial::isSolid() const
{
	return false;
}

bool DecorationMaterial::blocksLight() const
{
	return false;
}

bool DecorationMaterial::blocksMotion() const
{
	return false;
}

LiquidMaterial::LiquidMaterial(MapColor* mapColor) : Material(mapColor)
{
	setReplaceable();
}

bool LiquidMaterial::isLiquid() const
{
	return true;
}

bool LiquidMaterial::isSolid() const
{
	return false;
}

bool LiquidMaterial::blocksMotion() const
{
	return false;
}
