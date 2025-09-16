#pragma once

#include "EntityRenderer.hpp"
#include "../TileRenderer.hpp"

class ArrowRenderer : public EntityRenderer
{
public:
	ArrowRenderer();

	void render(Entity*, const Vec3&, float, float) override;

public:
	TileRenderer m_tileRenderer;
};

