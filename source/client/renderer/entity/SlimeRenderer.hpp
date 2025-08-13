/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include "MobRenderer.hpp"

class SlimeRenderer : public MobRenderer
{
public:
	SlimeRenderer(Model*, Model*, float);
	~SlimeRenderer();

	void scale(Mob*, float) override;

protected:
	virtual bool prepareArmor(Mob* mob, int, float) override;
};
