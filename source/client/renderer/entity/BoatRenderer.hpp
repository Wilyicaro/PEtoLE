#pragma once

#include "EntityRenderer.hpp"

class BoatRenderer : public EntityRenderer
{
public:
	BoatRenderer();

	void render(Entity*, const Vec3&, float, float) override;

protected:
	Model* m_pModel;
};

