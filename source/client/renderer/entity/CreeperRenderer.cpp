/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "CreeperRenderer.hpp"
#include <world/entity/Creeper.hpp>

CreeperRenderer::CreeperRenderer(Model* pModel, Model* pArmorModel, float f) : MobRenderer(pModel, f)
{
    setArmor(pArmorModel);
}

CreeperRenderer::~CreeperRenderer()
{
}

bool CreeperRenderer::prepareArmor(Mob* mob, int a, float b)
{
    if (((Creeper*)mob)->isPowered())
    {
        if (a == 1)
        {
            float var4 = mob->m_tickCount + b;
            bindTexture("armor/power.png");
            glMatrixMode(GL_TEXTURE);
            glLoadIdentity();
            float var5 = var4 * 0.01F;
            float var6 = var4 * 0.01F;
            glTranslatef(var5, var6, 0.0F);
            glMatrixMode(GL_MODELVIEW);
            glEnable(GL_BLEND);
            float var7 = 0.5F;
            glColor4f(var7, var7, var7, 1.0F);
            glDisable(GL_LIGHTING);
            glBlendFunc(GL_ONE, GL_ONE);
            return true;
        }

        if (a == 2)
        {
            glMatrixMode(GL_TEXTURE);
            glLoadIdentity();
            glMatrixMode(GL_MODELVIEW);
            glEnable(GL_LIGHTING);
            glDisable(GL_BLEND);
        }
    }
    return false;
}

int CreeperRenderer::getOverlayColor(Mob* pMob, float a, float b)
{
    float step = ((Creeper*)pMob)->getSwelling(b);
    if ((int)(step * 10.0F) % 2 == 0) {
        return 0;
    }
    else {
        int _a = (int)(step * 0.2F * 255.0F);
        if (_a < 0) {
            _a = 0;
        }

        if (_a > 255) {
            _a = 255;
        }

        int r = 255;
        int g = 255;
        int b = 255;
        return _a << 24 | r << 16 | g << 8 | b;
    }
}

void CreeperRenderer::scale(Mob* pMob, float f)
{
    float g = ((Creeper*)pMob)->getSwelling(f);
    float wobble = 1.0F + Mth::sin(g * 100.0F) * g * 0.01F;
    if (g < 0.0F) {
        g = 0.0F;
    }

    if (g > 1.0F) {
        g = 1.0F;
    }

    g *= g;
    g *= g;
    float s = (1.0F + g * 0.4F) * wobble;
    float hs = (1.0F + g * 0.1F) / wobble;
    glScalef(s, hs, s);
}
