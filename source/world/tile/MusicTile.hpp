#pragma once

#include "Tile.hpp"

class MusicTile : public Tile
{
public:
	MusicTile(int id, int texture);
	void neighborChanged(Level*, const TilePos& pos, TileID tile) override;
	bool use(Level* level, const TilePos& pos, Player* player) override;
	void attack(Level* level, const TilePos& pos, Player* player) override;
	bool hasTileEntity() const override;
	std::shared_ptr<TileEntity> newTileEntity() override;
	void triggerEvent(Level*, const TilePos& pos, int, int) override;
};

