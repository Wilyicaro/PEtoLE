#include "PaintingItem.hpp"
#include "world/level/Level.hpp"
#include <world/entity/Painting.hpp>

PaintingItem::PaintingItem(int id) : Item(id)
{
	m_maxDamage = 64;
}

bool PaintingItem::useOn(ItemInstance* instance, Player* player, Level* level, const TilePos& pos, Facing::Name face)
{
    if (Facing::isVertical(face))
        return false;
    else {
        uint8_t var8 = 0;
        if (face == Facing::WEST)
            var8 = 1;

        if (face == Facing::SOUTH)
            var8 = 2;

        if (face == Facing::EAST)
            var8 = 3;

        auto painting = std::make_shared<Painting>(level, pos, var8);
        if (painting->survives())
        {
            if (!level->m_bIsOnline)
                level->addEntity(painting);

            --instance->m_count;
        }

        return true;
    }
}
