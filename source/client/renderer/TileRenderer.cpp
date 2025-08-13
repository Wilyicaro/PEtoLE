/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TileRenderer.hpp"
#include "client/app/Minecraft.hpp"
#include "client/renderer/PatchManager.hpp"
#include "world/tile/FireTile.hpp"
#include "world/tile/LiquidTile.hpp"
#include "client/renderer/GrassColor.hpp"
#include "client/renderer/FoliageColor.hpp"
#include <world/tile/RedStoneDustTile.hpp>
#include <world/tile/BedTile.hpp>
#include <world/tile/RailTile.hpp>
#include <world/tile/RepeaterTile.hpp>

bool TileRenderer::m_bFancyGrass = true;
bool TileRenderer::m_bBiomeColors = true;

void TileRenderer::_init()
{
	m_textureOverride = -1;
	m_bDisableCulling = false;
	m_bAmbientOcclusion = false;
	field_C = 0;
	field_10 = 0;
	field_14 = 0;
	field_18 = 0;
	field_1C = 0;
	field_20 = 0;
	field_24 = 0;
	field_28 = 0;
	field_2C = 0;
	m_noNeighborUpdate = 0;
	field_34 = 0;
	field_38 = 0;
	field_3C = 0;
	field_40 = 0;
	field_44 = 0;
	field_48 = 0;
	field_4C = 0;
	field_50 = 0;
	field_54 = 0;
	field_58 = 0;
	field_5C = 0;
	field_60 = 0;
	field_64 = 0;
	field_68 = 0;
	field_6C = 0;
	m_checkBuffer = 0;
	m_oTex = 0;
	field_78 = 1;
	field_AC = false;
	field_AD = false;
	field_AE = false;
	field_AF = false;
	field_B0 = false;
	field_B1 = false;
	field_B2 = false;
	field_B3 = false;
	field_B4 = false;
	field_B5 = false;
	field_B6 = false;
	field_B7 = false;
}

TileRenderer::TileRenderer()
{
	_init();
#ifndef ORIGINAL_CODE
	// @BUG: Not initializing level source
	m_pLevelSource = nullptr;
#endif
}

TileRenderer::TileRenderer(LevelSource* pLevelSource)
{
	_init();
	m_pLevelSource = pLevelSource;
}

float TileRenderer::getWaterHeight(const TilePos& pos, const Material* pCheckMtl)
{
	int iBias = 0;
	float fHeight = 0.0f;
	for (int i = 0; i < 4; i++)
	{
		TilePos checkPos(pos.x - (i & 1),
					  pos.y,
					  pos.z - ((i >> 1) & 1));

		if (m_pLevelSource->getMaterial(TilePos(checkPos.x, checkPos.y + 1, checkPos.z)) == pCheckMtl)
			return 1.0f;

		Material* pMtl = m_pLevelSource->getMaterial(checkPos);
		if (pMtl == pCheckMtl)
		{
			int data = m_pLevelSource->getData(checkPos);
			if (data >= 8 || data == 0)
			{
				fHeight += LiquidTile::getHeight(data) * 10.0f;
				iBias += 10;
			}

			fHeight += LiquidTile::getHeight(data);
			iBias++;
			continue;
		}

		if (!pMtl->isSolid())
		{
			fHeight++;
			iBias++;
		}
	}

	return 1.0f - fHeight / float(iBias);
}

bool TileRenderer::canRender(int renderShape)
{
	return renderShape == SHAPE_SOLID || renderShape == SHAPE_STAIRS || renderShape == SHAPE_FENCE || renderShape == SHAPE_CACTUS;
}

void TileRenderer::renderFace(Tile* tile, const Vec3& pos, int texture, Facing::Name face, float r, float g, float b, int rot)
{
	int renderShape = tile->getRenderShape();
	if (renderShape == SHAPE_CACTUS) {
		switch (face) {
		case Facing::NORTH: case Facing::SOUTH: tile->setShape(0, 0, 0.0625, 1, 1, 0.9375); break;
		case Facing::WEST: case Facing::EAST: tile->setShape(0.0625, 0, 0, 0.9375, 1, 1); break;
		}
	}

	static constexpr float C_RATIO = 1.0f / 256.0f;

	if (m_textureOverride >= 0)
		texture = m_textureOverride;

	bool flip = texture < 0;

	if (flip) texture = -texture;

	float texX = float(16 * (texture % 16));
	float texY = float(16 * (texture / 16));

	float u1, u2, v1, v2;

	AABB& aabb = tile->m_aabb;

	const auto& uvCheck = Facing::UV_CHECKERS[face / 2];

	float minU = aabb.byIndex(uvCheck[0]);
	float maxU = aabb.byIndex(uvCheck[1]);

	if (minU < 0.0f || maxU > 1.0f)
	{
		u1 = C_RATIO * (texX);
		u2 = C_RATIO * (texX + 15.99f);
	}
	else {
		u1 = C_RATIO * (texX + 16.0f * minU);
		u2 = C_RATIO * (texX + 16.0f * maxU - 0.01f);
	}

	float minV = aabb.byIndex(uvCheck[2]);
	float maxV = aabb.byIndex(uvCheck[3]);

	if (minV < 0.0f || maxV > 1.0f)
	{
		v1 = C_RATIO * (texY);
		v2 = C_RATIO * (texY + 15.99f);
	}
	else {
		v1 = C_RATIO * (texY + 16.0f * minV);
		v2 = C_RATIO * (texY + 16.0f * maxV - 0.01f);
	}
	

	const auto& baseUVs = Facing::UVS[face];

	Tesselator& t = Tesselator::instance;

	if (!m_bAmbientOcclusion) t.color(r, g, b);

	for (int i = 0; i < 4; ++i) {
		const auto vertex = Facing::CORNERS[Facing::VERTICES[face][i]];
		if (m_bAmbientOcclusion) t.color(m_vtxRed[i] * r, m_vtxGreen[i] * g, m_vtxBlue[i] * b);
		int rotIndex = Facing::ROTATED_INDEX[rot % 4][i];
		bool useU2 = baseUVs[rotIndex][0];
		if (flip) useU2 = !useU2;
		t.vertexUV(aabb.byIndex(vertex[0]) + pos.x, aabb.byIndex(vertex[1]) + pos.y, aabb.byIndex(vertex[2]) + pos.z, useU2 ? u2 : u1, baseUVs[rotIndex][1] ? v2 : v1);
	}

	if (renderShape == SHAPE_CACTUS && Facing::isHorizontal(face)) {
		tile->updateShape(m_pLevelSource, pos);
	}
}

void TileRenderer::renderEast(Tile* tile, const Vec3& pos, int texture, float r, float g, float b)
{
	renderFace(tile, pos, texture, Facing::EAST, r, g, b);
}

void TileRenderer::renderWest(Tile* tile, const Vec3& pos, int texture, float r, float g, float b)
{
	renderFace(tile, pos, texture, Facing::WEST, r, g, b);
}

void TileRenderer::renderSouth(Tile* tile, const Vec3& pos, int texture, float r, float g, float b)
{
	renderFace(tile, pos, texture, Facing::SOUTH, r, g, b);
}

void TileRenderer::renderNorth(Tile* tile, const Vec3& pos, int texture, float r, float g, float b)
{
	renderFace(tile, pos, texture, Facing::NORTH, r, g, b);
}

void TileRenderer::renderDown(Tile* tile, const Vec3& pos, int texture, float r, float g, float b)
{
	renderFace(tile, pos, texture, Facing::DOWN, r, g, b);
}

void TileRenderer::renderUp(Tile* tile, const Vec3& pos, int texture, float r, float g, float b)
{
	renderFace(tile, pos, texture, Facing::UP, r, g, b);
}

void TileRenderer::tesselateCrossTexture(Tile* tile, int data, const Vec3& pos)
{
	static constexpr float C_RATIO = 1.0f / 256.0f;

	int texture = m_textureOverride;
	if (texture < 0)
		texture = tile->getTexture(Facing::DOWN, data);

	float texX = float(16 * (texture % 16));
	float texY = float(16 * (texture / 16));

	// calculate U and V coordinates
	float texU_l = texX * C_RATIO, texU_r = (texX + 15.99f) * C_RATIO;
	float texV_u = texY * C_RATIO, texV_d = (texY + 15.99f) * C_RATIO;

	

	float cenX = pos.x + 0.5f, cenZ = pos.z + 0.5f;
	float newY = pos.y;

	if (tile->getRenderShape() == SHAPE_RANDOM_CROSS)
	{
		int64_t var17 = int64_t(pos.x * 3129871) ^ (int64_t)pos.z * 116129781L ^ (int64_t)pos.y;
		var17 = var17 * var17 * 42317861L + var17 * 11L;
		cenX += (((var17 >> 16 & 15L) / 15.0F) - 0.5f) * 0.5f;
		newY += (((var17 >> 20 & 15L) / 15.0F) - 1.0f) * 0.2f;
		cenZ += (((var17 >> 24 & 15L) / 15.0F) - 0.5f) * 0.5f;
	}
	
	float x1 = cenX - 0.45f, x2 = cenX + 0.45f;
	float z1 = cenZ - 0.45f, z2 = cenZ + 0.45f;

	Tesselator& t = Tesselator::instance;
	// face 1
	t.vertexUV(x1, newY + 1, z1, texU_l, texV_u);
	t.vertexUV(x1, newY + 0, z1, texU_l, texV_d);
	t.vertexUV(x2, newY + 0, z2, texU_r, texV_d);
	t.vertexUV(x2, newY + 1, z2, texU_r, texV_u);

	// face 2
	t.vertexUV(x2, newY + 1, z2, texU_l, texV_u);
	t.vertexUV(x2, newY + 0, z2, texU_l, texV_d);
	t.vertexUV(x1, newY + 0, z1, texU_r, texV_d);
	t.vertexUV(x1, newY + 1, z1, texU_r, texV_u);

	// face 3
	t.vertexUV(x1, newY + 1, z2, texU_l, texV_u);
	t.vertexUV(x1, newY + 0, z2, texU_l, texV_d);
	t.vertexUV(x2, newY + 0, z1, texU_r, texV_d);
	t.vertexUV(x2, newY + 1, z1, texU_r, texV_u);

	// face 4
	t.vertexUV(x2, newY + 1, z1, texU_l, texV_u);
	t.vertexUV(x2, newY + 0, z1, texU_l, texV_d);
	t.vertexUV(x1, newY + 0, z2, texU_r, texV_d);
	t.vertexUV(x1, newY + 1, z2, texU_r, texV_u);
}

