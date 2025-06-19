#include "ShovelItem.hpp"
#include "world/tile/Tile.hpp"

ShovelItem::ShovelItem(int id, Tier& tier) : DiggerItem(id, 1, tier, { Tile::grass, Tile::dirt, Tile::sand, Tile::gravel, Tile::topSnow, Tile::snow, Tile::clay })
{
}

bool ShovelItem::canDestroySpecial(const Tile* tile)
{
    if (tile == Tile::topSnow) {
        return true;
    }
    else {
        return tile == Tile::snow;
    }
}
