#pragma once

#include "MobRenderer.hpp"

class GiantMobRenderer : public MobRenderer
{
public:
	GiantMobRenderer(Model*, float, float);
	~GiantMobRenderer();

	void scale(Mob*, float) override;

private:
    float m_scale;
};
