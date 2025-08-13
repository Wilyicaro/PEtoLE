/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "WolfModel.hpp"
#include "common/Mth.hpp"
#include <world/entity/Wolf.hpp>

WolfModel::WolfModel() :
	Model(64, 32),
	m_head(0, 0),
	m_body(18, 14),
	m_leg1(0, 18),
	m_leg2(0, 18),
	m_leg3(0, 18),
	m_leg4(0, 18),
	m_rightEar(16, 14),
	m_leftEar(16, 14),
	m_snout(0, 10),
	m_tail(9, 18),
	m_mane(21, 0)
{
	m_yBabyHeadOffset = 8.0f;
	m_zBabyHeadOffset = 4.0f;

	m_head.setModel(this);
	m_body.setModel(this);
	m_leg1.setModel(this);
	m_leg2.setModel(this);
	m_leg3.setModel(this);
	m_leg4.setModel(this);
	m_rightEar.setModel(this);
	m_leftEar.setModel(this);
	m_snout.setModel(this);
	m_tail.setModel(this);
	m_mane.setModel(this);

	float var1 = 0.0F;
	const float var2 = 13.5F;
	m_head.addBox(-3.0F, -3.0F, -2.0F, 6, 6, 4, var1);
	m_head.setPos(-1.0F, var2, -7.0F);
	m_body.addBox(-4.0F, -2.0F, -3.0F, 6, 9, 6, var1);
	m_body.setPos(0.0F, 14.0F, 2.0F);
	m_mane.addBox(-4.0F, -3.0F, -3.0F, 8, 6, 7, var1);
	m_mane.setPos(-1.0F, 14.0F, 2.0F);
	m_leg1.addBox(-1.0F, 0.0F, -1.0F, 2, 8, 2, var1);
	m_leg1.setPos(-2.5F, 16.0F, 7.0F);
	m_leg2.addBox(-1.0F, 0.0F, -1.0F, 2, 8, 2, var1);
	m_leg2.setPos(0.5F, 16.0F, 7.0F);
	m_leg3.addBox(-1.0F, 0.0F, -1.0F, 2, 8, 2, var1);
	m_leg3.setPos(-2.5F, 16.0F, -4.0F);
	m_leg4.addBox(-1.0F, 0.0F, -1.0F, 2, 8, 2, var1);
	m_leg4.setPos(0.5F, 16.0F, -4.0F);
	m_tail.addBox(-1.0F, 0.0F, -1.0F, 2, 8, 2, var1);
	m_tail.setPos(-1.0F, 12.0F, 8.0F);
	m_rightEar.addBox(-3.0F, -5.0F, 0.0F, 2, 2, 1, var1);
	m_rightEar.setPos(-1.0F, var2, -7.0F);
	m_leftEar.addBox(1.0F, -5.0F, 0.0F, 2, 2, 1, var1);
	m_leftEar.setPos(-1.0F, var2, -7.0F);
	m_snout.addBox(-2.0F, 0.0F, -5.0F, 3, 3, 4, var1);
	m_snout.setPos(-0.5F, var2, -7.0F);
}

WolfModel::~WolfModel()
{
}

// NOTE: Model::render now takes an Entity pointer... It's unused, though...
void WolfModel::render(float a, float b, float c, float d, float e, float f)
{
	setupAnim(a, b, c, d, e, f);

	m_head.renderRollable(f);
	m_body.render(f);
	m_leg1.render(f);
	m_leg2.render(f);
	m_leg3.render(f);
	m_leg4.render(f);
	m_rightEar.renderRollable(f);
	m_leftEar.renderRollable(f);
	m_snout.renderRollable(f);
	m_tail.renderRollable(f);
	m_mane.render(f);
}

void WolfModel::setupAnim(float a2, float a3, float a4, float a5, float a6, float a7)
{
	m_head.m_rot.x = a6 / (float)(180.0f / M_PI);
	m_head.m_rot.y = a5 / (float)(180.0f / M_PI);
	m_rightEar.m_rot.y = m_head.m_rot.y;
	m_rightEar.m_rot.x = m_head.m_rot.x;
	m_leftEar.m_rot.y = m_head.m_rot.y;
	m_leftEar.m_rot.x = m_head.m_rot.x;
	m_snout.m_rot.y = m_head.m_rot.y;
	m_snout.m_rot.x = m_head.m_rot.x;
	m_tail.m_rot.x = a4;
}

void WolfModel::prepareMobModel(Mob* mob, float var2, float var3, float var4)
{
	Wolf* wolf = (Wolf*) mob;
	if (wolf->isAngry())
		m_tail.m_rot.y = 0.0F;
	else
		m_tail.m_rot.y = Mth::cos(var2 * 0.6662F) * 1.4F * var3;

	if (wolf->isOrderedToSit())
	{
		m_mane.setPos(-1.0F, 16.0F, -3.0F);
		m_mane.m_rot.x = M_PI * 0.4F;
		m_mane.m_rot.y = 0.0F;
		m_body.setPos(0.0F, 18.0F, 0.0F);
		m_body.m_rot.x = M_PI * 0.25F;
		m_tail.setPos(-1.0F, 21.0F, 6.0F);
		m_leg1.setPos(-2.5F, 22.0F, 2.0F);
		m_leg1.m_rot.x = M_PI * 3.0F / 2.0F;
		m_leg2.setPos(0.5F, 22.0F, 2.0F);
		m_leg2.m_rot.x = M_PI * 3.0F / 2.0F;
		m_leg3.m_rot.x = M_PI * 1.85F;
		m_leg3.setPos(-2.49F, 17.0F, -4.0F);
		m_leg4.m_rot.x = M_PI * 1.85F;
		m_leg4.setPos(0.51F, 17.0F, -4.0F);
	}
	else
	{
		m_body.setPos(0.0F, 14.0F, 2.0F);
		m_body.m_rot.x = M_PI * 0.5F;
		m_mane.setPos(-1.0F, 14.0F, -3.0F);
		m_mane.m_rot.x = m_body.m_rot.x;
		m_tail.setPos(-1.0F, 12.0F, 8.0F);
		m_leg1.setPos(-2.5F, 16.0F, 7.0F);
		m_leg2.setPos(0.5F, 16.0F, 7.0F);
		m_leg3.setPos(-2.5F, 16.0F, -4.0F);
		m_leg4.setPos(0.5F, 16.0F, -4.0F);
		m_leg1.m_rot.x = Mth::cos(var2 * 0.6662F) * 1.4F * var3;
		m_leg2.m_rot.x = Mth::cos(var2 * 0.6662F + M_PI) * 1.4F * var3;
		m_leg3.m_rot.x = Mth::cos(var2 * 0.6662F + M_PI) * 1.4F * var3;
		m_leg4.m_rot.x = Mth::cos(var2 * 0.6662F) * 1.4F * var3;
	}

	float var6 = wolf->getHeadRollAngle(var4) + wolf->getBodyRollAngle(var4, 0.0F);
	m_head.m_rot.z = var6;
	m_rightEar.m_rot.z = var6;
	m_leftEar.m_rot.z = var6;
	m_snout.m_rot.z = var6;
	m_mane.m_rot.z = wolf->getBodyRollAngle(var4, -0.08F);
	m_body.m_rot.z = wolf->getBodyRollAngle(var4, -0.16F);
	m_tail.m_rot.z = wolf->getBodyRollAngle(var4, -0.2F);
	if (wolf->isShaking())
	{
		float shade = wolf->getBrightness(var4) * wolf->getWetShade(var4);
		glColor4f(shade, shade, shade, 1.0f);
	}
}