void TileRenderer::tesselateRowTexture(Tile* tile, int data, const Vec3& pos)
{
	static constexpr float C_RATIO = 1.0f / 256.0f;

	int texture = m_textureOverride;
	if (texture < 0)
		texture = tile->getTexture(Facing::DOWN, data);

	float texX = float(16 * (texture % 16));
	float texY = float(16 * (texture / 16));

	// calculate U and V coordinates
	float u0 = texX * C_RATIO, u1 = (texX + 15.99f) * C_RATIO;
	float v0 = texY * C_RATIO, v1 = (texY + 15.99f) * C_RATIO;


	float x0 = pos.x + 0.25f, x1 = pos.x + 0.75f;
	float z0 = pos.z, z1 = pos.z + 1.0f;

	Tesselator& t = Tesselator::instance;
	t.vertexUV(x0, pos.y + 1.0, z0, u0, v0);
	t.vertexUV(x0, pos.y + 0.0, z0, u0, v1);
	t.vertexUV(x0, pos.y + 0.0, z1, u1, v1);
	t.vertexUV(x0, pos.y + 1.0, z1, u1, v0);
	t.vertexUV(x0, pos.y + 1.0, z1, u0, v0);
	t.vertexUV(x0, pos.y + 0.0, z1, u0, v1);
	t.vertexUV(x0, pos.y + 0.0, z0, u1, v1);
	t.vertexUV(x0, pos.y + 1.0, z0, u1, v0);
	t.vertexUV(x1, pos.y + 1.0, z1, u0, v0);
	t.vertexUV(x1, pos.y + 0.0, z1, u0, v1);
	t.vertexUV(x1, pos.y + 0.0, z0, u1, v1);
	t.vertexUV(x1, pos.y + 1.0, z0, u1, v0);
	t.vertexUV(x1, pos.y + 1.0, z0, u0, v0);
	t.vertexUV(x1, pos.y + 0.0, z0, u0, v1);
	t.vertexUV(x1, pos.y + 0.0, z1, u1, v1);
	t.vertexUV(x1, pos.y + 1.0, z1, u1, v0);
	x0 = pos.x + 0.5 - 0.5;
	x1 = pos.x + 0.5 + 0.5;
	z0 = pos.z + 0.5 - 0.25;
	z1 = pos.z + 0.5 + 0.25;
	t.vertexUV(x0, pos.y + 1.0, z0, u0, v0);
	t.vertexUV(x0, pos.y + 0.0, z0, u0, v1);
	t.vertexUV(x1, pos.y + 0.0, z0, u1, v1);
	t.vertexUV(x1, pos.y + 1.0, z0, u1, v0);
	t.vertexUV(x1, pos.y + 1.0, z0, u0, v0);
	t.vertexUV(x1, pos.y + 0.0, z0, u0, v1);
	t.vertexUV(x0, pos.y + 0.0, z0, u1, v1);
	t.vertexUV(x0, pos.y + 1.0, z0, u1, v0);
	t.vertexUV(x1, pos.y + 1.0, z1, u0, v0);
	t.vertexUV(x1, pos.y + 0.0, z1, u0, v1);
	t.vertexUV(x0, pos.y + 0.0, z1, u1, v1);
	t.vertexUV(x0, pos.y + 1.0, z1, u1, v0);
	t.vertexUV(x0, pos.y + 1.0, z1, u0, v0);
	t.vertexUV(x0, pos.y + 0.0, z1, u0, v1);
	t.vertexUV(x1, pos.y + 0.0, z1, u1, v1);
	t.vertexUV(x1, pos.y + 1.0, z1, u1, v0);
}

inline void computeColor(int color, float& red, float& grn, float& blu, float bright) {
	red = float(GET_RED(color)) / 255.0f * bright;
	grn = float(GET_GREEN(color)) / 255.0f * bright;
	blu = float(GET_BLUE(color)) / 255.0f * bright;
}

bool TileRenderer::tesselateBlockInWorld(Tile* tile, const TilePos& pos, float r, float g, float b)
{
	Tesselator& t = Tesselator::instance;
	float fLightHere = tile->getBrightness(m_pLevelSource, pos);
	bool bDrewAnything = false;

	for (Facing::Name face : Facing::ALL)
	{
		TilePos neighborPos = pos.relative(face);

		if (!m_bDisableCulling && !tile->shouldRenderFace(m_pLevelSource, neighborPos, face))
			continue;

		bDrewAnything = true;

		float light = tile->getBrightness(m_pLevelSource, neighborPos);

		switch (face) {
		case Facing::UP:
			if (tile->m_aabb.max.y != 1.0f && !tile->m_pMaterial->isLiquid())
				light = fLightHere;
			break;
		case Facing::NORTH:
			if (tile->m_aabb.min.z > 0.0f)
				light = fLightHere;
			break;
		case Facing::SOUTH:
			if (tile->m_aabb.max.z < 1.0f)
				light = fLightHere;
			break;
		case Facing::WEST:
			if (tile->m_aabb.min.x > 0.0f)
				light = fLightHere;
			break;
		case Facing::EAST:
			if (tile->m_aabb.max.x < 1.0f)
				light = fLightHere;
			break;
		default:
			break;
		}

		int texture = tile->getTexture(m_pLevelSource, pos, face);
		float red, grn, blue;
		computeColor(tile->getColor(m_pLevelSource, pos, face, texture), red, grn, blue, Facing::LIGHT[face] * light);

		renderFace(tile, pos, texture, face, red * r, grn * g, blue * b);

		if (m_bFancyGrass && this->m_textureOverride < 0 && texture == TEXTURE_GRASS_SIDE && Facing::isHorizontal(face))
		{
			computeColor(tile->getColor(m_pLevelSource, pos, face, TEXTURE_GRASS_SIDE_OVERLAY), red, grn, blue, Facing::LIGHT[face] * light);
			renderFace(tile, pos, TEXTURE_GRASS_SIDE_OVERLAY, face, red * r, grn * g, blue * b);
		}
	}

	return bDrewAnything;
}

bool TileRenderer::tesselateBlockInWorld(Tile* tile, const TilePos& pos)
{
	if (useAmbientOcclusion())
	{
		return tesselateBlockInWorldWithAmbienceOcclusionV2(tile, pos, 1.0f, 1.0f, 1.0f);
	}

	return tesselateBlockInWorld(tile, pos, 1.0f, 1.0f, 1.0f);
}

bool TileRenderer::tesselateCrossInWorld(Tile* tile, const TilePos& pos)
{

	float r, g, b;
	computeColor(getTileColor(tile, pos), r, g, b, tile->getBrightness(m_pLevelSource, pos));

	Tesselator& t = Tesselator::instance;

	t.color(r, g, b);

	tesselateCrossTexture(tile, m_pLevelSource->getData(pos), pos);

	return true;
}

bool TileRenderer::tesselateRowInWorld(Tile* tile, const TilePos& pos)
{
	float r, g, b;
	computeColor(getTileColor(tile, pos), r, g, b, tile->getBrightness(m_pLevelSource, pos));

	Tesselator& t = Tesselator::instance;

	t.color(r, g, b);

	tesselateRowTexture(tile, m_pLevelSource->getData(pos), pos);

	return true;
}

bool TileRenderer::tesselateWaterInWorld(Tile* tile1, const TilePos& pos)
{
	constexpr float C_RATIO = 1.0f / 256.0f;

	LiquidTile* tile = (LiquidTile*)tile1;
	bool bRenderFaceDown, bRenderFaceUp, bRenderSides[4];

	Tesselator& t = Tesselator::instance;

	bRenderFaceDown = tile->shouldRenderFace(m_pLevelSource, pos.above(), Facing::UP);
	bRenderFaceUp   = tile->shouldRenderFace(m_pLevelSource, pos.below(), Facing::DOWN);

	bRenderSides[0] = tile->shouldRenderFace(m_pLevelSource, pos.north(), Facing::NORTH);
	bRenderSides[1] = tile->shouldRenderFace(m_pLevelSource, pos.south(), Facing::SOUTH);
	bRenderSides[2] = tile->shouldRenderFace(m_pLevelSource, pos.west(), Facing::WEST);
	bRenderSides[3] = tile->shouldRenderFace(m_pLevelSource, pos.east(), Facing::EAST);

	if (!bRenderFaceDown &&
		!bRenderFaceUp   &&
		!bRenderSides[0] &&
		!bRenderSides[1] &&
		!bRenderSides[2] &&
		!bRenderSides[3])
		return false;

	int tileData = m_pLevelSource->getData(pos);
	float
		fHeight1 = getWaterHeight(pos, tile->m_pMaterial),
		fHeight2 = getWaterHeight(TilePos(pos.x, pos.y, pos.z + 1), tile->m_pMaterial),
		fHeight3 = getWaterHeight(TilePos(pos.x + 1, pos.y, pos.z + 1), tile->m_pMaterial),
		fHeight4 = getWaterHeight(pos.east(), tile->m_pMaterial);

	// @TODO: fix gotos
	bool bFlag1, bFlag2;
	if (!m_bDisableCulling)
	{
		bFlag1 = bRenderFaceDown;
		if (!bRenderFaceDown)
		{
			if (!bRenderFaceUp)
				goto label_8;

			goto label_6;
		}
	}

	{
		int texFaceDown = tile->getTexture(Facing::UP, tileData);
		float slopeAngle = tile->getSlopeAngle(m_pLevelSource, pos, tile->m_pMaterial);

		int texX, texY;

		if (slopeAngle > -999.0f)
		{
			int texNorth = tile->getTexture(Facing::NORTH, tileData);
			texX = texNorth & 0xF0;
			texY = (texNorth & 0xF) * 16;
		}
		else
		{
			texX = texFaceDown & 0xF0;
			texY = (texFaceDown & 0xF) * 16;
		}

		float texUV_1, texUV_2, texUV_3, texUV_4, texUV_5, texUV_6, texUV_7, texUV_8;
		if (slopeAngle >= -999.0f)
		{
			texUV_1 = float(texY + 16) * C_RATIO;
			texUV_2 = float(texX + 16) * C_RATIO;
		}
		else
		{
			slopeAngle = 0.0f;
			texUV_1 = float(texY + 8) * C_RATIO;
			texUV_2 = float(texX + 8) * C_RATIO;
		}

		texUV_3 = C_RATIO * 8.0f * Mth::sin(slopeAngle);
		texUV_4 = C_RATIO * 8.0f * Mth::cos(slopeAngle);

		float bright = tile->getBrightness(m_pLevelSource, pos);

		texUV_5 = texUV_1 - texUV_4;
		texUV_6 = texUV_2 - texUV_4;

		t.color(bright, bright, bright);
		texUV_7 = texUV_2 + texUV_4;
		texUV_8 = texUV_1 + texUV_4;

		t.vertexUV(pos.x + 0.0f, pos.y + fHeight1, pos.z + 0.0f, (texUV_1 - texUV_4) - texUV_3, texUV_6 + texUV_3);
		t.vertexUV(pos.x + 0.0f, pos.y + fHeight2, pos.z + 1.0f, texUV_3 + texUV_5, texUV_7 + texUV_3);
		t.vertexUV(pos.x + 1.0f, pos.y + fHeight3, pos.z + 1.0f, texUV_8 + texUV_3, texUV_7 - texUV_3);
		t.vertexUV(pos.x + 1.0f, pos.y + fHeight4, pos.z + 0.0f, texUV_8 - texUV_3, texUV_6 - texUV_3);
	}

	if (m_bDisableCulling)
		goto label_7;

	bFlag1 = true;
	if (bRenderFaceUp)
	{
	label_6:
	label_7:
		float bright = tile->getBrightness(m_pLevelSource, pos.below());
		renderDown(tile1, pos, tile->getTexture(Facing::DOWN), bright * 0.5f, bright * 0.5f, bright * 0.5f);
		bFlag1 = true;
	}

label_8:
	bFlag2 = bFlag1;
	bool bRenderedSides = false;

	for (int dir = 0; dir < 4; dir++)
	{
		int checkX = pos.x, checkZ = pos.z;
		switch (dir)
		{
			case 0: checkZ--; break;
			case 1: checkZ++; break;
			case 2: checkX--; break;
			case 3: checkX++; break;
		}

		int texture = tile1->getTexture((Facing::Name)(dir + Facing::NORTH), tileData);
		if (!m_bDisableCulling && !bRenderSides[dir])
			continue;

		float vtxX1, vtxX2, vtxZ1, vtxZ2, height1, height2;
		switch (dir)
		{
			case 0:
			{
				vtxX1 = float(pos.x);
				vtxX2 = float(pos.x + 1);
				vtxZ1 = float(pos.z);
				vtxZ2 = float(pos.z);
				height1 = fHeight1;
				height2 = fHeight4;
				break;
			}
			case 1:
			{
				vtxX1 = float(pos.x + 1);
				vtxX2 = float(pos.x);
				vtxZ1 = float(pos.z + 1);
				vtxZ2 = float(pos.z + 1);
				height1 = fHeight3;
				height2 = fHeight2;
				break;
			}
			case 2:
			{
				vtxX1 = float(pos.x);
				vtxX2 = float(pos.x);
				vtxZ1 = float(pos.z + 1);
				vtxZ2 = float(pos.z);
				height1 = fHeight2;
				height2 = fHeight1;
				break;
			}
			case 3:
			{
				vtxX1 = float(pos.x + 1);
				vtxX2 = float(pos.x + 1);
				vtxZ1 = float(pos.z);
				vtxZ2 = float(pos.z + 1);
				height1 = fHeight4;
				height2 = fHeight3;
				break;
			}
		}

		float texU_1, texU_2, texV_1, texV_2, texV_3;

		int texX = (texture & 0xF) * 16;
		int texY = (texture >> 4) * 16;
		texU_1 = C_RATIO * float(texX);
		texU_2 = C_RATIO * (float(texX) + 15.99f);
		texV_1 = C_RATIO * (float(texY) + (1.0f - height1) * 16.0f);
		texV_2 = C_RATIO * (float(texY) + (1.0f - height2) * 16.0f);
		texV_3 = C_RATIO * (float(texY + 16.0f) - 0.01f);
		bFlag2 = true;
		bRenderedSides = true;

		float brightMul = dir >= Facing::WEST ? 0.6f : 0.8f;
		float bright = tile->getBrightness(m_pLevelSource, TilePos(checkX, pos.y, checkZ));
		t.color(bright* brightMul, bright* brightMul, bright* brightMul);
		t.vertexUV(vtxX1, float(pos.y) + height1, vtxZ1, texU_1, texV_1);
		t.vertexUV(vtxX2, float(pos.y) + height2, vtxZ2, texU_2, texV_2);
		t.vertexUV(vtxX2, float(pos.y) + 0.0f, vtxZ2, texU_2, texV_3);
		t.vertexUV(vtxX1, float(pos.y) + 0.0f, vtxZ1, texU_1, texV_3);
	}

	tile->m_aabb.min.y = 0.0f;
	tile->m_aabb.max.y = 1.0f;

	return bFlag2;
}

