/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "thirdparty/GL/GL.hpp"
#include "GameRenderer.hpp"
#include "client/app/Minecraft.hpp"
#include "client/player/input/Multitouch.hpp"
#include "client/player/input/Controller.hpp"
#include "Frustum.hpp"
#include "Lighting.hpp"
#include "renderer/GL/GL.hpp"

// #define SHOW_VERTEX_COUNTER_GRAPHIC

#if defined SHOW_VERTEX_COUNTER_GRAPHIC && !defined _DEBUG
#undef  SHOW_VERTEX_COUNTER_GRAPHIC
#endif

static int t_keepHitResult; // that is its address in v0.1.1j
int t_keepPic;

void GameRenderer::_init()
{
	//ItemInHandRenderer* m_pItemInHandRenderer = nullptr;

	m_renderDistance = 0.0f;
	m_ticks = 0;
	m_rainSoundTime = 0;
	m_hovered = nullptr;
	field_14 = 0.0f;
	field_18 = 0.0f;
	field_1C = 0.0f;
	field_20 = 0.0f;
	field_24 = 0.0f;
	field_28 = 0.0f;
	field_2C = 4.0f;
	m_noNeighborUpdate = 4.0f;
	field_34 = 0.0f;
	field_38 = 0.0f;
	field_3C = 0.0f;
	field_40 = 0.0f;
	m_zoom = 1.0f;
	field_48 = 0.0f;
	field_4C = 0.0f;
	field_50 = 0.0f;
	field_54 = 0.0f;
	field_58 = 0.0f;
	field_5C = 0.0f;
	m_fr = 0.0f;
	m_fg = 0.0f;
	m_fb = 0.0f;
	m_fogBrO = 0.0f;
	m_fogBr = 0.0f;
	m_oTex = 0.0f;
	field_78 = 0.0f;
	field_7C = 0.0f;
	field_80 = 0.0f;
	field_84 = 0.0f;
	m_lastUpdatedMS = 0;
	m_shownFPS = 0;
	m_shownChunkUpdates = 0;

	m_envTexturePresence = 0;
}

GameRenderer::GameRenderer(Minecraft* pMinecraft) :
	m_pMinecraft(pMinecraft)
{
	_init();

	saveMatrices();

	m_pItemInHandRenderer = new ItemInHandRenderer(pMinecraft);
	EntityRenderDispatcher::getInstance()->m_pItemInHandRenderer = m_pItemInHandRenderer;
}

GameRenderer::~GameRenderer()
{
	delete m_pItemInHandRenderer;
}

void GameRenderer::zoomRegion(float a, float b, float c)
{
	m_zoom = a;
	field_48 = b;
	field_4C = c;
}

void GameRenderer::unZoomRegion()
{
	m_zoom = 1.0f;
}

void GameRenderer::setupCamera(float f, int i)
{
	m_renderDistance = float(256 >> m_pMinecraft->getOptions()->m_iViewDistance);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (m_pMinecraft->getOptions()->m_bAnaglyphs)
	{
		glTranslatef(float(1 - 2 * i) * 0.07f, 0.0f, 0.0f);
	}

	if (m_zoom != 1.0)
	{
		glTranslatef(field_48, -field_4C, 0.0);
		glScalef(m_zoom, m_zoom, 1.0);
	}

	float fov = getFov(f);
	gluPerspective(fov, float(Minecraft::width) / float(Minecraft::height), 0.05f, m_renderDistance * 2);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (m_pMinecraft->getOptions()->m_bAnaglyphs)
	{
		glTranslatef(float(2 * i - 1) * 0.1f, 0.0f, 0.0f);
	}

	bobHurt(f);
	if (m_pMinecraft->getOptions()->m_bViewBobbing)
		bobView(f);

	moveCameraToPlayer(f);
}

