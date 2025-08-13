#pragma once

#include "Tile.hpp"

class RepeaterTile : public Tile
{
public:
	static const constexpr real repeaterFacing[4] = { -0.0625, 1.0 / 16.0, 0.1875, 0.3125 };

	RepeaterTile(int id, bool powered);
	bool mayPlace(const Level*, const TilePos& pos) const override;
	bool canSurvive(const Level*, const TilePos& pos) const override;
	void tick(Level*, const TilePos& pos, Random*) override;
	void neighborChanged(Level* level, const TilePos& pos, TileID tile) override;
	bool isSolidRender() const override;
	bool isCubeShaped() const override;
	int getTexture(Facing::Name face, int) const override;
	int getTexture(Facing::Name face) const override;
	bool shouldRenderFace(const LevelSource*, const TilePos& pos, Facing::Name face) const override;
	int getRenderShape() const override;
	bool use(Level* level, const TilePos& pos, Player* player) override;
	void setPlacedBy(Level*, const TilePos& pos, Mob*, Facing::Name face) override;
	void onPlace(Level* pLevel, const TilePos& pos) override;
	int getSignal(const LevelSource*, const TilePos& pos, Facing::Name face) const override;
	int getDirectSignal(const Level*, const TilePos& pos, Facing::Name face) const override;
	bool isSignalSource() const override;
	int getResource(int, Random*) const override;
	void animateTick(Level* level, const TilePos& pos, Random* random) override;

private:
	bool m_bIsPowered;

	static const constexpr int repeaterFaceTicks[4] = { 1, 2, 3, 4 };

	bool canPower(Level* level, const TilePos& pos, int data);
};