#ifdef ENH_FIX_INVIS_STAIRS
#define PROC_RESULT(x) bRenderedAnything = (x) || bRenderedAnything;
#else
#define PROC_RESULT(x) (x)
#endif

bool TileRenderer::tesselateStairsInWorld(Tile* tile, const TilePos& pos)
{
	bool bRenderedAnything = false;

	switch (m_pLevelSource->getData(pos))
	{
		case 0:
		{
			tile->setShape(0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 1.0f);
			PROC_RESULT(tesselateBlockInWorld(tile, pos));
			tile->setShape(0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
			PROC_RESULT(tesselateBlockInWorld(tile, pos));
			break;
		}
		case 1:
		{
			tile->setShape(0.0f, 0.0f, 0.0f, 0.5f, 1.0f, 1.0f);
			PROC_RESULT(tesselateBlockInWorld(tile, pos));
			tile->setShape(0.5f, 0.0f, 0.0f, 1.0f, 0.5f, 1.0f);
			PROC_RESULT(tesselateBlockInWorld(tile, pos));
			break;
		}
		case 2:
		{
			tile->setShape(0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.5f);
			PROC_RESULT(tesselateBlockInWorld(tile, pos));
			tile->setShape(0.0f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f);
			PROC_RESULT(tesselateBlockInWorld(tile, pos));
			break;
		}
		case 3:
		{
			tile->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f);
			PROC_RESULT(tesselateBlockInWorld(tile, pos));
			tile->setShape(0.0f, 0.0f, 0.5f, 1.0f, 0.5f, 1.0f);
			PROC_RESULT(tesselateBlockInWorld(tile, pos));
			break;
		}
	}

	tile->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	return bRenderedAnything;
}

bool TileRenderer::tesselateFenceInWorld(Tile* tile, const TilePos& pos)
{
	tile->setShape(0.375f, 0.0f, 0.375f, 0.625f, 1.0f, 0.625f);
	bool bRenderedAnything = tesselateBlockInWorld(tile, pos);


	bool var10 = m_pLevelSource->getTile(pos.west()) == tile->m_ID;
	bool var11 = m_pLevelSource->getTile(pos.east()) == tile->m_ID;
	bool var12 = m_pLevelSource->getTile(pos.north()) == tile->m_ID;
	bool var13 = m_pLevelSource->getTile(pos.south()) == tile->m_ID;

	bool var8 = var10 || var11;
	bool var9 = var12 || var13;

	if (!var8 && !var9) {
		var8 = true;
	}

	float var6 = 7.0F / 16.0F;
	float var7 = 9.0F / 16.0F;
	float var14 = 12.0F / 16.0F;
	float var15 = 15.0F / 16.0F;
	float var16 = var10 ? 0.0F : var6;
	float var17 = var11 ? 1.0F : var7;
	float var18 = var12 ? 0.0F : var6;
	float var19 = var13 ? 1.0F : var7;

	if (var8) 
	{
		tile->setShape(var16, var14, var6, var17, var15, var7);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;

	}

	if (var9)
	{
		tile->setShape(var6, var14, var18, var7, var15, var19);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
	}

	var14 = 6.0F / 16.0F;
	var15 = 9.0F / 16.0F;

	if (var8)
	{
		tile->setShape(var16, var14, var6, var17, var15, var7);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
	}

	if (var9)
	{
		tile->setShape(var6, var14, var18, var7, var15, var19);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
	}

	tile->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	return bRenderedAnything;
}



void TileRenderer::tesselateTorch(Tile* tile, const Vec3& pos, float a, float b)
{
	constexpr float C_RATIO = 1.0f / 256.0f;
	constexpr float C_ONE_PIXEL = 1.0f / 16.0f;
	constexpr float C_HALF_TILE = 1.0f / 2.0f;
	constexpr float C_TOP_SKEW_RATIO = 0.375f;

	int texture = tile->getTexture(Facing::DOWN);

	if (m_textureOverride >= 0)
		texture = m_textureOverride;

	// @TODO: Clean up a bit more

	float texX = float(16 * (texture % 16));
	float texY = float(16 * (texture / 16));

	float texU_1 = texX * C_RATIO;
	float texU_2 = (texX + 15.99f) * C_RATIO;
	float texV_1 = (texY * C_RATIO) + 1.0f / 32.0f;
	float texV_2 = (texY + 15.99f) * C_RATIO;

	float x1 = pos.x + C_HALF_TILE, z1 = pos.z + C_HALF_TILE;
	float x2 = x1 + (float)(a * C_TOP_SKEW_RATIO);
	float z2 = z1 + (float)(b * C_TOP_SKEW_RATIO);

	Tesselator& t = Tesselator::instance;
	
	// Top side (flame)
	float x_1 = x2 - C_ONE_PIXEL; 
	float x_2 = x2 + C_ONE_PIXEL;
	float z_1 = z2 - C_ONE_PIXEL;
	float z_2 = z2 + C_ONE_PIXEL;

	// Edges (close by)
	float x_3 = x1 - C_ONE_PIXEL;
	float x_4 = x1 + C_ONE_PIXEL;
	float z_5 = z1 - C_ONE_PIXEL;
	float z_6 = z1 + C_ONE_PIXEL;
	
	// Far edges
	float x_5 = x1 - C_HALF_TILE; 
	float x_6 = x1 + C_HALF_TILE;
	float z_3 = z1 - C_HALF_TILE;
	float z_4 = z1 + C_HALF_TILE;
	
	float x_7 = x_6 + a; // Skewed bottom
	float x_8 = x_3 + a;
	float x_9 = x_4 + a;
	float x_0 = x_5 + a;
	float z_7 = z_3 + b;
	float z_8 = z_4 + b;
	float z_9 = z_5 + b;
	float z_0 = z_6 + b;

	float y_1 = pos.y + C_ONE_PIXEL * 10.0f; // Torch height
	float y_2 = pos.y + 1.0f; // Top
	float y_3 = pos.y + 0.0f; // Bottom

	float texU_3 = texU_1 + 0.027344f;
	float texU_4 = texU_1 + 0.035156f;
	float texV_3 = texY * C_RATIO;
	float texV_4 = texY * C_RATIO + 0.023438f;

	// Top
	t.vertexUV(x_1, y_1, z_1, texU_3, texV_4);
	t.vertexUV(x_1, y_1, z_2, texU_3, texV_1);
	t.vertexUV(x_2, y_1, z_2, texU_4, texV_1);
	t.vertexUV(x_2, y_1, z_1, texU_4, texV_4);
	// Sides
	t.vertexUV(x_3, y_2, z_3, texU_1, texV_3);
	t.vertexUV(x_8, y_3, z_7, texU_1, texV_2);
	t.vertexUV(x_8, y_3, z_8, texU_2, texV_2);
	t.vertexUV(x_3, y_2, z_4, texU_2, texV_3);
	
	t.vertexUV(x_4, y_2, z_4, texU_1, texV_3);
	t.vertexUV(x_9, y_3, z_8, texU_1, texV_2);
	t.vertexUV(x_9, y_3, z_7, texU_2, texV_2);
	t.vertexUV(x_4, y_2, z_3, texU_2, texV_3);
	
	t.vertexUV(x_5, y_2, z_6, texU_1, texV_3);
	t.vertexUV(x_0, y_3, z_0, texU_1, texV_2);
	t.vertexUV(x_7, y_3, z_0, texU_2, texV_2);
	t.vertexUV(x_6, y_2, z_6, texU_2, texV_3);
	
	t.vertexUV(x_6, y_2, z_5, texU_1, texV_3);
	t.vertexUV(x_7, y_3, z_9, texU_1, texV_2);
	t.vertexUV(x_0, y_3, z_9, texU_2, texV_2);
	t.vertexUV(x_5, y_2, z_5, texU_2, texV_3);
}

bool TileRenderer::tesselateTorchInWorld(Tile* tile, const TilePos& pos)
{
	int data = m_pLevelSource->getData(pos);
	float bright = tile->getBrightness(m_pLevelSource, pos);

	if (Tile::lightEmission[tile->m_ID] > 0)
		bright = 1.0f;

	Tesselator& t = Tesselator::instance;
	t.color(bright, bright, bright);

	switch (data)
	{
		case 1:
			tesselateTorch(tile, Vec3(float(pos.x) - 0.1f, float(pos.y) + 0.2f, float(pos.z)), -0.4f, 0.0f);
			break;
		case 2:
			tesselateTorch(tile, Vec3(float(pos.x) + 0.1f, float(pos.y) + 0.2f, float(pos.z)), 0.4f, 0.0f);
			break;
		case 3:
			tesselateTorch(tile, Vec3(float(pos.x), float(pos.y) + 0.2f, float(pos.z) - 0.1f), 0.0f, -0.4f);
			break;
		case 4:
			tesselateTorch(tile, Vec3(float(pos.x), float(pos.y) + 0.2f, float(pos.z) + 0.1f), 0.0f, 0.4f);
			break;
		default:
			tesselateTorch(tile, pos, 0.0f, 0.0f);
			break;
	}

	return true;
}

