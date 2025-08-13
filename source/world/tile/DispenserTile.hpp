#pragma once

#include "Tile.hpp"

class DispenserTile : public Tile
{
public:
	DispenserTile(int id, int texture);
	int getTickDelay() const;
	int getTexture(const LevelSource*, const TilePos& pos, Facing::Name face) const override;
	int getTexture(Facing::Name face) const override;
	void onPlace(Level* level, const TilePos& pos) override;
	void onRemove(Level* level, const TilePos& pos) override;
	bool use(Level* level, const TilePos& pos, Player* var5) override;
	void fireArrow(Level* level, const TilePos& pos, Random* random);
	void neighborChanged(Level* level, const TilePos& pos, TileID tile) override;
	void tick(Level*, const TilePos& pos, Random*) override;
	void setPlacedBy(Level* level, const TilePos& pos, Mob* mob, Facing::Name face) override;
	bool hasTileEntity() const override;
	std::shared_ptr<TileEntity> newTileEntity() override;

	Random m_dispenserRandom;
};

