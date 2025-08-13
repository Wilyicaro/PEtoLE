#pragma once

#include "EntityRenderer.hpp"
#include "world/entity/Painting.hpp"

class PaintingRenderer : public EntityRenderer
{
public:
	PaintingRenderer();
	void render(Entity*, float x, float y, float z, float a, float b) override;

private:
	void renderPainting(Painting* painting, int w, int h, int uo, int vo);
	void setBrightness(Painting* painting, float ss, float ya);

public:
	Random m_random;
};

