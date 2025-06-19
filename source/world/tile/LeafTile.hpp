/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "TransparentTile.hpp"

class LeafTile : public TransparentTile
{
public:
	LeafTile(int id);
	~LeafTile();

	int getColor(const LevelSource*, const TilePos& pos, Facing::Name facing, int texture = -1) const override;
	int getColor(int data, Facing::Name facing, int texture = -1) const override;
	int getTexture(Facing::Name face, int data) const override;
	bool isSolidRender() const override;
	void onRemove(Level*, const TilePos& pos) override;
	void stepOn(Level*, const TilePos& pos, Entity*) override;
	void tick(Level*, const TilePos& pos, Random*) override;

	int getResource(int x, Random* random) const override;

	int getSpawnResourcesAuxValue(int x) const override;

	void die(Level*, const TilePos& pos);

	int* m_checkBuffer;
	int m_oTex;
};
