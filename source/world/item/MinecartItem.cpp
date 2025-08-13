#include "MinecartItem.hpp"
#include "world/level/Level.hpp"
#include <world/tile/RailTile.hpp>
#include <world/entity/Minecart.hpp>

MinecartItem::MinecartItem(int id, int type) : Item(id)
{
	m_maxStackSize = 1;
	m_type = type;
}

bool MinecartItem::useOn(ItemInstance* instance, Player* player, Level* level, const TilePos& pos, Facing::Name face)
{
    if (RailTile::isRail(level->getTile(pos))) {
        if (!level->m_bIsOnline) {
            level->addEntity(std::make_shared<Minecart>(level, pos.center(), m_type));
        }

        --instance->m_count;
        return true;
    }
    else {
        return false;
    }
}
