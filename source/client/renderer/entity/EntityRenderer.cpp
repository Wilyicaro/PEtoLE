/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "EntityRenderer.hpp"
#include "EntityRenderDispatcher.hpp"
#include "world/entity/Entity.hpp"
#include "world/level/Level.hpp"

EntityRenderer::EntityRenderer() : m_model(0.0f, 0.0f)
{
	m_shadowRadius = 0.0f;
	m_shadowStrength = 1.0f;
	m_pDispatcher = nullptr;
}

void EntityRenderer::bindTexture(const std::string& file)
{
	m_pDispatcher->m_pTextures->loadAndBindTexture(file);
}

void EntityRenderer::bindHttpTexture(const std::string& url, const std::string& fallback)
{
	m_pDispatcher->m_pTextures->bind(m_pDispatcher->m_pTextures->loadHttpTexture(url, fallback));
}

Font* EntityRenderer::getFont()
{
	return m_pDispatcher->m_pFont;
}

void EntityRenderer::init(EntityRenderDispatcher* d)
{
	m_pDispatcher = d;
}

void EntityRenderer::onGraphicsReset()
{

}

float EntityRenderer::getBrightness(Entity* e, float g)
{
	return e->getBrightness(g);
}

void EntityRenderer::renderFlame(Entity* e, const Vec3& pos, float a)
{
	glDisable(GL_LIGHTING);
	int tex = Tile::fire->getTexture(Facing::NORTH);
	int xt = (tex & 15) << 4;
	int yt = tex & 240;
	float u0 = (float)xt / 256.0f;
	float u1 = ((float)xt + 15.99f) / 256.0f;
	float v0 = (float)yt / 256.0f;
	float v1 = ((float)yt + 15.99f) / 256.0f;
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);
	float s = e->m_bbWidth * 1.4f; // bbWidth instead of e->m_hitbox.max.x
	glScalef(s, s, s);
	bindTexture(C_TERRAIN_NAME);
	Tesselator& t = Tesselator::instance;
	float xo = 0.5f;
	float h = e->m_bbHeight / s;
	float yo = e->m_pos.y - e->m_hitbox.min.y;
	glRotatef(-m_pDispatcher->m_rot.y, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, -0.3f + (float)((int)h) * 0.02f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	t.begin();
	float sd = 0.0f;
	int i = 0;
	while (h > 0.0f)
	{
		if (i % 2 == 0) {
			u0 = (float)xt / 256.0F;
			u1 = ((float)xt + 15.99F) / 256.0F;
			v0 = (float)yt / 256.0F;
			v1 = ((float)yt + 15.99F) / 256.0F;
		}
		else {
			u0 = (float)xt / 256.0F;
			u1 = ((float)xt + 15.99F) / 256.0F;
			v0 = (float)(yt + 16) / 256.0F;
			v1 = ((float)(yt + 16) + 15.99F) / 256.0F;
		}

		if (i / 2 % 2 == 0) {
			float var24 = u1;
			u1 = u0;
			u0 = var24;
		}

		t.vertexUV(xo, 0.0f - yo, sd, u1, v1);
		t.vertexUV(- xo, 0.0f - yo, sd, u0, v1);
		t.vertexUV(- xo, 1.4f - yo, sd, u0, v0);
		t.vertexUV(xo, 1.4f - yo, sd, u1, v0);
		h -= 0.45f;
		yo -= 0.45f;
		xo *= 0.9f;
		sd += 0.03f;
		i++;
	}

	t.draw();
	glPopMatrix();
	glEnable(GL_LIGHTING);
}

void EntityRenderer::renderShadow(Entity* e, const Vec3& pos, float pow, float a)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_pDispatcher->m_pTextures->setClampToEdge(true);
	bindTexture("misc/shadow.png");
	m_pDispatcher->m_pTextures->setClampToEdge(false);

	Level* level = getLevel();

	glDepthMask(false);
	float r = m_shadowRadius;

	Vec3 cPos = pos.add(0, e->getShadowHeightOffs(), 0);
	Vec3 ePos(e->m_posPrev + (e->m_pos - e->m_posPrev) * a);
	ePos.y += e->getShadowHeightOffs();

	TilePos tpMin(ePos - r);
	TilePos tpMax(ePos.x + r, ePos.y, ePos.z + r);
	Vec3 ePosO(cPos - ePos);

	Tesselator& tt = Tesselator::instance;
	tt.begin();
	TilePos tp(tpMin);
	for (tp.x = tpMin.x; tp.x <= tpMax.x; tp.x++)
	{
		for (tp.y = tpMin.y; tp.y <= tpMax.y; tp.y++)
		{
			for (tp.z = tpMin.z; tp.z <= tpMax.z; tp.z++)
			{
				TileID t = level->getTile(tp.below());
				if (t > 0 && level->getRawBrightness(tp) > 3)
				{
					renderTileShadow(Tile::tiles[t],
						cPos, tp,
						pow, r,
						ePosO
					);
				}
			}
		}
	}
	tt.draw();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_BLEND);
	glDepthMask(true);
}