void GameRenderer::moveCameraToPlayer(float f)
{
	auto& pMob = m_pMinecraft->m_pMobPersp;

	float headHeightDiff = pMob->m_heightOffset - 1.62f;

	float posX = Mth::Lerp(pMob->m_oPos.x, pMob->m_pos.x, f);
	float posY = Mth::Lerp(pMob->m_oPos.y, pMob->m_pos.y, f);
	float posZ = Mth::Lerp(pMob->m_oPos.z, pMob->m_pos.z, f);

	glRotatef(field_5C + f * (field_58 - field_5C), 0.0f, 0.0f, 1.0f);
	
	if (pMob->isSleeping())
	{
		headHeightDiff += 1;
		glTranslatef(0.0F, 0.3F, 0.0F);
		if (!m_pMinecraft->getOptions()->field_241) {
			int d3 = m_pMinecraft->m_pLevel->getTile(pMob->m_pos);
			if (d3 == Tile::bed->m_ID) {
				int j = m_pMinecraft->m_pLevel->getData(pMob->m_pos);
				int f5 = j & 3;
				glRotatef(f5 * 90.0f, 0.0F, 1.0F, 0.0F);
			}

			glRotatef(pMob->m_rotPrev.y + (pMob->m_rot.y - pMob->m_rotPrev.y) * f + 180.0F, 0.0F, -1.0F, 0.0F);
			glRotatef(pMob->m_rotPrev.x + (pMob->m_rot.x - pMob->m_rotPrev.x) * f, -1.0F, 0.0F, 0.0F);
		}
	}
	else if (m_pMinecraft->getOptions()->m_bThirdPerson)
	{
		float v11 = m_noNeighborUpdate + (field_2C - m_noNeighborUpdate) * f;
		if (m_pMinecraft->getOptions()->field_241)
		{
			glTranslatef(0.0f, 0.0f, -v11);
			glRotatef(field_38 + (field_34 - field_38) * f, 1.0f, 0.0f, 0.0f);
			glRotatef(field_40 + (field_3C - field_40) * f, 0.0f, 1.0f, 0.0f);
		}
		else
		{
			float mob_yaw = pMob->m_rot.y;
			float mob_pitch = pMob->m_rot.x;

			float pitchRad = mob_pitch / 180.0f * float(M_PI);

			float aX = posX - (-(Mth::sin(mob_yaw / 180.0f * float(M_PI)) * Mth::cos(pitchRad)) * v11);
			float aY = posY + (Mth::sin(pitchRad) * v11);
			float aZ = posZ - ((Mth::cos(mob_yaw / 180.0f * float(M_PI)) * Mth::cos(pitchRad)) * v11);

			for (int i = 0; i < 8; i++)
			{
				float offsX = (2 * (i & 1) - 1) * 0.1f;
				float offsY = ((i & 2) - 1) * 0.1f;
				float offsZ = (2 * ((i >> 2) & 1) - 1) * 0.1f;

				HitResult hr = m_pMinecraft->m_pLevel->clip(
					Vec3(posX + offsX, posY + offsY, posZ + offsZ),
					Vec3(aX + offsX + offsZ, aY + offsY, aZ + offsZ) // @NOTE: Not sure why it adds offsZ to offsX.
				);

				if (hr.m_hitType != HitResult::NONE)
				{
					float dX = posX - hr.m_hitPos.x;
					float dY = posY - hr.m_hitPos.y;
					float dZ = posZ - hr.m_hitPos.z;
					float dist = Mth::sqrt(dX * dX + dY * dY + dZ * dZ);
					if (v11 > dist)
						v11 = dist;
				}
			}

			// @HUH: Why the hell is it rotating by 0
			glRotatef(pMob->m_rot.x - mob_pitch, 1.0f, 0.0f, 0.0f);
			glRotatef(pMob->m_rot.y - mob_yaw, 0.0f, 1.0f, 0.0f);
			glTranslatef(0.0, 0.0, -v11);
			glRotatef(mob_yaw - pMob->m_rot.y, 0.0f, 1.0f, 0.0f);
			glRotatef(mob_pitch - pMob->m_rot.x, 1.0f, 0.0f, 0.0f);
		}
	}
	else
	{
		glTranslatef(0.0f, 0.0f, -0.1f);
	}

	if (!m_pMinecraft->getOptions()->field_241)
	{
		glRotatef(pMob->m_rotPrev.x + f * (pMob->m_rot.x - pMob->m_rotPrev.x), 1.0f, 0.0f, 0.0f);
		glRotatef(pMob->m_rotPrev.y + f * (pMob->m_rot.y - pMob->m_rotPrev.y) + 180.0f, 0.0f, 1.0f, 0.0f);
	}

	glTranslatef(0.0f, headHeightDiff, 0.0f);
}

void GameRenderer::saveMatrices()
{
	glGetFloatv(GL_PROJECTION_MATRIX, m_matrix_projection);
	glGetFloatv(GL_MODELVIEW_MATRIX,  m_matrix_model_view);
}

void GameRenderer::setupGuiScreen()
{
	float x = Gui::InvGuiScale * Minecraft::width;
	float y = Gui::InvGuiScale * Minecraft::height;
	glClear(GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	xglOrthof(0, x, y, 0, 1000.0f, 3000.0f); // @NOTE: for whatever reason, nearpl is 1000.0f on LCE
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -2000.0f);
}

void GameRenderer::bobHurt(float f)
{
	auto& pMob = m_pMinecraft->m_pMobPersp;

	if (pMob->m_health <= 0)
		glRotatef(-8000.0f / (float(pMob->m_deathTime) + f + 200.0f) + 40.0f, 0.0f, 0.0f, 1.0f);

	if (pMob->m_hurtTime > 0)
	{
		float val = (pMob->m_hurtTime - f) / pMob->m_hurtDuration;

		glRotatef(-pMob->m_hurtDir, 0.0f, 1.0f, 0.0f);
		glRotatef(Mth::sin(val * val * val * val * 3.1416f) * -14.0f, 0.0f, 0.0f, 1.0f);
		glRotatef(pMob->m_hurtDir, 0.0f, 1.0f, 0.0f);
	}
}

