#pragma once

#include "EntityRenderer.hpp"

class ItemSpriteRenderer : public EntityRenderer
{
public:
	ItemSpriteRenderer(int sprite, float scale = 0.5f);
	void render(Entity*, const Vec3&, float a, float b) override;

public:
	int m_sprite;
	float m_scale;
};

