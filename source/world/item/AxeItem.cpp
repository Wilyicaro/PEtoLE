#include "AxeItem.hpp"
#include "world/tile/Tile.hpp"

AxeItem::AxeItem(int id, Tier& tier) : DiggerItem(id, 1, tier, { Tile::wood, Tile::bookshelf, Tile::treeTrunk, Tile::chest })
{
}
