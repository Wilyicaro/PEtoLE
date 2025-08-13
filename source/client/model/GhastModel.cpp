/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "GhastModel.hpp"
#include "common/Mth.hpp"

GhastModel::GhastModel() :
	Model(64, 32),
	m_body(0, 0)
{
	m_body.setModel(this);

    int yoffs = -16;
    m_body.addBox(-8.0F, -8.0F, -8.0F, 16, 16, 16);
    m_body.m_pos.y += (float)(24 + yoffs);
    Random random(1660L);

    for (int i = 0; i < 9; ++i)
    {
        m_tentacles[i] = new ModelPart(0, 0);
        m_tentacles[i]->setModel(this);
        float xo = (((float)(i % 3) - (float)(i / 3 % 2) * 0.5F + 0.25F) / 2.0F * 2.0F - 1.0F) * 5.0F;
        float yo = ((float)(i / 3) / 2.0F * 2.0F - 1.0F) * 5.0F;
        int len = random.nextInt(7) + 8;
        m_tentacles[i]->addBox(-1.0F, 0.0F, -1.0F, 2, len, 2);
        m_tentacles[i]->m_pos.x = xo;
        m_tentacles[i]->m_pos.z = yo;
        m_tentacles[i]->m_pos.y = (31 + yoffs);
    }
}

GhastModel::~GhastModel()
{
    for (ModelPart* part : m_tentacles)
        delete part;
}

void GhastModel::render(float a, float b, float c, float d, float e, float f)
{
	setupAnim(a, b, c, d, e, f);

	m_body.render(f);

    for (ModelPart* part : m_tentacles)
        part->render(f);
}

void GhastModel::setupAnim(float time, float r, float bob, float yRot, float xRot, float scale)
{
    for (int i = 0; i < 9; ++i)
        m_tentacles[i]->m_rot.x = 0.2F * Mth::sin(bob * 0.3F + i) + 0.4F;
}