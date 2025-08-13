#pragma once

#include "EntityRenderer.hpp"

class FishingHookRenderer : public EntityRenderer
{
public:
	FishingHookRenderer();
	void render(Entity*, float x, float y, float z, float a, float b) override;
};