Level* EntityRenderer::getLevel() const
{
	return m_pDispatcher->m_pLevel;
}

void EntityRenderer::renderTileShadow(Tile* tt, const Vec3& pos, TilePos& tilePos, float pow, float r, const Vec3& oPos)
{
	Tesselator& t = Tesselator::instance;
	if (!tt->isCubeShaped()) return;

	float a = (pow - (pos.y - ((float)tilePos.y + oPos.y)) / 2.0f) * 0.5f * getLevel()->getBrightness(tilePos);
	if (a < 0.0f)
		return;
	if (a > 1.0f) a = 1.0f;

	t.color(1.0f, 1.0f, 1.0f, a);
	float x0 = (float)tilePos.x + tt->m_aabb.min.x + oPos.x;
	float x1 = (float)tilePos.x + tt->m_aabb.max.x + oPos.x;
	float y0 = (float)tilePos.y + tt->m_aabb.min.y + oPos.y + (1.0f / 64.0f);
	float z0 = (float)tilePos.z + tt->m_aabb.min.z + oPos.z;
	float z1 = (float)tilePos.z + tt->m_aabb.max.z + oPos.z;
	float u0 = ((pos.x - x0) / 2.0f / r + 0.5f);
	float u1 = ((pos.x - x1) / 2.0f / r + 0.5f);
	float v0 = ((pos.z - z0) / 2.0f / r + 0.5f);
	float v1 = ((pos.z - z1) / 2.0f / r + 0.5f);
	t.vertexUV(x0, y0, z0, u0, v0);
	t.vertexUV(x0, y0, z1, u0, v1);
	t.vertexUV(x1, y0, z1, u1, v1);
	t.vertexUV(x1, y0, z0, u1, v0);
}


void EntityRenderer::render(const AABB& aabb, float offX, float offY, float offZ)
{
	glDisable(GL_TEXTURE_2D);
	Tesselator& t = Tesselator::instance;
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	t.begin();
	t.offset(offX, offY, offZ);
	t.normal(0.0f, 0.0f, -1.0f);
	t.vertex(aabb.min.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.min.x, aabb.min.y, aabb.min.z);

	t.normal(0.0f, 0.0f, 1.0f);
	t.vertex(aabb.min.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.max.z);

	t.normal(0.0f, -1.0f, 0.0f);
	t.vertex(aabb.min.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.min.y, aabb.max.z);

	t.normal(0.0f, 1.0f, 0.0f);
	t.vertex(aabb.min.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.min.z);
	
	t.normal(-1.0f, 0.0f, 0.0f);
	t.vertex(aabb.min.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.min.x, aabb.min.y, aabb.min.z);
	
	t.normal(1.0f, 0.0f, 0.0f);
	t.vertex(aabb.max.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.max.z);
	
	t.offset(0.0f, 0.0f, 0.0f);
	t.draw(); // t.end() on Java
	glEnable(GL_TEXTURE_2D);
}

void EntityRenderer::renderFlat(const AABB& aabb)
{
	Tesselator& t = Tesselator::instance;

	t.begin();
	t.vertex(aabb.min.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.min.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.min.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.min.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.min.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.max.z);
	t.draw(); // t.end() on Java
}

void EntityRenderer::postRender(Entity* entity, const Vec3& pos, float rot, float a)
{
	//LocalPlayer's shadow not rendering was a bug in the original, caused by the heightOffset being applied to the y position, but as we want to replicate it, this extra condition was added
	if (m_pDispatcher->m_pOptions->m_bFancyGraphics.get() && m_shadowRadius > 0.0f && (!entity->isPlayer() || !((Player*)entity)->isLocalPlayer()))
	{
		float dist = m_pDispatcher->distanceToSqr(entity->m_pos);
		float pow = (1.0f - dist / 256.0f) * m_shadowStrength;
		if (pow > 0.0f)
			renderShadow(entity, pos, pow, a);
	}

	if (entity->isOnFire())
		renderFlame(entity, pos, a);
}
