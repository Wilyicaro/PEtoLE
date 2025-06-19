/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Material.hpp"

Material::Material() :
	m_bFlammable(false)
{
}

Material::Material(bool bFlammable) :
	m_bFlammable(bFlammable)
{
}

Material::~Material()
{
}

Material
* Material::air,
* Material::dirt,
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
* Material::web;

void Material::initMaterials()
{
	air        = new GasMaterial();
	dirt       = new Material();
	wood       = new Material(true);
	stone      = new Material();
	metal      = new Material();
	water      = new LiquidMaterial();
	lava       = new LiquidMaterial();
	leaves     = new Material(true);
	plant      = new DecorationMaterial();
	sponge     = new Material();
	cloth      = new Material(true);
	fire       = new GasMaterial();
	sand       = new Material();
	decoration = new DecorationMaterial();
	glass      = new Material();
	explosive  = new Material(true);
	coral      = new Material();
	ice        = new Material();
	topSnow    = new DecorationMaterial();
	snow       = new Material();
	cactus     = new Material();
	clay       = new Material();
	vegetable  = new Material();
	portal     = new Material();
	cake       = new Material();
	web = new DecorationMaterial();
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
}

bool Material::isLiquid() const
{
	return false;
}

bool Material::letsWaterThrough() const
{
	if (isLiquid())
		return false;

	return !isSolid();
}

bool Material::isSolid() const
{
	return true;
}

bool Material::isFlammable() const
{
	return m_bFlammable;
}

bool Material::blocksLight() const
{
	return true;
}

bool Material::blocksMotion() const
{
	return true;
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
