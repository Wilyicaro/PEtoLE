#include "BoatItem.hpp"
#include "world/level/Level.hpp"
#include <world/entity/Boat.hpp>

BoatItem::BoatItem(int id) : Item(id)
{
	m_maxStackSize = 1;
}


std::shared_ptr<ItemInstance> BoatItem::use(std::shared_ptr<ItemInstance> item, Level* level, Player* player)
{
    Vec3 var13 = Vec3(player->m_pos.x, player->m_pos.y + 1.62, player->m_pos.z);
    float const constexpr r = M_PI / 180;
    float var14 = Mth::cos(-player->m_rot.y * r - M_PI);
    float var15 = Mth::sin(-player->m_rot.y * r - M_PI);
    float var16 = -Mth::cos(-player->m_rot.x * r);
    float var17 = Mth::sin(-player->m_rot.x * r);
    float var18 = var15 * var16;
    float var20 = var14 * var16;
    HitResult hitResult = level->clip(var13, var13.add(var18 * 5, var17 * 5, var20 * 5), true);
    if (!hitResult.isHit()) {
        return item;
    }
    else {
        if (hitResult.m_hitType == HitResult::TILE) {
            if (!level->m_bIsOnline) {
                level->addEntity(std::make_shared<Boat>(level, TilePos(hitResult.m_tilePos.x + 0.5, hitResult.m_tilePos.y + 1.5, hitResult.m_tilePos.z + 0.5)));
            }

            --item->m_count;
        }

        return item;
    }
}
