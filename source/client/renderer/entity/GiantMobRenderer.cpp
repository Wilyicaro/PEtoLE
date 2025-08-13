#include "GiantMobRenderer.hpp"

GiantMobRenderer::GiantMobRenderer(Model* pModel, float f, float scale) : MobRenderer(pModel, f), m_scale(scale)
{
}

GiantMobRenderer::~GiantMobRenderer()
{
}

void GiantMobRenderer::scale(Mob* pMob, float f)
{
	glScalef(m_scale, m_scale, m_scale);
}

