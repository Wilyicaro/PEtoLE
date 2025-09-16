#pragma once

#include "EntityRenderer.hpp"

class FishingHookRenderer : public EntityRenderer
{
public:
	FishingHookRenderer();
	void render(Entity*, const Vec3&, float a, float b) override;
};
