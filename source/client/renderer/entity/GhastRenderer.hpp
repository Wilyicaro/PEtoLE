/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include "MobRenderer.hpp"

class GhastRenderer : public MobRenderer
{
public:
	GhastRenderer(Model*, float);
	~GhastRenderer();

	void scale(Mob*, float) override;
};