bool TileRenderer::tesselateLadderInWorld(Tile* tile, const TilePos& pos)
{
	constexpr float C_RATIO = 1.0f / 256.0f;

	Tesselator& t = Tesselator::instance;

	int texture = tile->getTexture(Facing::DOWN);

	if (m_textureOverride >= 0)
		texture = m_textureOverride;

	float bright = m_pLevelSource->getBrightness(pos);
	t.color(bright, bright, bright);

	float texX = float(16 * (texture % 16));
	float texY = float(16 * (texture / 16));

	float texU_1 = C_RATIO * texX;
	float texU_2 = C_RATIO * (texX + 15.99f);
	float texV_1 = C_RATIO * texY;
	float texV_2 = C_RATIO * (texY + 15.99f);

	switch (m_pLevelSource->getData(pos))
	{
		case 2:
			t.vertexUV(float(pos.x + 1), float(pos.y + 1), float(pos.z + 1) - 0.05f, texU_1, texV_1);
			t.vertexUV(float(pos.x + 1), float(pos.y + 0), float(pos.z + 1) - 0.05f, texU_1, texV_2);
			t.vertexUV(float(pos.x + 0), float(pos.y + 0), float(pos.z + 1) - 0.05f, texU_2, texV_2);
			t.vertexUV(float(pos.x + 0), float(pos.y + 1), float(pos.z + 1) - 0.05f, texU_2, texV_1);
			break;
		case 3:
			t.vertexUV(float(pos.x + 1), float(pos.y + 0), float(pos.z) + 0.05f, texU_2, texV_2);
			t.vertexUV(float(pos.x + 1), float(pos.y + 1), float(pos.z) + 0.05f, texU_2, texV_1);
			t.vertexUV(float(pos.x + 0), float(pos.y + 1), float(pos.z) + 0.05f, texU_1, texV_1);
			t.vertexUV(float(pos.x + 0), float(pos.y + 0), float(pos.z) + 0.05f, texU_1, texV_2);
			break;
		case 4:
			t.vertexUV(float(pos.x + 1) - 0.05f, float(pos.y + 0), float(pos.z + 1), texU_2, texV_2);
			t.vertexUV(float(pos.x + 1) - 0.05f, float(pos.y + 1), float(pos.z + 1), texU_2, texV_1);
			t.vertexUV(float(pos.x + 1) - 0.05f, float(pos.y + 1), float(pos.z + 0), texU_1, texV_1);
			t.vertexUV(float(pos.x + 1) - 0.05f, float(pos.y + 0), float(pos.z + 0), texU_1, texV_2);
			break;
		case 5:
			t.vertexUV(float(pos.x) + 0.05f, float(pos.y + 1), float(pos.z + 1), texU_1, texV_1);
			t.vertexUV(float(pos.x) + 0.05f, float(pos.y + 0), float(pos.z + 1), texU_1, texV_2);
			t.vertexUV(float(pos.x) + 0.05f, float(pos.y + 0), float(pos.z + 0), texU_2, texV_2);
			t.vertexUV(float(pos.x) + 0.05f, float(pos.y + 1), float(pos.z + 0), texU_2, texV_1);
			break;
	}

	return true;
}

bool TileRenderer::tesselateLeverInWorld(Tile* tt, const TilePos& pos) {
	int data = m_pLevelSource->getData(pos);
	int dir = data & 7;
	bool flipped = (data & 8) > 0;

	Tesselator& t = Tesselator::instance;
	bool hadFixed = m_textureOverride >= 0;
	if (!hadFixed) {
		m_textureOverride = Tile::cobblestone->m_TextureFrame;
	}

	float w1 = 0.25f;
	float w2 = 0.1875f;
	float h = 0.1875f;

	if (dir == 5) {
		tt->setShape(0.5f - w2, 0.0f, 0.5f - w1, 0.5f + w2, h, 0.5f + w1);
	}
	else if (dir == 6) {
		tt->setShape(0.5f - w1, 0.0f, 0.5f - w2, 0.5f + w1, h, 0.5f + w2);
	}
	else if (dir == 4) {
		tt->setShape(0.5f - w2, 0.5f - w1, 1.0f - h, 0.5f + w2, 0.5f + w1, 1.0f);
	}
	else if (dir == 3) {
		tt->setShape(0.5f - w2, 0.5f - w1, 0.0f, 0.5f + w2, 0.5f + w1, h);
	}
	else if (dir == 2) {
		tt->setShape(1.0f - h, 0.5f - w1, 0.5f - w2, 1.0f, 0.5f + w1, 0.5f + w2);
	}
	else if (dir == 1) {
		tt->setShape(0.0f, 0.5f - w1, 0.5f - w2, h, 0.5f + w1, 0.5f + w2);
	}

	tesselateBlockInWorld(tt, pos);

	if (!hadFixed) {
		m_textureOverride = -1;
	}

	float br = tt->getBrightness(m_pLevelSource, pos);
	if (Tile::lightEmission[tt->m_ID] > 0) {
		br = 1.0f;
	}

	t.color(br, br, br);

	int tex = tt->getTexture(Facing::DOWN);
	if (m_textureOverride >= 0) {
		tex = m_textureOverride;
	}

	int xt = (tex & 15) << 4;
	int yt = tex & 240;

	float u0 = xt / 256.0f;
	float u1 = (xt + 15.99f) / 256.0f;
	float v0 = yt / 256.0f;
	float v1 = (yt + 15.99f) / 256.0f;

	float xv = 0.0625f;
	float zv = 0.0625f;
	float yv = 0.625f;

	Vec3 corners[8] = {
		Vec3(-xv, 0.0f, -zv),
		Vec3(xv, 0.0f, -zv),
		Vec3(xv, 0.0f,  zv),
		Vec3(-xv, 0.0f,  zv),
		Vec3(-xv, yv, -zv),
		Vec3(xv, yv, -zv),
		Vec3(xv, yv,  zv),
		Vec3(-xv, yv,  zv)
	};

	for (auto& corner : corners) {
		if (flipped) {
			corner.z -= 0.0625f;
			corner.xRot(0.69813174f);
		}
		else {
			corner.z += 0.0625f;
			corner.xRot(-0.69813174f);
		}

		if (dir == 6) {
			corner.yRot(M_PI_2);
		}

		if (dir < 5) {
			corner.y -= 0.375f;
			corner.xRot(M_PI_2);
			if (dir == 4) corner.yRot(0.0f);
			if (dir == 3) corner.yRot(M_PI);
			if (dir == 2) corner.yRot(M_PI_2);
			if (dir == 1) corner.yRot(-M_PI_2);

			corner.x += pos.x + 0.5;
			corner.y += pos.y + 0.5f;
			corner.z += pos.z + 0.5;
		}
		else {
			corner.x += pos.x + 0.5;
			corner.y += pos.y + 0.125f;
			corner.z += pos.z + 0.5;
		}
	}

	for (int i = 0; i < 6; ++i) {
		Vec3 c0;
		Vec3 c1;
		Vec3 c2;
		Vec3 c3;

		if (i == Facing::DOWN || i == Facing::NORTH) {
			u0 = (xt + 7) / 256.0f;
			u1 = (xt + 9 - 0.01f) / 256.0f;
			v0 = (yt + 6) / 256.0f;
			v1 = (i == 0) ? (yt + 8 - 0.01f) / 256.0f : (yt + 16 - 0.01f) / 256.0f;
		}

		switch (i) {
		case 0: c0 = corners[0]; c1 = corners[1]; c2 = corners[2]; c3 = corners[3]; break;
		case 1: c0 = corners[7]; c1 = corners[6]; c2 = corners[5]; c3 = corners[4]; break;
		case 2: c0 = corners[1]; c1 = corners[0]; c2 = corners[4]; c3 = corners[5]; break;
		case 3: c0 = corners[2]; c1 = corners[1]; c2 = corners[5]; c3 = corners[6]; break;
		case 4: c0 = corners[3]; c1 = corners[2]; c2 = corners[6]; c3 = corners[7]; break;
		case 5: c0 = corners[0]; c1 = corners[3]; c2 = corners[7]; c3 = corners[4]; break;
		}

		t.vertexUV(c0.x, c0.y, c0.z, u0, v1);
		t.vertexUV(c1.x, c1.y, c1.z, u1, v1);
		t.vertexUV(c2.x, c2.y, c2.z, u1, v0);
		t.vertexUV(c3.x, c3.y, c3.z, u0, v0);
	}

	return true;
}


