/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "GhastRenderer.hpp"
#include <world/entity/Ghast.hpp>

GhastRenderer::GhastRenderer(Model* pModel, float f) : MobRenderer(pModel, f)
{
}

GhastRenderer::~GhastRenderer()
{
}

void GhastRenderer::scale(Mob* pMob, float f)
{
    Ghast* ghast = (Ghast*)pMob;
    float ss = (ghast->m_oCharge + (ghast->m_charge - ghast->m_oCharge) * f) / 20.0F;
    if (ss < 0.0F)
        ss = 0.0F;

    ss = 1.0F / (ss * ss * ss * ss * ss * 2.0F + 1.0F);
    float s = (8.0F + ss) / 2.0F;
    float hs = (8.0F + 1.0F / ss) / 2.0F;
    glScalef(hs, s, hs);
    glColor4f(1.0F, 1.0F, 1.0F, 1.0F);
}
