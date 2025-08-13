/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class TrapDoorTile : public Tile
{
public:
	TrapDoorTile(int ID, Material*);

	void attack(Level*, const TilePos& pos, Player*) override;
	bool use(Level*, const TilePos& pos, Player*) override;
	HitResult clip(const Level*, const TilePos& pos, Vec3, Vec3) override;
	AABB* getAABB(const Level*, const TilePos& pos) override;
	AABB getTileAABB(const Level*, const TilePos& pos) override;
	bool isCubeShaped() const override;
	bool isSolidRender() const override;
	bool mayPlace(const Level*, const TilePos& pos, Facing::Name face) const override;
	void setPlacedOnFace(Level*, const TilePos& pos, Facing::Name face) override;
	void updateShape(const LevelSource*, const TilePos& pos) override;
	void updateDefaultShape() override;
	void neighborChanged(Level*, const TilePos& pos, TileID newTile) override;

	void setOpen(Level*, const TilePos& pos, bool bOpen);
	
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
	void setShape(int dir);
#pragma GCC diagnostic pop

	// @NOTE: These are inlined.
	inline static bool isOpen(int data)
	{
		return (data & 4) != 0;
	}
	inline static bool isTop(int data)
	{
		return (data & 8) != 0;
	}
};