bool TileRenderer::tesselateFireInWorld(Tile* tile, const TilePos& pos)
{
	constexpr float C_RATIO = 1.0f / 256.0f;

	Tesselator& t = Tesselator::instance;

	int texture = tile->getTexture(Facing::DOWN);
	float bright = tile->getBrightness(m_pLevelSource, pos);

	t.color(bright, bright, bright);

	FireTile* pFireTile = (FireTile*)Tile::fire;

	float texX = float(16 * (texture % 16));
	float texY = float(16 * (texture / 16));

	float texU_1 = C_RATIO * (texX);
	float texU_2 = C_RATIO * (texX + 15.99f);
	float texV_1 = C_RATIO * (texY);
	float texV_2 = C_RATIO * (texY + 15.99f);
	float xf = float(pos.x), yf = float(pos.y), zf = float(pos.z);

	if (m_pLevelSource->isSolidTile(pos.below()) || pFireTile->canBurn(m_pLevelSource, pos.below()))
	{
		t.vertexUV(xf + 0.5f - 0.3f, yf + 1.4f, zf + 1.0f, texU_2, texV_1);
		t.vertexUV(xf + 0.5f + 0.2f, yf + 0.0f, zf + 1.0f, texU_2, texV_2);
		t.vertexUV(xf + 0.5f + 0.2f, yf + 0.0f, zf + 0.0f, texU_1, texV_2);
		t.vertexUV(xf + 0.5f - 0.3f, yf + 1.4f, zf + 0.0f, texU_1, texV_1);

		t.vertexUV(xf + 0.5f + 0.3f, yf + 1.4f, zf + 0.0f, texU_2, texV_1);
		t.vertexUV(xf + 0.5f - 0.2f, yf + 0.0f, zf + 0.0f, texU_2, texV_2);
		t.vertexUV(xf + 0.5f - 0.2f, yf + 0.0f, zf + 1.0f, texU_1, texV_2);
		t.vertexUV(xf + 0.5f + 0.3f, yf + 1.4f, zf + 1.0f, texU_1, texV_1);

		t.vertexUV(xf + 1.0f, yf + 1.4f, zf + 0.5f + 0.3f, texU_2, texV_1);
		t.vertexUV(xf + 1.0f, yf + 0.0f, zf + 0.5f - 0.2f, texU_2, texV_2);
		t.vertexUV(xf + 0.0f, yf + 0.0f, zf + 0.5f - 0.2f, texU_1, texV_2);
		t.vertexUV(xf + 0.0f, yf + 1.4f, zf + 0.5f + 0.3f, texU_1, texV_1);

		t.vertexUV(xf + 0.0f, yf + 1.4f, zf + 0.5f - 0.3f, texU_2, texV_1);
		t.vertexUV(xf + 0.0f, yf + 0.0f, zf + 0.5f + 0.2f, texU_2, texV_2);
		t.vertexUV(xf + 1.0f, yf + 0.0f, zf + 0.5f + 0.2f, texU_1, texV_2);
		t.vertexUV(xf + 1.0f, yf + 1.4f, zf + 0.5f - 0.3f, texU_1, texV_1);

		t.vertexUV(xf + 0.5f - 0.4f, yf + 1.4f, zf + 0.0f, texU_1, texV_1);
		t.vertexUV(xf + 0.5f - 0.5f, yf + 0.0f, zf + 0.0f, texU_1, texV_2);
		t.vertexUV(xf + 0.5f - 0.5f, yf + 0.0f, zf + 1.0f, texU_2, texV_2);
		t.vertexUV(xf + 0.5f - 0.4f, yf + 1.4f, zf + 1.0f, texU_2, texV_1);

		t.vertexUV(xf + 0.5f + 0.4f, yf + 1.4f, zf + 1.0f, texU_1, texV_1);
		t.vertexUV(xf + 0.5f + 0.5f, yf + 0.0f, zf + 1.0f, texU_1, texV_2);
		t.vertexUV(xf + 0.5f + 0.5f, yf + 0.0f, zf + 0.0f, texU_2, texV_2);
		t.vertexUV(xf + 0.5f + 0.4f, yf + 1.4f, zf + 0.0f, texU_2, texV_1);

		t.vertexUV(xf + 0.0f, yf + 1.4f, zf + 0.5f + 0.4f, texU_1, texV_1);
		t.vertexUV(xf + 0.0f, yf + 0.0f, zf + 0.5f + 0.5f, texU_1, texV_2);
		t.vertexUV(xf + 1.0f, yf + 0.0f, zf + 0.5f + 0.5f, texU_2, texV_2);
		t.vertexUV(xf + 1.0f, yf + 1.4f, zf + 0.5f + 0.4f, texU_2, texV_1);

		t.vertexUV(xf + 1.0f, yf + 1.4f, zf + 0.5f - 0.4f, texU_1, texV_1);
		t.vertexUV(xf + 1.0f, yf + 0.0f, zf + 0.5f - 0.5f, texU_1, texV_2);
		t.vertexUV(xf + 0.0f, yf + 0.0f, zf + 0.5f - 0.5f, texU_2, texV_2);
		t.vertexUV(xf + 0.0f, yf + 1.4f, zf + 0.5f - 0.4f, texU_2, texV_1);

		return true;
	}

	if (((pos.y / 2 + pos.x / 2 + pos.z / 2) & 1) == 0)
		std::swap(texU_1, texU_2);

	if (pFireTile->canBurn(m_pLevelSource, pos.west()))
	{
		t.vertexUV(xf + 0.2f, (yf + 1.4f) + 0.0625f, zf + 1.0f, texU_1, texV_1);
		t.vertexUV(xf + 0.0f, (yf + 0.0f) + 0.0625f, zf + 1.0f, texU_1, texV_2);
		t.vertexUV(xf + 0.0f, (yf + 0.0f) + 0.0625f, zf + 0.0f, texU_2, texV_2);
		t.vertexUV(xf + 0.2f, (yf + 1.4f) + 0.0625f, zf + 0.0f, texU_2, texV_1);
		t.vertexUV(xf + 0.2f, (yf + 1.4f) + 0.0625f, zf + 0.0f, texU_2, texV_1);
		t.vertexUV(xf + 0.0f, (yf + 0.0f) + 0.0625f, zf + 0.0f, texU_2, texV_2);
		t.vertexUV(xf + 0.0f, (yf + 0.0f) + 0.0625f, zf + 1.0f, texU_1, texV_2);
		t.vertexUV(xf + 0.2f, (yf + 1.4f) + 0.0625f, zf + 1.0f, texU_1, texV_1);
	}

	if (pFireTile->canBurn(m_pLevelSource, pos.east()))
	{
		t.vertexUV(xf + 1.0f - 0.2f, (pos.y + 1.4f) + 0.0625f, pos.z + 0.0f, texU_2, texV_1);
		t.vertexUV(xf + 1.0f + 0.0f, (pos.y + 0.0f) + 0.0625f, pos.z + 0.0f, texU_2, texV_2);
		t.vertexUV(xf + 1.0f + 0.0f, (pos.y + 0.0f) + 0.0625f, pos.z + 1.0f, texU_1, texV_2);
		t.vertexUV(xf + 1.0f - 0.2f, (pos.y + 1.4f) + 0.0625f, pos.z + 1.0f, texU_1, texV_1);
		t.vertexUV(xf + 1.0f - 0.2f, (pos.y + 1.4f) + 0.0625f, pos.z + 1.0f, texU_1, texV_1);
		t.vertexUV(xf + 1.0f + 0.0f, (pos.y + 0.0f) + 0.0625f, pos.z + 1.0f, texU_1, texV_2);
		t.vertexUV(xf + 1.0f + 0.0f, (pos.y + 0.0f) + 0.0625f, pos.z + 0.0f, texU_2, texV_2);
		t.vertexUV(xf + 1.0f - 0.2f, (pos.y + 1.4f) + 0.0625f, pos.z + 0.0f, texU_2, texV_1);
	}

	if (pFireTile->canBurn(m_pLevelSource, pos - 1))
	{
		t.vertexUV(pos.x + 0.0f, (pos.y + 1.4f) + 0.0625f, pos.z + 0.2f, texU_1, texV_1);
		t.vertexUV(pos.x + 0.0f, (pos.y + 0.0f) + 0.0625f, pos.z + 0.0f, texU_1, texV_2);
		t.vertexUV(pos.x + 1.0f, (pos.y + 0.0f) + 0.0625f, pos.z + 0.0f, texU_2, texV_2);
		t.vertexUV(pos.x + 1.0f, (pos.y + 1.4f) + 0.0625f, pos.z + 0.2f, texU_2, texV_1);
		t.vertexUV(pos.x + 1.0f, (pos.y + 1.4f) + 0.0625f, pos.z + 0.2f, texU_2, texV_1);
		t.vertexUV(pos.x + 1.0f, (pos.y + 0.0f) + 0.0625f, pos.z + 0.0f, texU_2, texV_2);
		t.vertexUV(pos.x + 0.0f, (pos.y + 0.0f) + 0.0625f, pos.z + 0.0f, texU_1, texV_2);
		t.vertexUV(pos.x + 0.0f, (pos.y + 1.4f) + 0.0625f, pos.z + 0.2f, texU_1, texV_1);
	}

	if (pFireTile->canBurn(m_pLevelSource, pos + 1))
	{
		t.vertexUV(pos.x + 1.0f, (pos.y + 1.4f) + 0.0625f, pos.z + 1.0f - 0.2f, texU_2, texV_1);
		t.vertexUV(pos.x + 1.0f, (pos.y + 0.0f) + 0.0625f, pos.z + 1.0f + 0.0f, texU_2, texV_2);
		t.vertexUV(pos.x + 0.0f, (pos.y + 0.0f) + 0.0625f, pos.z + 1.0f + 0.0f, texU_1, texV_2);
		t.vertexUV(pos.x + 0.0f, (pos.y + 1.4f) + 0.0625f, pos.z + 1.0f - 0.2f, texU_1, texV_1);
		t.vertexUV(pos.x + 0.0f, (pos.y + 1.4f) + 0.0625f, pos.z + 1.0f - 0.2f, texU_1, texV_1);
		t.vertexUV(pos.x + 0.0f, (pos.y + 0.0f) + 0.0625f, pos.z + 1.0f + 0.0f, texU_1, texV_2);
		t.vertexUV(pos.x + 1.0f, (pos.y + 0.0f) + 0.0625f, pos.z + 1.0f + 0.0f, texU_2, texV_2);
		t.vertexUV(pos.x + 1.0f, (pos.y + 1.4f) + 0.0625f, pos.z + 1.0f - 0.2f, texU_2, texV_1);
	}

	if (pFireTile->canBurn(m_pLevelSource, pos.above()))
	{
		// @NOTE: Converting z and x to uint8_t for whatever reason
		if (((uint8_t(pos.z) + uint8_t(pos.x) + pos.y + 1) & 1) != 0)
		{
			float y1f = float(pos.y + 1);
			t.vertexUV(xf + 0.0f, y1f - 0.2f, zf + 0.5f + 0.5f, texU_2, texV_1);
			t.vertexUV(xf + 0.0f, y1f + 0.0f, zf + 0.5f - 0.5f, texU_2, texV_2);
			t.vertexUV(xf + 1.0f, y1f + 0.0f, zf + 0.5f - 0.5f, texU_1, texV_2);
			t.vertexUV(xf + 1.0f, y1f - 0.2f, zf + 0.5f + 0.5f, texU_1, texV_1);
			t.vertexUV(xf + 1.0f, y1f - 0.2f, zf + 0.5f - 0.5f, texU_2, texV_1);
			t.vertexUV(xf + 1.0f, y1f + 0.0f, zf + 0.5f + 0.5f, texU_2, texV_2);
			t.vertexUV(xf + 0.0f, y1f + 0.0f, zf + 0.5f + 0.5f, texU_1, texV_2);
			t.vertexUV(xf + 0.0f, y1f - 0.2f, zf + 0.5f - 0.5f, texU_1, texV_1);
		}
		else
		{
			float y1f = float(pos.y + 1);
			t.vertexUV(xf + 0.0f, y1f - 0.2f, zf + 0.0f, texU_2, texV_1);
			t.vertexUV(xf + 1.0f, y1f + 0.0f, zf + 0.0f, texU_2, texV_2);
			t.vertexUV(xf + 1.0f, y1f + 0.0f, zf + 1.0f, texU_1, texV_2);
			t.vertexUV(xf + 0.0f, y1f - 0.2f, zf + 1.0f, texU_1, texV_1);
			t.vertexUV(xf + 1.0f, y1f - 0.2f, zf + 1.0f, texU_2, texV_1);
			t.vertexUV(xf + 0.0f, y1f + 0.0f, zf + 1.0f, texU_2, texV_2);
			t.vertexUV(xf + 0.0f, y1f + 0.0f, zf + 0.0f, texU_1, texV_2);
			t.vertexUV(xf + 1.0f, y1f - 0.2f, zf + 0.0f, texU_1, texV_1);
		}
	}
	
	// @NOTE: Returning true when you may not necessarily have rendered something?
	return true;
}

bool TileRenderer::tesselateBedInWorld(Tile* tile, const TilePos& pos)
{
	Tesselator& t = Tesselator::instance;
	int var6 = m_pLevelSource->getData(pos);
	int var7 = BedTile::getDirectionFromData(var6);
	bool var8 = BedTile::isHead(var6);
	float var9 = 0.5F;
	float var10 = 1.0F;
	float var11 = 0.8F;
	float var12 = 0.6F;
	float fLightHere = tile->getBrightness(m_pLevelSource, pos);
	t.color(var9 * fLightHere, var9 * fLightHere, var9 * fLightHere);
	int hiddenFace = Tile::wood->m_TextureFrame;
	int var27 = (hiddenFace & 15) << 4;
	int var28 = hiddenFace & 240;
	float var29 = ((float)var27 / 256.0F);
	float var31 = ((var27 + 16) - 0.01) / 256.0;
	float var33 = ((float)var28 / 256.0F);
	float var35 = ((var28 + 16) - 0.01) / 256.0;
	real var37 = pos.x + tile->m_aabb.min.x;
	real var39 = pos.x + tile->m_aabb.max.x;
	real var41 = pos.y + tile->m_aabb.min.y + 0.1875;
	real var43 = pos.z + tile->m_aabb.min.z;
	real var45 = pos.z + tile->m_aabb.max.z;
	bool z9 = m_textureOverride >= 0;
	if (!z9) {
		t.vertexUV(var37, var41, var45, var29, var35);
		t.vertexUV(var37, var41, var43, var29, var33);
		t.vertexUV(var39, var41, var43, var31, var33);
		t.vertexUV(var39, var41, var45, var31, var35);
	}

	float var64 = tile->getBrightness(m_pLevelSource, pos.above());
	t.color(var10 * var64, var10 * var64, var10 * var64);
	var27 = tile->getTexture(m_pLevelSource, pos, Facing::UP);
	var28 = (var27 & 15) << 4;
	int var67 = var27 & 240;
	hiddenFace = BedTile::hiddenFace[var7];
	if (var8) {
		hiddenFace = BedTile::hiddenFace[BedTile::hiddenFaceIndex[var7]];
	}

	Facing::Name flippedFace = Facing::WEST;
	uint8_t upRot = 2;
	switch (var7) {
	case 0:
		flippedFace = Facing::EAST;
		upRot = 1;
		break;
	case 1:
		flippedFace = Facing::SOUTH;
		upRot = 3;
	case 2:
	default:
		break;
	case 3:
		flippedFace = Facing::NORTH;
		upRot = 0;
	}

	bool bDrewAnything = false;

	for (Facing::Name face : Facing::ALL)
	{
		if (hiddenFace == face || face == Facing::DOWN) continue;

		TilePos neighborPos = pos.relative(face);

		if (!m_bDisableCulling && !tile->shouldRenderFace(m_pLevelSource, neighborPos, face))
			continue;

		bDrewAnything = true;

		float light = tile->getBrightness(m_pLevelSource, neighborPos);

		switch (face) {
		case Facing::UP:
			if (tile->m_aabb.max.y != 1.0f && !tile->m_pMaterial->isLiquid())
				light = fLightHere;
			break;
		case Facing::NORTH:
			if (tile->m_aabb.min.z > 0.0f)
				light = fLightHere;
			break;
		case Facing::SOUTH:
			if (tile->m_aabb.max.z < 1.0f)
				light = fLightHere;
			break;
		case Facing::WEST:
			if (tile->m_aabb.min.x > 0.0f)
				light = fLightHere;
			break;
		case Facing::EAST:
			if (tile->m_aabb.max.x < 1.0f)
				light = fLightHere;
			break;
		default:
			break;
		}

		int texture = tile->getTexture(m_pLevelSource, pos, face);

		if (flippedFace == face) texture = -texture;
		float red, grn, blue;
		computeColor(tile->getColor(m_pLevelSource, pos, face, texture), red, grn, blue, Facing::LIGHT[face] * light);

		renderFace(tile, pos, texture, face, red, grn, blue, face == Facing::UP ? upRot : 0);
	}

	return bDrewAnything;
}

