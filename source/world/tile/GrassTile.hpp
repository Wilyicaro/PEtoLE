/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class GrassTile : public Tile
{
public:
	GrassTile(int ID, Material*);

	int getResource(int, Random*) const override;
	int getColor(const LevelSource*, const TilePos& pos, Facing::Name facing, int texture = -1) const override;
	int getColor(int data, Facing::Name facing, int texture = -1) const override;
	int getTexture(Facing::Name face) const override;
	int getTexture(const LevelSource*, const TilePos& pos, Facing::Name face) const override;
	void tick(Level*, const TilePos& pos, Random*) override;
};
