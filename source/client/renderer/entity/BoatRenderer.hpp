#pragma once

#include "EntityRenderer.hpp"

class BoatRenderer : public EntityRenderer
{
public:
	BoatRenderer();

	void render(Entity*, float, float, float, float, float) override;

protected:
	Model* m_pModel;
};