bool TileRenderer::tesselateDustInWorld(Tile* tile, const TilePos& pos)
{
	Tesselator& t = Tesselator::instance;
	int data = m_pLevelSource->getData(pos);
	int tex = tile->getTexture(Facing::UP, data);

	if (m_textureOverride >= 0) {
		tex = m_textureOverride;
	}

	float br = tile->getBrightness(m_pLevelSource, pos);
	float power = data / 15.0F;
	float red = power * 0.6F + 0.4F;
	if (data == 0) {
		red = 0.3F;
	}

	float green = Mth::Max(0.0f, power * power * 0.7F - 0.5F);
	float blue = Mth::Max(0.0f, power * power * 0.6F - 0.7F);
	t.color(br * red, br * green, br * blue);

	int xt = (tex & 15) << 4;
	int yt = tex & 240;

	double u0 = xt / 256.0;
	double u1 = (xt + 15.99) / 256.0;
	double v0 = yt / 256.0;
	double v1 = (yt + 15.99) / 256.0;

	constexpr float o = 0.0f;
	constexpr float r = 0.03125f;

	TilePos above = pos.above();
	TilePos west = pos.west();
	TilePos east = pos.east();
	TilePos north = pos.north();
	TilePos south = pos.south();

	bool w = RedStoneDustTile::shouldConnectTo(m_pLevelSource, west, 1) || (!m_pLevelSource->isSolidTile(west) && RedStoneDustTile::shouldConnectTo(m_pLevelSource, west.below(), -1));
	bool e = RedStoneDustTile::shouldConnectTo(m_pLevelSource, east, 3) || (!m_pLevelSource->isSolidTile(east) && RedStoneDustTile::shouldConnectTo(m_pLevelSource, east.below(), -1));
	bool n = RedStoneDustTile::shouldConnectTo(m_pLevelSource, north, 2) || (!m_pLevelSource->isSolidTile(north) && RedStoneDustTile::shouldConnectTo(m_pLevelSource, north.below(), -1));
	bool s = RedStoneDustTile::shouldConnectTo(m_pLevelSource, south, 0) || (!m_pLevelSource->isSolidTile(south) && RedStoneDustTile::shouldConnectTo(m_pLevelSource, south.below(), -1));

	if (!m_pLevelSource->isSolidTile(above)) {
		if (m_pLevelSource->isSolidTile(west) && RedStoneDustTile::shouldConnectTo(m_pLevelSource, west.above(), -1)) w = true;
		if (m_pLevelSource->isSolidTile(east) && RedStoneDustTile::shouldConnectTo(m_pLevelSource, east.above(), -1)) e = true;
		if (m_pLevelSource->isSolidTile(north) && RedStoneDustTile::shouldConnectTo(m_pLevelSource, north.above(), -1)) n = true;
		if (m_pLevelSource->isSolidTile(south) && RedStoneDustTile::shouldConnectTo(m_pLevelSource, south.above(), -1)) s = true;
	}

	constexpr float d = 0.3125f;
	constexpr float tOff = 1 / 16.0f;
	float x0 = float(pos.x);
	float x1 = x0 + 1.0f;
	float z0 = float(pos.z);
	float z1 = z0 + 1.0f;

	int pic = 0;
	if ((w || e) && !n && !s) pic = 1;
	if ((n || s) && !e && !w) pic = 2;

	if (pic != 0) {
		u0 = (xt + 16) / 256.0;
		u1 = (xt + 16 + 15.99) / 256.0;
	}

	if (pic == 0) {
		if (w || e || n || s) {
			if (!w) { x0 += d; u0 += d / 16.0; }
			if (!e) { x1 -= d; u1 -= d / 16.0; }
			if (!n) { z0 += d; v0 += d / 16.0; }
			if (!s) { z1 -= d; v1 -= d / 16.0; }
		}

		t.vertexUV(x1 + o, pos.y + r, z1 + o, u1, v1);
		t.vertexUV(x1 + o, pos.y + r, z0 - o, u1, v0);
		t.vertexUV(x0 - o, pos.y + r, z0 - o, u0, v0);
		t.vertexUV(x0 - o, pos.y + r, z1 + o, u0, v1);
		t.color(br, br, br);
		t.vertexUV(x1 + o, pos.y + r, z1 + o, u1, v1 + tOff);
		t.vertexUV(x1 + o, pos.y + r, z0 - o, u1, v0 + tOff);
		t.vertexUV(x0 - o, pos.y + r, z0 - o, u0, v0 + tOff);
		t.vertexUV(x0 - o, pos.y + r, z1 + o, u0, v1 + tOff);
	}

	if (pic == 1) {
		t.vertexUV(x1 + o, pos.y + r, z1 + o, u1, v1);
		t.vertexUV(x1 + o, pos.y + r, z0 - o, u1, v0);
		t.vertexUV(x0 - o, pos.y + r, z0 - o, u0, v0);
		t.vertexUV(x0 - o, pos.y + r, z1 + o, u0, v1);
		t.color(br, br, br);
		t.vertexUV(x1 + o, pos.y + r, z1 + o, u1, v1 + tOff);
		t.vertexUV(x1 + o, pos.y + r, z0 - o, u1, v0 + tOff);
		t.vertexUV(x0 - o, pos.y + r, z0 - o, u0, v0 + tOff);
		t.vertexUV(x0 - o, pos.y + r, z1 + o, u0, v1 + tOff);
	}

	if (pic == 2) {
		t.vertexUV(x1 + o, pos.y + r, z1 + o, u1, v1);
		t.vertexUV(x1 + o, pos.y + r, z0 - o, u0, v1);
		t.vertexUV(x0 - o, pos.y + r, z0 - o, u0, v0);
		t.vertexUV(x0 - o, pos.y + r, z1 + o, u1, v0);
		t.color(br, br, br);
		t.vertexUV(x1 + o, pos.y + r, z1 + o, u1, v1 + tOff);
		t.vertexUV(x1 + o, pos.y + r, z0 - o, u0, v1 + tOff);
		t.vertexUV(x0 - o, pos.y + r, z0 - o, u0, v0 + tOff);
		t.vertexUV(x0 - o, pos.y + r, z1 + o, u1, v0 + tOff);
	}

	u0 = (xt + 16) / 256.0;
	u1 = (xt + 16 + 15.99) / 256.0;
	v0 = yt / 256.0;
	v1 = (yt + 15.99) / 256.0;

	if (!m_pLevelSource->isSolidTile(above)) {
		if (m_pLevelSource->isSolidTile(west) && m_pLevelSource->getTile(west.above()) == Tile::redstoneDust->m_ID) {
			t.color(br * red, br * green, br * blue);
			t.vertexUV(pos.x + r, pos.y + 1 + o, pos.z + 1 + o, u1, v0);
			t.vertexUV(pos.x + r, pos.y - o, pos.z + 1 + o, u0, v0);
			t.vertexUV(pos.x + r, pos.y - o, pos.z - o, u0, v1);
			t.vertexUV(pos.x + r, pos.y + 1 + o, pos.z - o, u1, v1);
			t.color(br, br, br);
			t.vertexUV(pos.x + r, pos.y + 1 + o, pos.z + 1 + o, u1, v0 + tOff);
			t.vertexUV(pos.x + r, pos.y - o, pos.z + 1 + o, u0, v0 + tOff);
			t.vertexUV(pos.x + r, pos.y - o, pos.z - o, u0, v1 + tOff);
			t.vertexUV(pos.x + r, pos.y + 1 + o, pos.z - o, u1, v1 + tOff);
		}

		if (m_pLevelSource->isSolidTile(east) && m_pLevelSource->getTile(east.above()) == Tile::redstoneDust->m_ID) {
			t.color(br * red, br * green, br * blue);
			t.vertexUV(pos.x + 1 - r,pos.y - o, pos.z + 1 + o, u0, v1);
			t.vertexUV(pos.x + 1 - r, pos.y + 1 + o, pos.z + 1 + o, u1, v1);
			t.vertexUV(pos.x + 1 - r, pos.y + 1 + o, pos.z - o, u1, v0);
			t.vertexUV(pos.x + 1 - r, pos.y - o, pos.z - o, u0, v0);
			t.color(br, br, br);
			t.vertexUV(pos.x + 1 - r, pos.y - o, pos.z + 1 + o, u0, v1 + tOff);
			t.vertexUV(pos.x + 1 - r, pos.y + 1 + o, pos.z + 1 + o, u1, v1 + tOff);
			t.vertexUV(pos.x + 1 - r, pos.y + 1 + o, pos.z - o, u1, v0 + tOff);
			t.vertexUV(pos.x + 1 - r, pos.y - o, pos.z - o, u0, v0 + tOff);
		}

		if (m_pLevelSource->isSolidTile(north) && m_pLevelSource->getTile(north.above()) == Tile::redstoneDust->m_ID) {
			t.color(br * red, br * green, br * blue);
			t.vertexUV(pos.x + 1 + o, pos.y - o, pos.z + r, u0, v1);
			t.vertexUV(pos.x + 1 + o, pos.y + 1 + o, pos.z + r, u1, v1);
			t.vertexUV(pos.x - o, pos.y + 1 + o, pos.z + r, u1, v0);
			t.vertexUV(pos.x - o, pos.y - o, pos.z + r, u0, v0);
			t.color(br, br, br);
			t.vertexUV(pos.x + 1 + o, pos.y - o, pos.z + r, u0, v1 + tOff);
			t.vertexUV(pos.x + 1 + o, pos.y + 1 + o, pos.z + r, u1, v1 + tOff);
			t.vertexUV(pos.x - o, pos.y + 1 + o, pos.z + r, u1, v0 + tOff);
			t.vertexUV(pos.x - o, pos.y - o, pos.z + r, u0, v0 + tOff);
		}

		if (m_pLevelSource->isSolidTile(south) && m_pLevelSource->getTile(south.above()) == Tile::redstoneDust->m_ID) {
			t.color(br * red, br * green, br * blue);
			t.vertexUV(pos.x + 1 + o, pos.y + 1 + o, pos.z + 1 - r, u1, v0);
			t.vertexUV(pos.x + 1 + o, pos.y  - o, pos.z + 1 - r, u0, v0);
			t.vertexUV(pos.x + 0 - o, pos.y  - o, pos.z + 1 - r, u0, v1);
			t.vertexUV(pos.x + 0 - o, pos.y + 1 + o, pos.z + 1 - r, u1, v1);
			t.color(br, br, br);
			t.vertexUV(pos.x + 1 + o, pos.y + 1 + o, pos.z + 1 - r, u1, v0 + tOff);
			t.vertexUV(pos.x + 1 + o, pos.y - o, pos.z + 1 - r, u0, v0 + tOff);
			t.vertexUV(pos.x + 0 - o, pos.y - o, pos.z + 1 - r, u0, v1 + tOff);
			t.vertexUV(pos.x + 0 - o, pos.y + 1 + o, pos.z + 1 - r, u1, v1 + tOff);
		}
	}

	return true;
}

