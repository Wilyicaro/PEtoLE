#pragma once

#include "Tile.hpp"

class FurnaceTile : public Tile
{
public:
	bool m_active;

	FurnaceTile(int id, bool active);
	int getTexture(const LevelSource*, const TilePos& pos, Facing::Name face) const override;
	void animateTick(Level* level, const TilePos& pos, Random* random) override;
	int getTexture(Facing::Name face) const override;
	void onPlace(Level* level, const TilePos& pos) override;
	bool use(Level* level, const TilePos& pos, Player* player) override;
	virtual void setPlacedBy(Level*, const TilePos& pos, Mob*, Facing::Name face) override;
	void onRemove(Level* level, const TilePos& pos) override;
	static void setLit(bool var0, Level* level, const TilePos& pos);
	bool hasTileEntity() const override;
	std::shared_ptr<TileEntity> newTileEntity() override;
	int getResource(int, Random*) const override;

	static void recalcLockDir(Level* level, const TilePos& pos);

private:
	Random m_furnaceRandom;

	static bool keepFurnaceInventory;
};

