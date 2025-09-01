#include "FlintAndSteelItem.hpp"
#include "world/entity/Player.hpp"
#include "world/level/Level.hpp"

FlintAndSteelItem::FlintAndSteelItem(int id) : Item(id)
{
    m_maxDamage = 64;
	m_maxStackSize = 1;
}

bool FlintAndSteelItem::useOn(ItemInstance* item, Player* player, Level* level, const TilePos& pos, Facing::Name face)
{
    TilePos tp = pos.relative(face);
    int var8 = level->getTile(tp);
    if (var8 == TILE_AIR) {
        level->playSound(tp.center(), "fire.ignite", 1.0F, itemRand.nextFloat() * 0.4F + 0.8F);
        level->setTile(tp, Tile::fire->m_ID);
    }

    item->hurtAndBreak(1, player);
    return true;
}
