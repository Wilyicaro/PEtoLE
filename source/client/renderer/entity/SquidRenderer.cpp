/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "SquidRenderer.hpp"
#include <world/entity/Squid.hpp>

SquidRenderer::SquidRenderer(Model* pModel, float f) : MobRenderer(pModel, f)
{
}

SquidRenderer::~SquidRenderer()
{
}

void SquidRenderer::setupRotations(Entity* ent, float, float bodyRot, float a)
{
    Squid* squid = (Squid*)ent;
    float bodyXRot = squid->m_xBodyRotO + (squid->m_xBodyRot - squid->m_xBodyRotO) * a;
    float bodyZRot = squid->m_zBodyRotO + (squid->m_zBodyRot - squid->m_zBodyRotO) * a;
    glTranslatef(0.0F, 0.5F, 0.0F);
    glRotatef(180.0F - bodyRot, 0.0F, 1.0F, 0.0F);
    glRotatef(bodyXRot, 1.0F, 0.0F, 0.0F);
    glRotatef(bodyZRot, 0.0F, 1.0F, 0.0F);
    glTranslatef(0.0F, -1.2F, 0.0F);
}

float SquidRenderer::getBob(Mob* ent, float a)
{
    Squid* squid = (Squid*)ent;
    return squid->m_oldTentacleAngle + (squid->m_tentacleAngle - squid->m_oldTentacleAngle) * a;
}
