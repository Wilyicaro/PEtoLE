#pragma once

#include "EntityRenderer.hpp"
#include <client/renderer/TileRenderer.hpp>

class MinecartRenderer : public EntityRenderer
{
public:
	MinecartRenderer();

	void render(Entity*, const Vec3&, float, float) override;

protected:
	Model* m_pModel;
	TileRenderer m_tileRenderer;
};

