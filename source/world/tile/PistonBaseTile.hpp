#pragma once

#include "Tile.hpp"

class PistonBaseTile : public Tile
{
public:
	PistonBaseTile(int id, int texture, bool sticky);

	int getTexture(Facing::Name side, int meta) const override;
	int getTexture(const LevelSource*, const TilePos& pos, Facing::Name face) const override;
	int getRenderShape() const override;
	void triggerEvent(Level*, const TilePos& pos, int, int) override;
	void setPlacedBy(Level*, const TilePos& pos, Mob*, Facing::Name face) override;
	void neighborChanged(Level*, const TilePos& pos, TileID tile) override;
	void onPlace(Level*, const TilePos& pos) override;
	void updateShape(const LevelSource*, const TilePos&) override;
	void updateDefaultShape() override;
	void addAABBs(const Level* level, const TilePos& pos, const AABB* aabb, std::vector<AABB>& out) override;

	bool isSolidRender() const override;
	bool isCubeShaped() const override;

	int getFaceTexture() const;

	static int getFacing(int data)
	{
		return data & 7;
	}

	static bool isExtended(int data)
	{
		return (data & 8);
	}

private:
	void checkIfExtend(Level*, const TilePos& pos);
	bool getNeighborSignal(Level*, const TilePos& pos, Facing::Name facing);
	static int getRotationData(Level* level, const TilePos& pos, Player* player);
	static bool isPushable(TileID tile, Level* level, const TilePos& pos, bool destroy);
	static bool canMoveBlocks(Level* level, const TilePos& pos, Facing::Name facing);
	bool moveBlocks(Level* level, const TilePos& pos, Facing::Name facing);

private:
	bool m_bIsSticky, m_bUpdating;
};

