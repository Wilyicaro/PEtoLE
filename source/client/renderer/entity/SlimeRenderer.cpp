/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "SlimeRenderer.hpp"
#include <world/entity/Slime.hpp>

SlimeRenderer::SlimeRenderer(Model* pModel, Model* pArmor, float f) : MobRenderer(pModel, f)
{
	m_pArmorModel = pArmor;
}

SlimeRenderer::~SlimeRenderer()
{
}

void SlimeRenderer::scale(Mob* pMob, float f)
{
	Slime* slime = (Slime*) pMob;
	float ss = (slime->m_oSquish + (slime->m_squish - slime->m_oSquish) * f) / ((float)slime->m_size * 0.5F + 1.0F);
	float w = 1.0F / (ss + 1.0F);
	float s = slime->m_size;
	glScalef(w * s, 1.0F / w * s, w * s);
}

bool SlimeRenderer::prepareArmor(Mob* mob, int layer, float)
{
    if (layer == 0)
    {
        setArmor(m_pArmorModel);
        glEnable(GL_NORMALIZE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        return true;
    }
    else
    {
        if (layer == 1)
        {
            glDisable(GL_BLEND);
            glColor4f(1.0F, 1.0F, 1.0F, 1.0F);
        }

        return false;
    }
}
