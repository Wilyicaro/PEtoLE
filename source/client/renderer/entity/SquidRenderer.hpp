/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include "MobRenderer.hpp"

class SquidRenderer : public MobRenderer
{
public:
	SquidRenderer(Model*, float);
	~SquidRenderer();

	void setupRotations(Entity*, float, float, float) override;
	float getBob(Mob*, float) override;
};
