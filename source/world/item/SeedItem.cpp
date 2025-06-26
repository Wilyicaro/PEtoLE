#include "SeedItem.hpp"
#include "world/level/Level.hpp"
#include "world/tile/Tile.hpp"

SeedItem::SeedItem(int id, int place) : Item(id), m_tile(place)
{
}

bool SeedItem::useOn(ItemInstance* instance, Player* player, Level* level, const TilePos& pos, Facing::Name face)
{
    int tile = level->getTile(pos);
    if (tile == Tile::farmland->m_ID && level->isEmptyTile(pos.above())) {
        level->setTile(pos.above(), m_tile);
        --instance->m_count;
        return true;
    }
    else {
        return false;
    }
}