void GameRenderer::bobView(float f)
{
	if (!m_pMinecraft->m_pMobPersp->isPlayer())
		return;

	auto player = std::dynamic_pointer_cast<Player>(m_pMinecraft->m_pMobPersp);
	float f1 = Mth::Lerp(player->m_oBob, player->m_bob, f);
	float f2 = Mth::Lerp(player->m_oTilt, player->m_tilt, f);
	// @NOTE: Multiplying by M_PI inside of the paren makes it stuttery for some reason? Anyways it works now :)
	float f3 = -(player->m_walkDist + (player->m_walkDist - player->m_walkDistO) * f) * float(M_PI);
	float f4 = Mth::sin(f3);
	float f5 = Mth::cos(f3);
	glTranslatef((f4 * f1) * 0.5f, -fabsf(f5 * f1), 0.0f);
	float f6 = Mth::sin(f3);
	glRotatef((f6 * f1) * 3.0f, 0.0f, 0.0f, 1.0f);
	float f7 = Mth::cos(f3 - 0.2f);
	glRotatef(fabsf(f7 * f1) * 5.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(f2, 1.0f, 0.0f, 0.0f);
}

void GameRenderer::setupClearColor(float f)
{
	Minecraft* pMC = m_pMinecraft;
	Level* pLevel = pMC->m_pLevel;
	auto& pMob = pMC->m_pMobPersp;

	float x1 = 1.0f - powf(1.0f / float(4 - pMC->getOptions()->m_iViewDistance), 0.25f);

	Vec3f skyColor = pLevel->getSkyColor(pMob.get(), f), fogColor = pLevel->getFogColor(f);

	m_fr = fogColor.x;
	m_fg = fogColor.y;
	m_fb = fogColor.z;

	m_fr += (skyColor.x - m_fr) * x1;
	m_fg += (skyColor.y - m_fg) * x1;
	m_fb += (skyColor.z - m_fb) * x1;

	if (pMob->isUnderLiquid(Material::water))
	{
		m_fr = 0.02f;
		m_fg = 0.02f;
		m_fb = 0.2f;
	}
	else if (pMob->isUnderLiquid(Material::lava))
	{
		m_fr = 0.6f;
		m_fg = 0.1f;
		m_fb = 0.0f;
	}

	float x2 = m_fogBrO + (m_fogBr - m_fogBrO) * f;

	m_fr *= x2;
	m_fg *= x2;
	m_fb *= x2;

	if (pMC->getOptions()->m_bAnaglyphs)
	{
		float r = (m_fr * 30.0f + m_fg * 59.0f + m_fb * 11.0f) / 100.0f;
		float g = (m_fr * 30.0f + m_fg * 70.0f) / 100.0f;
		float b = (m_fr * 30.0f + m_fb * 70.0f) / 100.0f;

		m_fr = r;
		m_fg = g;
		m_fb = b;
	}

	glClearColor(m_fr, m_fg, m_fb, 1.0f);
}

#ifndef ORIGINAL_CODE
void GameRenderer::renderNoCamera()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
#endif

void GameRenderer::setupFog(int i)
{
	float fog_color[4];
	fog_color[0] = m_fr;
	fog_color[1] = m_fg;
	fog_color[2] = m_fb;
	fog_color[3] = 1.0f;

	glFogfv(GL_FOG_COLOR, fog_color);
	glNormal3f(0.0f, -1.0f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	if (m_pMinecraft->m_pMobPersp->isUnderLiquid(Material::water))
	{
	#if defined(ORIGINAL_CODE) || defined(ANDROID)
		glFogx(GL_FOG_MODE, GL_EXP);
	#else
		glFogi(GL_FOG_MODE, GL_EXP);
	#endif

		glFogf(GL_FOG_DENSITY, 0.1f);
	}
	else if (m_pMinecraft->m_pMobPersp->isUnderLiquid(Material::lava))
	{
	#if defined(ORIGINAL_CODE) || defined(ANDROID)
		glFogx(GL_FOG_MODE, GL_EXP);
	#else
		glFogi(GL_FOG_MODE, GL_EXP);
	#endif

		glFogf(GL_FOG_DENSITY, 2.0f);
	}
	else
	{
	#if defined(ORIGINAL_CODE) || defined(ANDROID)
		glFogx(GL_FOG_MODE, GL_LINEAR);
	#else
		glFogi(GL_FOG_MODE, GL_LINEAR);
	#endif

		glFogf(GL_FOG_START, m_renderDistance * 0.25f);
		glFogf(GL_FOG_END, m_renderDistance);
		if (i < 0)
		{
			glFogf(GL_FOG_START, 0.0f);
			glFogf(GL_FOG_END, m_renderDistance * 0.8f);
		}

		if (m_pMinecraft->m_pLevel->m_pDimension->m_bFoggy)
		{
			glFogf(GL_FOG_START, 0.0f);
		}

	}

	glEnable(GL_COLOR_MATERIAL);
}

float GameRenderer::getFov(float f)
{
	auto& pMob = m_pMinecraft->m_pMobPersp;

	float x1 = 70.0f;

	if (pMob->isUnderLiquid(Material::water))
		x1 = 60.0f;

	if (pMob->m_health <= 0)
	{
		float x2 = 1.0f + (-500.0f / ((pMob->m_deathTime + f) + 500.0f));
		x1 /= (1.0f + 2.0f * x2);
	}

	return field_54 + x1 + f * (field_50 - field_54);
}

void GameRenderer::renderLevel(float f)
{
	if (!m_pMinecraft->m_pMobPersp)
	{
		m_pMinecraft->m_pMobPersp = m_pMinecraft->m_pPlayer;

		if (!m_pMinecraft->m_pMobPersp)
		{
		#ifndef ORIGINAL_CODE
			renderNoCamera();
		#endif
			return;
		}
	}

	pick(f);

	auto& pMob = m_pMinecraft->m_pMobPersp;
	Vec3 fCamPos;

	fCamPos.x = pMob->m_posPrev.x + (pMob->m_pos.x - pMob->m_posPrev.x) * f;
	fCamPos.y = pMob->m_posPrev.y + (pMob->m_pos.y - pMob->m_posPrev.y) * f;
	fCamPos.z = pMob->m_posPrev.z + (pMob->m_pos.z - pMob->m_posPrev.z) * f;

	bool bAnaglyph = m_pMinecraft->getOptions()->m_bAnaglyphs;
	bool bFancy = m_pMinecraft->getOptions()->m_bFancyGraphics;

	LevelRenderer* pLR = m_pMinecraft->m_pLevelRenderer;
	ParticleEngine* pPE = m_pMinecraft->m_pParticleEngine;

	for (int i = 0; i < 2; i++)
	{
		if (bAnaglyph)
		{
			if (i)
				glColorMask(true, false, false, false);
			else
				glColorMask(false, true, true, false);
		}

		glViewport(0, 0, Minecraft::width, Minecraft::height);
		setupClearColor(f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_CULL_FACE);
		setupCamera(f, i);
		saveMatrices();

		Frustum::prepare();
		if (m_pMinecraft->getOptions()->m_iViewDistance <= 2)
		{
			setupFog(-1);
			pLR->renderSky(f);
		}

		glEnable(GL_FOG);
		setupFog(1);

		if (m_pMinecraft->getOptions()->m_bAmbientOcclusion)
			glShadeModel(GL_SMOOTH);

		Frustum& frust = Frustum::frustum;
		FrustumCuller frustumCuller;
		frustumCuller.m_frustumData.x = frust;
		frustumCuller.prepare(fCamPos.x, fCamPos.y, fCamPos.z);

		pLR->cull(&frustumCuller, f);
		if (!i) {
		  pLR->updateDirtyChunks(pMob.get(), false);
		}

		setupFog(0);
		glEnable(GL_FOG);

		m_pMinecraft->m_pTextures->loadAndBindTerrain();

		Lighting::turnOff();
		// render the opaque layer
		pLR->render(pMob.get(), 0, f);

		Lighting::turnOn();
		pLR->renderEntities(pMob->getPos(f), &frustumCuller, f);
		pPE->renderLit(pMob.get(), f);
		Lighting::turnOff();
		setupFog(0);
		pPE->render(pMob.get(), f);

		if (pMob->isPlayer() && m_pMinecraft->m_hitResult.m_hitType != HitResult::NONE && pMob->isUnderLiquid(Material::water))
		{
			glDisable(GL_ALPHA_TEST);

			pLR->renderHit((Player*)pMob.get(), m_pMinecraft->m_hitResult, 0, nullptr, f);

			if (m_pMinecraft->getOptions()->m_bBlockOutlines)
				pLR->renderHitOutline((Player*)pMob.get(), m_pMinecraft->m_hitResult, 0, nullptr, f);
			else
				pLR->renderHitSelect((Player*)pMob.get(), m_pMinecraft->m_hitResult, 0, nullptr, f);

			glEnable(GL_ALPHA_TEST);
		}

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		setupFog(0);

		glShadeModel(GL_SMOOTH);

		glEnable(GL_BLEND);
		glDisable(GL_CULL_FACE);

		// render the alpha layer
		m_pMinecraft->m_pTextures->loadAndBindTerrain();
		if (bFancy) {
			//@TODO: fix alpha layer dissapearing for thousandths when entering a world or moving the camera fast
			glColorMask(false, false, false, false);
			int c = pLR->render(pMob.get(), 1, f);
			if (bAnaglyph) {
				if (!i) 
					glColorMask(false, true, true, true);
				else 
					glColorMask(true, false, false, true);
			} else glColorMask(true, true, true, true);
			
			if (c > 0) pLR->renderSameAsLast(1, f);

		} else pLR->render(pMob.get(), 1, f);

		glDepthMask(true);

		glShadeModel(GL_FLAT);
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);

		//renderNameTags(f);
		if (m_zoom == 1.0f && pMob->isPlayer() && m_pMinecraft->m_hitResult.m_hitType != HitResult::NONE && !pMob->isUnderLiquid(Material::water))
		{
			glDisable(GL_ALPHA_TEST);

			// added by iProgramInCpp - renders the cracks
			pLR->renderHit((Player*)pMob.get(), m_pMinecraft->m_hitResult, 0, nullptr, f);

			if (m_pMinecraft->getOptions()->m_bBlockOutlines)
				pLR->renderHitOutline((Player*)pMob.get(), m_pMinecraft->m_hitResult, 0, nullptr, f);
			else
				pLR->renderHitSelect((Player*)pMob.get(), m_pMinecraft->m_hitResult, 0, nullptr, f);

			glEnable(GL_ALPHA_TEST);
		}

		renderSnowAndRain(f);

		setupFog(0);
		glEnable(GL_FOG);
		pLR->renderClouds(f);
		glDisable(GL_FOG);
		setupFog(1);

		if (m_zoom == 1.0f)
		{
			glClear(GL_DEPTH_BUFFER_BIT);
			renderItemInHand(f, i);
		}

		if (!bAnaglyph)
			return;
	}


	glColorMask(true, true, true, false);
}

void GameRenderer::render(float f)
{
	if (m_pMinecraft->m_pPlayer && m_pMinecraft->m_bGrabbedMouse)
	{
		Minecraft *pMC = m_pMinecraft;
		pMC->m_mouseHandler.poll();

		float multPitch = -1.0f;
		float diff_field_84;

		if (pMC->getOptions()->m_bInvertMouse)
			multPitch = 1.0f;

		if (pMC->m_mouseHandler.smoothTurning())
		{
			float mult1 = 2.0f * (0.2f + pMC->getOptions()->m_fSensitivity * 0.6f);
			mult1 = pow(mult1, 3);

			float xd = 4.0f * mult1 * pMC->m_mouseHandler.m_delta.x;
			float yd = 4.0f * mult1 * pMC->m_mouseHandler.m_delta.y;

			float old_field_84 = field_84;
			field_84 = float(m_ticks) + f;
			diff_field_84 = field_84 - old_field_84;
			m_oTex += xd;
			field_78 += yd;

			if (diff_field_84 > 3.0f)
				diff_field_84 = 3.0f;

			if (!pMC->getOptions()->field_240)
			{
				// @TODO: untangle this code
				float v17 = xd + field_14;
				float v18 = field_18;
				float v19 = field_1C;
				field_14 = v17;
				float v20 = mult1 * 0.25f * (v17 - v18);
				float v21 = v19 + (v20 - v19) * 0.5f;
				field_1C = v21;
				if ((v20 <= 0.0 || v20 <= v21) && (v20 >= 0.0 || v20 >= v21))
					v21 = mult1 * 0.25f * (v17 - v18);
				float v22 = yd + field_20;
				field_18 = v18 + v21;
				float v23 = field_24;
				field_20 = v22;
				float v24 = mult1 * 0.15f * (v22 - v23);
				float v25 = field_28 + (v24 - field_28) * 0.5f;
				field_28 = v25;
				if ((v24 <= 0.0 || v24 <= v25) && (v24 >= 0.0 || v24 >= v25))
					v25 = v24;
				field_24 = v23 + v25;
			}
		}
		else
		{
			diff_field_84 = 1.0f;
			field_7C = pMC->m_mouseHandler.m_delta.x;
			field_80 = pMC->m_mouseHandler.m_delta.y;
		}

		Vec2 rot(field_7C * diff_field_84,
			     field_80 * diff_field_84 * multPitch);
		m_pItemInHandRenderer->turn(rot);
		pMC->m_pPlayer->turn(rot);
	}

	int mouseX = int(Mouse::getX() * Gui::InvGuiScale);
	int mouseY = int(Mouse::getY() * Gui::InvGuiScale);

	if (m_pMinecraft->isTouchscreen())
	{
		int pointerId = Multitouch::getFirstActivePointerIdExThisUpdate();
		if (pointerId < 0)
		{
			mouseX = -9999;
			mouseY = -9999;
		}
		else
		{
			mouseX = int(float(Multitouch::getX(pointerId)) * Gui::InvGuiScale);
			mouseY = int(float(Multitouch::getY(pointerId)) * Gui::InvGuiScale);
		}
	}

	if (m_pMinecraft->isLevelReady())
	{
		if (t_keepPic < 0)
		{
			renderLevel(f);

			m_pMinecraft->m_gui.render(f, m_pMinecraft->m_pScreen != nullptr, mouseX, mouseY);
		}
	}
	else
	{
		glViewport(0, 0, Minecraft::width, Minecraft::height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		setupGuiScreen();
	}

	if (m_pMinecraft->m_pPlayer &&
		m_pMinecraft->m_pPlayer->m_pMoveInput)
		m_pMinecraft->m_pPlayer->m_pMoveInput->render(f);

	if (m_pMinecraft->m_pScreen)
	{
		glClear(GL_DEPTH_BUFFER_BIT);
		m_pMinecraft->m_pScreen->onRender(mouseX, mouseY, f);

	}

	// @TODO: Move to its own function
	std::stringstream debugText;
	debugText << "ReMinecraftPE " << m_pMinecraft->getVersionString();
	debugText << " (" << m_shownFPS << " fps, " << m_shownChunkUpdates << " chunk updates)" << "\n";

	if (m_pMinecraft->getOptions()->m_bDebugText)
	{
		if (m_pMinecraft->m_pPlayer && m_pMinecraft->isLevelReady())
		{
			char posStr[96];
			Vec3 pos = m_pMinecraft->m_pPlayer->getPos(f);
			sprintf(posStr, "%.2f / %.2f / %.2f", pos.x, pos.y, pos.z);

			debugText << m_pMinecraft->m_pLevelRenderer->gatherStats1();
			debugText << m_pMinecraft->m_pLevelRenderer->gatherStats2() << "\n";
			debugText << "XYZ: " << posStr << "\n";
			debugText << "Biome: " << m_pMinecraft->m_pLevel->getBiomeSource()->getBiome(TilePos(pos))->m_name << "\n";
		}
#ifdef SHOW_VERTEX_COUNTER_GRAPHIC
		extern int g_nVertices; // Tesselator.cpp
		debugText << "\nverts: " << g_nVertices;

		static int vertGraph[200];
		memcpy(vertGraph, vertGraph + 1, sizeof(vertGraph) - sizeof(int));
		vertGraph [ (sizeof(vertGraph) / sizeof(vertGraph[0])) - 1 ] = g_nVertices;

		g_nVertices = 0;

		Tesselator& t = Tesselator::instance;

		int max = 0;
		for (int i = 0; i < 200; i++)
			max = std::max(max, vertGraph[i]);

		int maxht = 100;
		int h = int(Minecraft::height * Gui::InvGuiScale);

		glDisable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		t.begin();
		t.color(1.0f, 1.0f, 1.0f, 0.15f);
		t.vertex(000, h-maxht, 0);
		t.vertex(000, h,       0);
		t.vertex(200, h,       0);
		t.vertex(200, h-maxht, 0);
		t.draw();

		t.begin();
		t.color(0.0f, 1.0f, 0.0f, 1.0f);

		for (int i = 0; i < 200 && max != 0; i++)
		{
			t.vertex(i + 0, h - (vertGraph[i] * maxht / max), 0);
			t.vertex(i + 0, h - 0, 0);
			t.vertex(i + 1, h - 0, 0);
			t.vertex(i + 1, h - (vertGraph[i] * maxht / max), 0);
		}

		t.draw();
		glEnable(GL_DEPTH_TEST);


		m_pMinecraft->m_pFont->drawShadow(std::to_string(max), 200, h - maxht, 0xFFFFFF);
#endif

		/*debugText << "\nController::stickValuesX[1]: " << Controller::stickValuesX[1];
		debugText << "\nController::stickValuesY[1]: " << Controller::stickValuesY[1];
		debugText << "\nGameRenderer::field_7C: "      << field_7C;
		debugText << "\nGameRenderer::field_80: "      << field_80;*/

		m_pMinecraft->m_pFont->drawShadow(debugText.str(), 2, 2, 0xFFFFFF);

#ifdef SHOW_VERTEX_COUNTER_GRAPHIC
		g_nVertices = 0;
#endif
	}

	int timeMs = getTimeMs();
	if (timeMs - m_lastUpdatedMS >= 1000)
	{
		m_lastUpdatedMS = timeMs;
		m_shownFPS = m_pMinecraft->getFpsIntlCounter();
		m_shownChunkUpdates = Chunk::updates;
		Chunk::updates = 0;
	}
}

void GameRenderer::tick()
{
	--t_keepPic;
#ifndef ORIGINAL_CODE
	// @BUG: If the game is left on for approximately 1,242 days, the counter will underflow,
	// causing the screen to appear frozen, and the level to not render.
	if (t_keepPic < -100)
		t_keepPic = -100;
#endif

	if (!m_pMinecraft->m_pPlayer)
		return;
	
	if (--t_keepHitResult == 0)
		m_pMinecraft->m_hitResult.m_hitType = HitResult::NONE;

#ifndef ORIGINAL_CODE
	// Not harmless to let it underflow, but we won't anyway
	if (t_keepHitResult < -100)
		t_keepHitResult = -100;
#endif

	if (m_pMinecraft->m_mouseHandler.smoothTurning())
	{
		float x1 = powf(fabsf(m_oTex), 1.2f);
		field_7C = x1 * 0.4f;
		if (m_oTex < 0.0f)
			field_7C = -field_7C;

		float x2 = powf(fabsf(field_78), 1.2f);
		field_80 = x2 * 0.4f;
		if (field_78 < 0.0f)
			field_80 = -field_80;
	}

	m_oTex = 0.0f;
	field_78 = 0.0f;
	m_fogBrO = m_fogBr;
	m_noNeighborUpdate = field_2C;
	field_38 = field_34;
	field_40 = field_3C;
	field_54 = field_50;
	field_5C = field_58;

	auto& pMob = m_pMinecraft->m_pMobPersp;
	if (!pMob)
	{
		pMob = m_pMinecraft->m_pMobPersp = m_pMinecraft->m_pPlayer;
	}

	float bright = m_pMinecraft->m_pLevel->getBrightness(pMob->m_pos);
	float x3 = float(3 - m_pMinecraft->getOptions()->m_iViewDistance);

	m_ticks++;

	float x4 = x3 / 3.0f;
	float x5 = (x4 + bright * (1.0f - x4) - m_fogBr) * 0.1f;

	m_fogBr += x5;

	m_pItemInHandRenderer->tick();
	tickRain();
}

void GameRenderer::renderItemInHand(float f, int i)
{
	glLoadIdentity();

	if (m_pMinecraft->getOptions()->m_bAnaglyphs)
		glTranslatef(float(2 * i - 1) * 0.1f, 0.0f, 0.0f);

	glPushMatrix();
	bobHurt(f);

	if (m_pMinecraft->getOptions()->m_bViewBobbing)
		bobView(f);

	if (!m_pMinecraft->getOptions()->m_bThirdPerson && !m_pMinecraft->m_pMobPersp->isSleeping() && !m_pMinecraft->getOptions()->m_bDontRenderGui)
		m_pItemInHandRenderer->render(f);

	glPopMatrix();

	if (!m_pMinecraft->getOptions()->m_bThirdPerson && !m_pMinecraft->m_pMobPersp->isSleeping())
	{
		m_pItemInHandRenderer->renderScreenEffect(f);
		bobHurt(f);
	}

	if (m_pMinecraft->getOptions()->m_bViewBobbing)
		bobView(f);
}

void GameRenderer::prepareAndRenderClouds(LevelRenderer* pLR, float f)
{

}

void GameRenderer::renderSnowAndRain(float f)
{
	float rainLevel = m_pMinecraft->m_pLevel->getRainLevel(f);

	if (rainLevel <= 0) return;

	auto& pLP = m_pMinecraft->m_pPlayer;
	int bPosX = Mth::floor(pLP->m_pos.x);
	int bPosY = Mth::floor(pLP->m_pos.y);
	int bPosZ = Mth::floor(pLP->m_pos.z);
	Vec3 pos = pLP->getPos(f);
	Tesselator& t = Tesselator::instance;
	Level* pLevel = m_pMinecraft->m_pLevel;

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int range = m_pMinecraft->getOptions()->m_bFancyGraphics ? 10 : 5;

	auto& biomes = pLevel->getBiomeSource()->getBiomeBlock(TilePos(bPosX - range, bPosY, bPosZ - range), range * 2 + 1, range * 2 + 1);
	int i = 0;

	TilePos tp(bPosX - range, 128, bPosZ - range);
	for (tp.x = bPosX - range; tp.x <= bPosX + range; tp.x++)
	{
		for (tp.z = bPosZ - range; tp.z <= bPosZ + range; tp.z++)
		{
			Biome* biome = biomes[i++];

			if (!biome->hasSnow && !biome->hasRain) continue;
			int tsb = pLevel->getTopSolidBlock(tp);
			if (tsb < 0)
				tsb = 0;

			int minY = bPosY - range;
			int maxY = bPosY + range;

			if (minY < tsb)
				minY = tsb;
			if (maxY < tsb)
				maxY = tsb;

			float offs = 1.0f;
			if (minY == maxY)
				continue;

			m_random.setSeed(static_cast<int64_t>(tp.x * tp.x * 3121 + tp.x * 45238971 + tp.z * tp.z * 418711 + tp.z * 13761));


			float f1 = float(tp.x + 0.5f) - pLP->m_pos.x;
			float f2 = float(tp.z + 0.5f) - pLP->m_pos.z;
			float f3 = Mth::sqrt(f1 * f1 + f2 * f2) / float(range);
			float f4 = pLevel->getBrightness(tp);

			if (biome->hasSnow)
			{
				float x1 = float(m_ticks) + f;
				float x2 = (float(m_ticks & 511) + f) / 512.0f;
				float x3 = m_random.nextFloat() + x1 * 0.01f * m_random.nextGaussian();
				float x4 = m_random.nextFloat() + x1 * 0.001f * m_random.nextGaussian();
				m_pMinecraft->m_pTextures->loadAndBindTexture("environment/snow.png");
				t.begin();
				glColor4f(f4, f4, f4, ((1.0f - f3 * f3) * 0.3f + 0.5f) * rainLevel);
				t.offset(-pos.x, -pos.y, -pos.z);
				t.vertexUV(float(tp.x + 0), float(minY), float(tp.z + 0.5f), 0.0f * offs + x3, float(minY) * offs / 4.0f + x2 * offs + x4);
				t.vertexUV(float(tp.x + 1), float(minY), float(tp.z + 0.5f), 1.0f * offs + x3, float(minY) * offs / 4.0f + x2 * offs + x4);
				t.vertexUV(float(tp.x + 1), float(maxY), float(tp.z + 0.5f), 1.0f * offs + x3, float(maxY) * offs / 4.0f + x2 * offs + x4);
				t.vertexUV(float(tp.x + 0), float(maxY), float(tp.z + 0.5f), 0.0f * offs + x3, float(maxY) * offs / 4.0f + x2 * offs + x4);
				t.vertexUV(float(tp.x + 0.5f), float(minY), float(tp.z + 0), 0.0f * offs + x3, float(minY) * offs / 4.0f + x2 * offs + x4);
				t.vertexUV(float(tp.x + 0.5f), float(minY), float(tp.z + 1), 1.0f * offs + x3, float(minY) * offs / 4.0f + x2 * offs + x4);
				t.vertexUV(float(tp.x + 0.5f), float(maxY), float(tp.z + 1), 1.0f * offs + x3, float(maxY) * offs / 4.0f + x2 * offs + x4);
				t.vertexUV(float(tp.x + 0.5f), float(maxY), float(tp.z + 0), 0.0f * offs + x3, float(maxY) * offs / 4.0f + x2 * offs + x4);
				t.offset(0.0f, 0.0f, 0.0f);
				t.draw();
			}
			if (biome->canOnlyRain())
			{
				float x2 = ((float)(m_ticks + tp.x * tp.x * 3121 + tp.x * 45238971 + tp.z * tp.z * 418711 + tp.z * 13761 & 31) + f) / 32.0F * (3.0F + m_random.nextFloat());
				m_pMinecraft->m_pTextures->loadAndBindTexture("environment/rain.png");
				t.begin();
				f4 = f4 * 0.85f + 0.15f;
				glColor4f(f4, f4, f4, ((1.0f - f3 * f3) * 0.5f + 0.5f) * rainLevel);
				t.offset(-pos.x, -pos.y, -pos.z);
				t.vertexUV(float(tp.x + 0), float(minY), float(tp.z + 0.5f), 0.0f * offs, float(minY) * offs / 4.0f + x2 * offs);
				t.vertexUV(float(tp.x + 1), float(minY), float(tp.z + 0.5f), 1.0f * offs, float(minY) * offs / 4.0f + x2 * offs);
				t.vertexUV(float(tp.x + 1), float(maxY), float(tp.z + 0.5f), 1.0f * offs, float(maxY) * offs / 4.0f + x2 * offs);
				t.vertexUV(float(tp.x + 0), float(maxY), float(tp.z + 0.5f), 0.0f * offs, float(maxY) * offs / 4.0f + x2 * offs);
				t.vertexUV(float(tp.x + 0.5f), float(minY), float(tp.z + 0), 0.0f * offs, float(minY) * offs / 4.0f + x2 * offs);
				t.vertexUV(float(tp.x + 0.5f), float(minY), float(tp.z + 1), 1.0f * offs, float(minY) * offs / 4.0f + x2 * offs);
				t.vertexUV(float(tp.x + 0.5f), float(maxY), float(tp.z + 1), 1.0f * offs, float(maxY) * offs / 4.0f + x2 * offs);
				t.vertexUV(float(tp.x + 0.5f), float(maxY), float(tp.z + 0), 0.0f * offs, float(maxY) * offs / 4.0f + x2 * offs);
				t.offset(0.0f, 0.0f, 0.0f);
				t.draw();
			}
		}
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}

void GameRenderer::tickRain()
{
	float rainLevel = m_pMinecraft->m_pLevel->getRainLevel(1.0F);
	if (!m_pMinecraft->getOptions()->m_bFancyGraphics)
		rainLevel /= 2.0F;

	if (rainLevel != 0.0F) {
		m_random.setSeed(m_ticks * 312987231L);
		auto& var2 = m_pMinecraft->m_pMobPersp;
		Level* var3 = m_pMinecraft->m_pLevel;
		int var4 = Mth::floor(var2->m_pos.x);
		int var5 = Mth::floor(var2->m_pos.y);
		int var6 = Mth::floor(var2->m_pos.z);
		const uint8_t var7 = 10;
		Vec3 rainPos;
		int var14 = 0;

		TilePos tp;
		for (int var15 = 0; var15 < (int)(100.0F * rainLevel * rainLevel); ++var15) {
			tp.x = var4 + m_random.nextInt(var7) - m_random.nextInt(var7);
			tp.z = var6 + m_random.nextInt(var7) - m_random.nextInt(var7);
			tp.y = var3->getTopSolidBlock(tp);
			int var19 = var3->getTile(tp.below());
			if (tp.y <= var5 + var7 && tp.y >= var5 - var7 && var3->getBiomeSource()->getBiome(tp)->canOnlyRain()) {
				float var20 = m_random.nextFloat();
				float var21 = m_random.nextFloat();
				if (var19 > 0) {
					if (Tile::tiles[var19]->m_pMaterial == Material::lava)
						m_pMinecraft->m_pParticleEngine->add(new SmokeParticle(var3, Vec3(tp.x + var20, tp.y + 0.1F - Tile::tiles[var19]->m_aabb.min.y, tp.z + var21), Vec3::ZERO));
					else
					{
						++var14;
						if (m_random.nextInt(var14) == 0)
						{
							rainPos.x = tp.x + var20;
							rainPos.y = (tp.y + 0.1F) - Tile::tiles[var19]->m_aabb.min.y;
							rainPos.z = tp.z + var21;
						}

						m_pMinecraft->m_pParticleEngine->add(new WaterDropParticle(var3, Vec3(tp.x + var20, tp.y + 0.1F - Tile::tiles[var19]->m_aabb.min.y, tp.z + var21)));
					}
				}
			}
		}

		if (var14 > 0 && m_random.nextInt(3) < m_rainSoundTime++)
		{
			m_rainSoundTime = 0;
			if (rainPos.y > var2->m_pos.y + 1.0 && var3->getTopSolidBlock(var2->m_pos))
				var3->playSound(rainPos, "ambient.weather.rain", 0.1F, 0.5F);
			else
				var3->playSound(rainPos, "ambient.weather.rain", 0.2F, 1.0F);
		}

	}
}

void GameRenderer::onGraphicsReset()
{

}

void GameRenderer::pick(float f)
{
	if (!m_pMinecraft->m_pMobPersp || !m_pMinecraft->m_pLevel)
		return;

	auto& pMob = m_pMinecraft->m_pMobPersp;
	HitResult& mchr = m_pMinecraft->m_hitResult;
	float dist = m_pMinecraft->m_pGameMode->getPickRange();
	bool isFirstPerson = !m_pMinecraft->getOptions()->m_bThirdPerson;

	if (!m_pMinecraft->useSplitControls())
	{
		Vec3 mobPos = pMob->getPos(f);
		Vec3 foundPosNear, foundPosFar;
		bool flag = true;
		float offset = isFirstPerson ? 6.0f : 12.0f;

		if (m_pMinecraft->m_pInputHolder->allowPicking())
		{
			int viewport[4] = { 0 };
			viewport[2] = Minecraft::width;
			viewport[3] = Minecraft::height;
			float obj_coord[3] = { 0 };

			if (glhUnProjectf(m_pMinecraft->m_pInputHolder->m_feedbackX,
				              Minecraft::height - m_pMinecraft->m_pInputHolder->m_feedbackY,
				              1.0f,
				              m_matrix_model_view,
				              m_matrix_projection,
				              viewport,
				              obj_coord))
			{
				foundPosFar = mobPos + Vec3(obj_coord[0], obj_coord[1], obj_coord[2]);

				glhUnProjectf(m_pMinecraft->m_pInputHolder->m_feedbackX,
				              Minecraft::height - m_pMinecraft->m_pInputHolder->m_feedbackY,
				              0.0f,
				              m_matrix_model_view,
				              m_matrix_projection,
				              viewport,
				              obj_coord);

				foundPosNear = mobPos + Vec3(obj_coord[0], obj_coord[1], obj_coord[2]);

				Vec3 diff = foundPosFar - foundPosNear;
				Vec3 normDiff = diff.normalize();
				Vec3 normScaledDiff = normDiff.scale(offset);
				
				mobPos = foundPosNear + normScaledDiff;

				foundPosFar = mobPos;
			}
			
			// keep the hit result forever
			t_keepHitResult = -1;
		}
		else
		{
			t_keepHitResult = 1; // keep the tick result for exactly one frame
			flag = false;
		}

		if (flag)
		{
			if (isFirstPerson)
			{
				mchr = m_pMinecraft->m_pLevel->clip(foundPosNear, foundPosFar, false);
			}
			else
			{
				HitResult hr = m_pMinecraft->m_pLevel->clip(foundPosNear, foundPosFar, false);

				float diffX = float(hr.m_tilePos.x) - m_pMinecraft->m_pMobPersp->m_pos.x;
				float diffY = float(hr.m_tilePos.y) - m_pMinecraft->m_pMobPersp->m_pos.y;
				float diffZ = float(hr.m_tilePos.z) - m_pMinecraft->m_pMobPersp->m_pos.z;

				if (hr.m_hitType == HitResult::NONE || diffX * diffX + diffY * diffY + diffZ * diffZ > offset * offset)
					mchr.m_hitType = HitResult::NONE;
				else
					mchr = hr;
			}
		}
	}
	else
	{
		// easy case: pick from the middle of the screen
		HitResult hrMob = pMob->pick(dist, f);
		mchr = hrMob;
	}

	Vec3 mobPos = pMob->getPos(f);

	if (m_pMinecraft->m_hitResult.m_hitType != HitResult::NONE)
		dist = mchr.m_hitPos.distanceTo(mobPos);

	if (m_pMinecraft->m_pGameMode->isCreativeType())
		dist = 7.0f;
	else if (dist > 3.0f)
		dist = 3.0f;

	Vec3 view = pMob->getViewVector(f);
	Vec3 exp  = view * dist;
	Vec3 limit = mobPos + view * dist;

	m_hovered = nullptr;

	AABB scanAABB = pMob->m_hitbox;

	if (exp.x < 0) scanAABB.min.x += exp.x;
	if (exp.x > 0) scanAABB.max.x += exp.x;
	if (exp.y < 0) scanAABB.min.y += exp.y;
	if (exp.y > 0) scanAABB.max.y += exp.y;
	if (exp.z < 0) scanAABB.min.z += exp.z;
	if (exp.z > 0) scanAABB.max.z += exp.z;

	scanAABB.grow(1, 1, 1);

	EntityVector ents = m_pMinecraft->m_pLevel->getEntities(pMob.get(), scanAABB);

	float fDist = 0.0f;
	for (int i = 0; i < int(ents.size()); i++)
	{
		auto& pEnt = (ents)[i];
		if (!pEnt->isPickable())
			continue;

		AABB checkAABB = pEnt->m_hitbox;
		checkAABB.grow(pEnt->getPickRadius());

		HitResult hrMobChk = checkAABB.clip(mobPos, limit);

		if (checkAABB.contains(mobPos))
		{
			if (fDist >= 0.0f)
			{
				//this is it brother
				m_hovered = pEnt;
				fDist = 0.0f;
			}
			continue;
		}

		if (hrMobChk.m_hitType != HitResult::NONE)
		{
			float dX = hrMobChk.m_hitPos.x - mobPos.x;
			float dY = hrMobChk.m_hitPos.y - mobPos.y;
			float dZ = hrMobChk.m_hitPos.z - mobPos.z;
			float fNewDist = Mth::sqrt(dX * dX + dY * dY + dZ * dZ);

			if (fDist > fNewDist || fDist == 0.0f)
			{
				m_hovered = pEnt;
				fDist = fNewDist;
			}
		}
	}

	// picked entities take priority over tiles (?!)
	if (m_hovered)
	{
		m_pMinecraft->m_hitResult = HitResult(m_hovered);
		return;
	}

	if (m_pMinecraft->m_hitResult.m_hitType != HitResult::NONE || view.y >= -0.7f)
		return;

	mobPos = pMob->getPos(f);
	Vec3 checkVec = mobPos;
	checkVec.translate(0, -2, 0);

	HitResult hrLevelChk = m_pMinecraft->m_pLevel->clip(mobPos, checkVec);

	if (hrLevelChk.m_hitType == HitResult::NONE)
		return;

	mchr = hrLevelChk;
	mchr.m_bUnk24 = true;

	if (fabsf(view.x) <= fabsf(view.z))
	{
		m_pMinecraft->m_hitResult.m_hitSide = view.z >= 0.0f ? Facing::SOUTH : Facing::NORTH;
	}
	else
	{
		m_pMinecraft->m_hitResult.m_hitSide = view.x >= 0.0f ? Facing::EAST : Facing::WEST;
	}
}

