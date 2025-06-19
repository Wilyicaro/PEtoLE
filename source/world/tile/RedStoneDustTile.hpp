/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"
#include <set>

class RedStoneDustTile : public Tile
{
public:
	RedStoneDustTile(int ID, int texture);

	AABB* getAABB(const Level*, const TilePos& pos) override;
	bool isSolidRender() const override;
	bool isCubeShaped() const override;
	int getRenderShape() const override;
	int getColor(const LevelSource*, const TilePos& pos, Facing::Name facing = Facing::UP, int texture = -1) const override;
	bool mayPlace(const Level*, const TilePos& pos) const override;
	int getResource(int, Random*) const override;
	int getDirectSignal(const Level*, const TilePos& pos, Facing::Name face) const override;
	int getSignal(const LevelSource*, const TilePos& pos, Facing::Name face) const override;
	void neighborChanged(Level* level, const TilePos& pos, TileID tile) override;
	bool isSignalSource() const override;
	void animateTick(Level*, const TilePos& pos, Random*) override;
	static bool shouldConnectTo(const LevelSource* level, const TilePos& pos, int side);
	void onPlace(Level* level, const TilePos& pos) override;
	void onRemove(Level* level, const TilePos& pos) override;

	static const constexpr int repeaterConnect[4] = {2, 3, 0, 1};

private:
	std::set<TilePos> m_toUpdate;
	void updatePowerStrength(Level* level, const TilePos& pos);
	void updatePowerStrength(Level* level, const TilePos& from, const TilePos& to);
	int checkTarget(Level* level, const TilePos& pos, int data);
	void checkCornerChangeAt(Level* level, const TilePos& pos);
	void checkSignalUpdates(Level* level, const TilePos& pos);

	bool m_shouldSignal = true;

};
