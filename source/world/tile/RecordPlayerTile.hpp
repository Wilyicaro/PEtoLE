#pragma once

#include "Tile.hpp"

class RecordPlayerTile : public Tile
{
public:
	RecordPlayerTile(int id, int texture);
	int getTexture(Facing::Name face) const override;
	bool use(Level* level, const TilePos& pos, Player* player) override;
	void spawnResources(Level*, const TilePos& pos, int, float) override;
	void onRemove(Level* level, const TilePos& pos) override;
	void playRecord(Level* level, const TilePos& pos, int);
	void ejectRecord(Level* level, const TilePos& pos);
	bool hasTileEntity() const override;
	std::shared_ptr<TileEntity> newTileEntity() override;
};