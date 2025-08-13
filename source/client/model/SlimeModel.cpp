/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "SlimeModel.hpp"
#include "common/Mth.hpp"

SlimeModel::SlimeModel(int vOffs) :
	Model(64, 32),
	m_cube(0, vOffs),
    m_eye0(32, 0),
    m_eye1(32, 4),
    m_mouth(32, 8)
{
	m_cube.setModel(this);

    if (vOffs > 0)
    {
        m_eye0.setModel(this);
        m_eye1.setModel(this);
        m_mouth.setModel(this);

        m_cube.addBox(-3.0F, 17.0F, -3.0F, 6, 6, 6);
        m_eye0.addBox(-3.25F, 18.0F, -3.5F, 2, 2, 2);
        m_eye1.addBox(1.25F, 18.0F, -3.5F, 2, 2, 2);
        m_mouth.addBox(0.0F, 21.0F, -3.5F, 1, 1, 1);
    } 
    else
        m_cube.addBox(-4.0F, 16.0F, -4.0F, 8, 8, 8);
}

SlimeModel::~SlimeModel()
{
}

void SlimeModel::render(float a, float b, float c, float d, float e, float f)
{
	setupAnim(a, b, c, d, e, f);
	m_cube.render(f);
    if (m_cube.m_yTexOffs > 0)
    {
        m_eye0.render(f);
        m_eye1.render(f);
        m_mouth.render(f);
    }
}

void SlimeModel::setupAnim(float time, float r, float bob, float yRot, float xRot, float scale)
{
}