bool TileRenderer::tesselateRailInWorld(Tile* tile, const TilePos& pos)
{
	Tesselator& t = Tesselator::instance;
	int data = m_pLevelSource->getData(pos);
	int tex = tile->getTexture(Facing::DOWN, data);
	if (m_textureOverride >= 0) {
		tex = m_textureOverride;
	}

	if (RailTile::isPowered(tile)) {
		data &= 7;
	}

	float br = tile->getBrightness(m_pLevelSource, pos);
	t.color(br, br, br);
	int xt = (tex & 15) << 4;
	int yt = tex & 240;
	float u0 = xt / 256.0F;
	float u1 = (xt + 15.99F) / 256.0F;
	float v0 = yt / 256.0F;
	float v1 = (yt + 15.99F) / 256.0F;
	float r = 0.0625F;
	float x0 = (float)(pos.x + 1);
	float x1 = (float)(pos.x + 1);
	float x2 = (float)(pos.x + 0);
	float x3 = (float)(pos.x + 0);
	float z0 = (float)(pos.z + 0);
	float z1 = (float)(pos.z + 1);
	float z2 = (float)(pos.z + 1);
	float z3 = (float)(pos.z + 0);
	float y0 = (float)pos.y + r;
	float y1 = (float)pos.y + r;
	float y2 = (float)pos.y + r;
	float y3 = (float)pos.y + r;
	if (data != 1 && data != 2 && data != 3 && data != 7) {
		if (data == 8) {
			x0 = x1 = (float)(pos.x + 0);
			x2 = x3 = (float)(pos.x + 1);
			z0 = z3 = (float)(pos.z + 1);
			z1 = z2 = (float)(pos.z + 0);
		}
		else if (data == 9) {
			x0 = x3 = (float)(pos.x + 0);
			x1 = x2 = (float)(pos.x + 1);
			z0 = z1 = (float)(pos.z + 0);
			z2 = z3 = (float)(pos.z + 1);
		}
	}
	else {
		x0 = x3 = (float)(pos.x + 1);
		x1 = x2 = (float)(pos.x + 0);
		z0 = z1 = (float)(pos.z + 1);
		z2 = z3 = (float)(pos.z + 0);
	}

	if (data != 2 && data != 4) {
		if (data == 3 || data == 5) {
			++y1;
			++y2;
		}
	}
	else {
		++y0;
		++y3;
	}

	t.vertexUV(x0, y0, z0, u1, v0);
	t.vertexUV(x1, y1, z1, u1, v1);
	t.vertexUV(x2, y2, z2, u0, v1);
	t.vertexUV(x3, y3, z3, u0, v0);
	t.vertexUV(x3, y3, z3, u0, v0);
	t.vertexUV(x2, y2, z2, u0, v1);
	t.vertexUV(x1, y1, z1, u1, v1);
	t.vertexUV(x0, y0, z0, u1, v0);
	return true;
}

bool TileRenderer::tesselateRepeaterInWorld(Tile* tile, const TilePos& pos)
{
	int var5 = m_pLevelSource->getData(pos);
	int var6 = var5 & 3;
	int var7 = (var5 & 12) >> 2;
	tesselateBlockInWorld(tile, pos);
	Tesselator& t = Tesselator::instance;
	float var9 = tile->getBrightness(m_pLevelSource, pos);
	if (Tile::lightBlock[tile->m_ID] > 0)
		var9 = (var9 + 1.0F) * 0.5F;

	t.color(var9, var9, var9);
	real var10 = -0.1875;
	real var12 = 0.0;
	real var14 = 0.0;
	real var16 = 0.0;
	real var18 = 0.0;
	switch (var6) {
	case 0:
		var18 = -0.3125;
		var14 = RepeaterTile::repeaterFacing[var7];
		break;
	case 1:
		var16 = 0.3125;
		var12 = -RepeaterTile::repeaterFacing[var7];
		break;
	case 2:
		var18 = 0.3125;
		var14 = -RepeaterTile::repeaterFacing[var7];
		break;
	case 3:
		var16 = -0.3125;
		var12 = RepeaterTile::repeaterFacing[var7];
	}

	tesselateTorch(tile, Vec3(pos.x + var12, pos.y + var10, pos.z + var14), 0.0, 0.0);
	tesselateTorch(tile, Vec3(pos.x + var16, pos.y + var10, pos.z + var18), 0.0, 0.0);
	int var20 = tile->getTexture(Facing::UP);
	int var21 = (var20 & 15) << 4;
	int var22 = var20 & 240;
	real var23 = (real)((float)var21 / 256.0F);
	real var25 = (real)(((float)var21 + 15.99F) / 256.0F);
	real var27 = (real)((float)var22 / 256.0F);
	real var29 = (real)(((float)var22 + 15.99F) / 256.0F);
	float var31 = 2.0F / 16.0F;
	float var32 = (float)(pos.x + 1);
	float var33 = (float)(pos.x + 1);
	float var34 = (float)(pos.x + 0);
	float var35 = (float)(pos.x + 0);
	float var36 = (float)(pos.z + 0);
	float var37 = (float)(pos.z + 1);
	float var38 = (float)(pos.z + 1);
	float var39 = (float)(pos.z + 0);
	float var40 = (float)pos.y + var31;
	if (var6 == 2) {
		var33 = (float)(pos.x + 0);
		var32 = var33;
		var35 = (float)(pos.x + 1);
		var34 = var35;
		var39 = (float)(pos.z + 1);
		var36 = var39;
		var38 = (float)(pos.z + 0);
		var37 = var38;
	}
	else if (var6 == 3) {
		var35 = (float)(pos.x + 0);
		var32 = var35;
		var34 = (float)(pos.x + 1);
		var33 = var34;
		var37 = (float)(pos.z + 0);
		var36 = var37;
		var39 = (float)(pos.z + 1);
		var38 = var39;
	}
	else if (var6 == 1) {
		var35 = (float)(pos.x + 1);
		var32 = var35;
		var34 = (float)(pos.x + 0);
		var33 = var34;
		var37 = (float)(pos.z + 1);
		var36 = var37;
		var39 = (float)(pos.z + 0);
		var38 = var39;
	}

	t.vertexUV(var35, var40, var39, var23, var27);
	t.vertexUV(var34, var40, var38, var23, var29);
	t.vertexUV(var33, var40, var37, var25, var29);
	t.vertexUV(var32, var40, var36, var25, var27);
	return true;
}

bool TileRenderer::tesselateInWorld(Tile* tile, const TilePos& pos)
{
	int shape = tile->getRenderShape();
	tile->updateShape(m_pLevelSource, pos);

	switch (shape)
	{
		case SHAPE_NONE:
			return false;
		default:
		case SHAPE_SOLID:
			return tesselateBlockInWorld(tile, pos);
		case SHAPE_WATER:
			return tesselateWaterInWorld(tile, pos);
		case SHAPE_CROSS:
		case SHAPE_RANDOM_CROSS:
			return tesselateCrossInWorld(tile, pos);
		case SHAPE_CROPS:
			return tesselateRowInWorld(tile, pos);
		case SHAPE_TORCH:
			return tesselateTorchInWorld(tile, pos);
		case SHAPE_FIRE:
			return tesselateFireInWorld(tile, pos);
		case SHAPE_DUST:
			return tesselateDustInWorld(tile, pos);
		case SHAPE_LADDER:
			return tesselateLadderInWorld(tile, pos);
		case SHAPE_RAIL:
			return tesselateRailInWorld(tile, pos);
		case SHAPE_DOOR:
			m_bDisableCulling = true;
			return tesselateBlockInWorld(tile, pos, 1.0f, 1.0f, 1.0f);
			m_bDisableCulling = false;
		case SHAPE_STAIRS:
			return tesselateStairsInWorld(tile, pos);
		case SHAPE_FENCE:
			return tesselateFenceInWorld(tile, pos);
		case SHAPE_LEVER:
			return tesselateLeverInWorld(tile, pos);
		case SHAPE_CACTUS:
			return tesselateBlockInWorld(tile, pos, 1.0f, 1.0f, 1.0f);
		case SHAPE_BED:
			return tesselateBedInWorld(tile, pos);
		case SHAPE_REPEATER:
			return tesselateRepeaterInWorld(tile, pos);
	}

	return false;
}

bool TileRenderer::tesselateInWorldNoCulling(Tile* tile, const TilePos& pos)
{
	m_bDisableCulling = true;
	bool r = tesselateInWorld(tile, pos);
	m_bDisableCulling = false;
	return r;
}

bool TileRenderer::tesselateInWorld(Tile* tile, const TilePos& pos, int a)
{
	m_textureOverride = a;
	bool r = tesselateInWorld(tile, pos);
	m_textureOverride = -1;
	return r;
}

inline void computeTileColor(Tile* tile, int data, Facing::Name facing, int texture, float& red, float& grn, float& blu, float bright) {
	computeColor(tile->getColor(data, facing, texture), red, grn, blu, bright);
}

inline void shadeIfNeeded(Tesselator& t, bool preshade, float red, float grn, float blu, float col) {
	if (preshade)
		t.color(col * red, col * grn, col * blu);
	else
		t.color(red, grn, blu);
}

