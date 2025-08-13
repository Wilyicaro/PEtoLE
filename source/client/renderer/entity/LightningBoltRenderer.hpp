#pragma once

#include "EntityRenderer.hpp"

class LightningBoltRenderer : public EntityRenderer
{
public:
	LightningBoltRenderer();
	void render(Entity*, float x, float y, float z, float a, float b) override;
};
