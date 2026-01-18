/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "EntityRenderer.hpp"
#include "../TileRenderer.hpp"
#include "world/item/ItemInstance.hpp"

class ItemRenderer : public EntityRenderer
{
public:
	ItemRenderer();

	void render(Entity*, const Vec3&, float, float) override;

	static void renderGuiItem(Textures*, std::shared_ptr<ItemInstance>, int, int, const Color& color = Color::WHITE);
	static void renderGuiItemDecorations(Font*, Textures*, std::shared_ptr<ItemInstance>, int, int);
	static void renderGuiItemAndDecorate(Font*, Textures*, std::shared_ptr<ItemInstance>, int, int);

public:
	Random m_random;
	static TileRenderer* tileRenderer;
};

