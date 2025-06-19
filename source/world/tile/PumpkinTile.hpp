#pragma once

#include "Tile.hpp"

class PumpkinTile : public Tile
{
    public:
		PumpkinTile(int id, bool lantern);
		int getTexture(const LevelSource*, const TilePos& pos, Facing::Name face) const override;
		int getTexture(Facing::Name face) const override;
		virtual void setPlacedBy(Level*, const TilePos& pos, Mob*, Facing::Name face) override;
		bool lantern;
};

