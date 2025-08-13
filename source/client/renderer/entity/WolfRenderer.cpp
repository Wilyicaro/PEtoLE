/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "WolfRenderer.hpp"
#include "world/entity/Wolf.hpp"

WolfRenderer::WolfRenderer(Model* pModel, float f) : MobRenderer(pModel, f)
{
}

WolfRenderer::~WolfRenderer()
{
}

float WolfRenderer::getBob(Mob* mob, float time)
{
	return ((Wolf*)mob)->getTailAngle();
}