void TileRenderer::renderTile(Tile* tile, int data, float bright, bool preshade)
{
	Tesselator& t = Tesselator::instance;

#ifndef ENH_SHADE_HELD_TILES
	bright = 1.0f; // 255
#endif
#ifndef USE_GL_NORMAL_LIGHTING
	preshade = true;
#endif

	int shape = tile->getRenderShape();
	switch (shape)
	{
	case SHAPE_SOLID:
	default:
	{
		tile->updateDefaultShape();
		glTranslatef(-0.5f, -0.5f, -0.5f);
		t.begin();

		float red, grn, blu;
		for (int dir = Facing::DOWN; dir <= Facing::EAST; dir++)
		{
			const auto normal = Facing::NORMALS[dir];
			int texture = tile->getTexture((Facing::Name)dir, data);
			computeTileColor(tile, data, (Facing::Name)dir, texture, red, grn, blu, preshade ? bright * Facing::LIGHT[dir] : bright);
			t.normal(normal[0], normal[1], normal[2]);
			renderFace(tile, Vec3::ZERO, tile->getTexture((Facing::Name)dir, data), (Facing::Name)dir, red, grn, blu);
		}
		t.draw();
		glTranslatef(0.5f, 0.5f, 0.5f);
		break;
	}
	case SHAPE_CROSS:
	{
		t.begin();
		t.normal(0.0f, -1.0f, 0.0f);
		tesselateCrossTexture(tile, data, Vec3(-0.5f, -0.5f, -0.5f));
		t.draw();
		break;
	}
	case SHAPE_CROPS:
	{
		t.begin();
		t.normal(0.0f, -1.0f, 0.0f);
		tesselateRowTexture(tile, data, Vec3(-0.5f, -0.5f, -0.5f));
		t.draw();
		break;
	}
	case SHAPE_STAIRS:
	{
		tile->updateDefaultShape();
		t.addOffset(-0.5f, -0.5f, -0.5f);
		for (int i = 0; i < 2; i++)
		{
			if (!i)
				tile->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f);
			else
				tile->setShape(0.0f, 0.0f, 0.5f, 1.0f, 0.5f, 1.0f);

			t.begin();

			float red, grn, blu;
			for (int dir = Facing::DOWN; dir <= Facing::EAST; dir++)
			{
				const auto normal = Facing::NORMALS[dir];
				int texture = tile->getTexture((Facing::Name)dir, data);
				computeTileColor(tile, data, (Facing::Name)dir, texture, red, grn, blu, preshade ? bright * Facing::LIGHT[dir] : bright);
				t.normal(normal[0], normal[1], normal[2]);
				renderFace(tile, Vec3::ZERO, tile->getTexture((Facing::Name)dir, data), (Facing::Name)dir, red, grn, blu);
			}

			shadeIfNeeded(t, preshade, red, grn, blu, 1.0f);
			t.draw();
		}
		t.addOffset(0.5f, 0.5f, 0.5f);
		break;
	}
	case SHAPE_FENCE:
	{
		t.addOffset(-0.5f, -0.5f, -0.5f);
		for (int i = 0; i < 4; i++)
		{
			switch (i) {
			case 1: tile->setShape(0.325F, 0.0F, 0.75F, 0.625F, 1.0F, 1.0F); break;
			case 2: tile->setShape(0.4375F, 0.8125F, -0.125F, 0.5625F, 0.9375, 1.125F); break;
			case 3: tile->setShape(0.4375F, 0.3125F, -0.125F, 0.5625F, 0.4375F, 1.125F); break;
			default: tile->setShape(0.325F, 0.0F, 0.0F, 0.625F, 1.0F, 0.25F); break;
			}

			t.begin();

			float red, grn, blu;
			for (int dir = Facing::DOWN; dir <= Facing::EAST; dir++)
			{
				const auto normal = Facing::NORMALS[dir];
				int texture = tile->getTexture((Facing::Name)dir, data);
				computeTileColor(tile, data, (Facing::Name)dir, texture, red, grn, blu, preshade ? bright * Facing::LIGHT[dir] : bright);
				t.normal(normal[0], normal[1], normal[2]);
				renderFace(tile, Vec3::ZERO, tile->getTexture((Facing::Name)dir, data), (Facing::Name)dir, red, grn, blu);
			}

			shadeIfNeeded(t, preshade, red, grn, blu, 1.0f);
			t.draw();
		}
		t.addOffset(0.5f, 0.5f, 0.5f);
		tile->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
		break;
	}
	}
}

#ifdef ENH_USE_OWN_AO
bool TileRenderer::tesselateBlockInWorldWithAmbienceOcclusionV2(Tile* tile, const TilePos& pos, float r, float g, float b)
{
	// START OF AUXILIARY DATA FOR AO

	// Well, this is a big enum.
	enum eDirection
	{
		ETILE_FACE_HERE,  // Here (no change in coordinates)
		ETILE_FACE_N,  // -Z
		ETILE_FACE_S,  // +Z
		ETILE_FACE_W,  // -X
		ETILE_FACE_E,  // +X
		ETILE_FACE_NW, // -X-Z
		ETILE_FACE_NE, // +X-Z
		ETILE_FACE_SW, // -X+Z
		ETILE_FACE_SE, // +X+Z

		ETILE_FACE_U,  // +Y
		ETILE_FACE_UN, // +Y-Z
		ETILE_FACE_US, // +Y+Z
		ETILE_FACE_UW, // +Y-X
		ETILE_FACE_UE, // +Y+X
		ETILE_FACE_UNW,// +Y-X-Z
		ETILE_FACE_UNE,// +Y+X-Z
		ETILE_FACE_USW,// +Y-X+Z
		ETILE_FACE_USE,// +Y+X+Z

		ETILE_FACE_D,  // -Y
		ETILE_FACE_DN, // -Y-Z
		ETILE_FACE_DS, // -Y+Z
		ETILE_FACE_DW, // -Y-X
		ETILE_FACE_DE, // -Y+X
		ETILE_FACE_DNW,// -Y-X-Z
		ETILE_FACE_DNE,// -Y+X-Z
		ETILE_FACE_DSW,// -Y-X+Z
		ETILE_FACE_DSE,// -Y+X+Z
		ETILE_FACE_COUNT,
	};

	// for Facing::Name directions
	static const int diffX[] = { 0,0,0,0,-1,1 };
	static const int diffZ[] = { 0,0,-1,1,0,0 };
	static const int diffY[] = { -1,1,0,0,0,0 };

	// for ETILE_FACE_? directions
	static const int diffEX[] = { 0,0,0,-1,+1,-1,+1,-1,+1,0,0,0,-1,+1,-1,+1,-1,+1,0,0,0,-1,+1,-1,+1,-1,+1 };
	static const int diffEZ[] = { 0,-1,+1,0,0,-1,-1,+1,+1,0,-1,+1,0,0,-1,-1,+1,+1,0,-1,+1,0,0,-1,-1,+1,+1 };
	static const int diffEY[] = { 0,0,0,0,0,0,0,0,0, +1,+1,+1,+1,+1,+1,+1,+1,+1, -1,-1,-1,-1,-1,-1,-1,-1,-1 };

	// Convert a Facing::Name to an ETILE_FACE_?
	static const int dirToEdir[] = { ETILE_FACE_D, ETILE_FACE_U, ETILE_FACE_N, ETILE_FACE_S, ETILE_FACE_W, ETILE_FACE_E };

	// this is a huge table. Essentially this tells us which 3 tiles we should base our lighting value on aside from the
	// one that's pointed to by the face we're rendering.
	// The array is 6 * 4 * 3. 6 for the directions, 4 for the number of vertices per face, and 3 for the number of other tiles we'll check.

	static const int massLUT[] = {
		ETILE_FACE_DS, ETILE_FACE_DW, ETILE_FACE_DSW,   ETILE_FACE_DN, ETILE_FACE_DW, ETILE_FACE_DNW,   ETILE_FACE_DN, ETILE_FACE_DE, ETILE_FACE_DNE,   ETILE_FACE_DS, ETILE_FACE_DE, ETILE_FACE_DSE,
		ETILE_FACE_US, ETILE_FACE_UE, ETILE_FACE_USE,   ETILE_FACE_UN, ETILE_FACE_UE, ETILE_FACE_UNE,   ETILE_FACE_UN, ETILE_FACE_UW, ETILE_FACE_UNW,   ETILE_FACE_US, ETILE_FACE_UW, ETILE_FACE_USW,
		ETILE_FACE_UN, ETILE_FACE_NW, ETILE_FACE_UNW,   ETILE_FACE_UN, ETILE_FACE_NE, ETILE_FACE_UNE,   ETILE_FACE_DN, ETILE_FACE_NE, ETILE_FACE_DNE,   ETILE_FACE_DN, ETILE_FACE_NW, ETILE_FACE_DNW,
		ETILE_FACE_US, ETILE_FACE_SW, ETILE_FACE_USW,   ETILE_FACE_DS, ETILE_FACE_SW, ETILE_FACE_DSW,   ETILE_FACE_DS, ETILE_FACE_SE, ETILE_FACE_DSE,   ETILE_FACE_US, ETILE_FACE_SE, ETILE_FACE_USE,
		ETILE_FACE_UW, ETILE_FACE_SW, ETILE_FACE_USW,   ETILE_FACE_UW, ETILE_FACE_NW, ETILE_FACE_UNW,   ETILE_FACE_DW, ETILE_FACE_NW, ETILE_FACE_DNW,   ETILE_FACE_DW, ETILE_FACE_SW, ETILE_FACE_DSW,
		ETILE_FACE_DE, ETILE_FACE_SE, ETILE_FACE_DSE,   ETILE_FACE_DE, ETILE_FACE_NE, ETILE_FACE_DNE,   ETILE_FACE_UE, ETILE_FACE_NE, ETILE_FACE_UNE,   ETILE_FACE_UE, ETILE_FACE_SE, ETILE_FACE_USE,
	};


	// END OF AUXILIARY DATA FOR AO

	bool bBother = false;

	// Check if we should bother at all. Most of the time, we don't. There are some calculations that we shouldn't do unless necessary
	for (int dir = Facing::DOWN; dir <= Facing::EAST; dir++)
	{
		TilePos tp(pos.x + diffX[dir],
				   pos.y + diffY[dir],
				   pos.z + diffZ[dir]);

		if (!m_bDisableCulling && !tile->shouldRenderFace(m_pLevelSource, tp, (Facing::Name)dir))
			continue;

		bBother = true;
		break;
	}

	if (!bBother)
		return false;

	//Tesselator& t = Tesselator::instance;

	//float fLightHere = tile->getBrightness(m_pLevelSource, pos);

	float lights[ETILE_FACE_COUNT];

	// Get the brightness of the tile we're processing, as well as all tiles around it
	lights[ETILE_FACE_HERE] = tile->getBrightness(m_pLevelSource, pos);

	for (int i = 1; i < ETILE_FACE_COUNT; i++)
	{
		lights[i] = tile->getBrightness(m_pLevelSource, TilePos(pos.x + diffEX[i], pos.y + diffEY[i], pos.z + diffEZ[i]));
	}

	// Render all the faces.
	for (int dir = Facing::DOWN; dir <= Facing::EAST; dir++)
	{
		TilePos tp(pos.x + diffX[dir],
			pos.y + diffY[dir],
			pos.z + diffZ[dir]);

		// check if we should bother in the first place
		if (!m_bDisableCulling && !tile->shouldRenderFace(m_pLevelSource, tp, (Facing::Name)dir))
		{
			continue;
		}

		float fR, fG, fB;
		int tex = tile->getTexture(m_pLevelSource, pos, (Facing::Name)dir);

		computeColor(tile->getColor(m_pLevelSource, pos, (Facing::Name)dir, tex), fR, fG, fB, Facing::LIGHT[dir]);

		for (int i = 0; i < 4; i++)
			m_vtxRed[i] = m_vtxGreen[i] = m_vtxBlue[i] = 1.0f;

		const int* table = &massLUT[dir * 12];

		for (int i = 0; i < 4; i++)
		{
			// average: the light at the tile the face's normal would point towards, and 3 other tiles
			// chosen based on the vertex corner number
			float br = lights[dirToEdir[dir]];
			for (int j = 0; j < 3; j++)
				br += lights[table[j + i * 3]];

			br *= 0.25f;
				
			m_vtxRed[i]   = br;
			m_vtxGreen[i] = br;
			m_vtxBlue[i]  = br;
		}

		m_bAmbientOcclusion = true;

		renderFace(tile, pos, tex, (Facing::Name)dir, fR * r, fG * g, fB * b);

		if (TileRenderer::m_bFancyGrass && tex == TEXTURE_GRASS_SIDE && Facing::isHorizontal((Facing::Name)dir))
		{
			computeColor(tile->getColor(m_pLevelSource, pos, (Facing::Name)dir, TEXTURE_GRASS_SIDE_OVERLAY), fR, fG, fB, Facing::LIGHT[dir]);
			renderFace(tile, pos, TEXTURE_GRASS_SIDE_OVERLAY, (Facing::Name)dir, fR * r, fG * g, fB * b);
		}

		m_bAmbientOcclusion = false;
	}

	return true;
}
#endif

int TileRenderer::getTileColor(Tile* tile, const TilePos& pos)
{
	if (tile == nullptr)
	{
		return 0xffffff;
	}

	return tile->getColor(m_pLevelSource, pos, Facing::UP, -1);
}

bool TileRenderer::useAmbientOcclusion() const
{
	return Minecraft::useAmbientOcclusion;
}
