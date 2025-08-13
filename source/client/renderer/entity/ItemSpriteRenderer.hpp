#pragma once

#include "EntityRenderer.hpp"

class ItemSpriteRenderer : public EntityRenderer
{
public:
	ItemSpriteRenderer(int sprite, float scale = 0.5f);
	void render(Entity*, float x, float y, float z, float a, float b) override;

public:
	int m_sprite;
	float m_scale;
};

