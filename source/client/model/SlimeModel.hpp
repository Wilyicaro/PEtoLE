/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include "Model.hpp"

class SlimeModel : public Model
{
public:
	SlimeModel(int);
	~SlimeModel();
	void render(float, float, float, float, float, float) override;
	void setupAnim(float, float, float, float, float, float) override;

private:
	ModelPart m_cube;
	ModelPart m_eye0;
	ModelPart m_eye1;
	ModelPart m_mouth;
};
