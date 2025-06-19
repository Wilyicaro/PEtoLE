#pragma once

#include "Tile.hpp"

class MobSpawnerTile : public Tile
{
    public:
		MobSpawnerTile(int id, int texture);
		bool hasTileEntity() const override;
		std::shared_ptr<TileEntity> newTileEntity() override;
		int getResource(int var1, Random*) const override;
		int getResourceCount(Random*) const override;
		bool isSolidRender() const override;
};
