#include "BucketItem.hpp"
#include "world/entity/Player.hpp"
#include "world/level/Level.hpp"

BucketItem::BucketItem(int id, int content) : Item(id)
{
	m_maxStackSize = 1;
    m_maxDamage = 64;
    m_content = content;
}

std::shared_ptr<ItemInstance> BucketItem::use(std::shared_ptr<ItemInstance> item, Level* level, Player* player)
{
    Vec3 var13 = Vec3(player->m_pos.x, player->m_pos.y + 1.62 - player->m_heightOffset, player->m_pos.z);
    float const constexpr r = M_PI / 180;
    float var14 = Mth::cos(-player->m_rot.y * r - M_PI);
    float var15 = Mth::sin(-player->m_rot.y * r - M_PI);
    float var16 = -Mth::cos(-player->m_rot.x * r);
    float var17 = Mth::sin(-player->m_rot.x * r);
    float var18 = var15 * var16;
    float var20 = var14 * var16;
    HitResult hitResult = level->clip(var13, var13.add(var18 * 5, var17 * 5, var20 * 5), m_content == 0);
    if (!hitResult.isHit()) {
        return item;
    }
    else {
        if (hitResult.m_hitType == HitResult::TILE) {
            TilePos tp(hitResult.m_tilePos);
            if (!level->mayInteract(player, tp)) {
                return item;
            }

            if (!m_content) {
                if (level->getMaterial(tp) == Material::water && level->getData(tp) == 0) {
                    level->setTile(tp, 0);
                    return std::make_shared<ItemInstance>(Item::waterBucket);
                }

                if (level->getMaterial(tp) == Material::lava && level->getData(tp) == 0) {
                    level->setTile(tp, 0);
                    return std::make_shared<ItemInstance>(Item::lavaBucket);
                }
            }
            else {
                if (m_content < 0) {
                    return std::make_shared<ItemInstance>(Item::emptyBucket);
                }

                if (hitResult.m_hitSide == 0) {
                    --tp.y;
                }

                if (hitResult.m_hitSide == 1) {
                    ++tp.y;
                }

                if (hitResult.m_hitSide == 2) {
                    --tp.z;
                }

                if (hitResult.m_hitSide == 3) {
                    ++tp.z;
                }

                if (hitResult.m_hitSide == 4) {
                    --tp.x;
                }

                if (hitResult.m_hitSide == 5) {
                    ++tp.x;
                }

                if (level->isEmptyTile(tp) || !level->getMaterial(tp)->isSolid()) {
                    if (level->m_pDimension->m_bUltraWarm && m_content == Tile::water->m_ID) {
                        level->playSound(var13 + 0.5, "random.fizz", 0.5F, 2.6F + (level->m_random.nextFloat() - level->m_random.nextFloat()) * 0.8F);

                        for (int var28 = 0; var28 < 8; ++var28) {
                            level->addParticle("largesmoke", Vec3(tp.x + Mth::random(), tp.y + Mth::random(), tp.z + Mth::random()));
                        }
                    }
                    else {
                        level->setTileAndData(tp, m_content, 0);
                    }

                    return std::make_shared<ItemInstance>(Item::emptyBucket);
                }
            }
        }
        else if (m_content == 0 && hitResult.m_pEnt->getType() == *EntityType::cow) {
            return std::make_shared<ItemInstance>(Item::milk);
        }

        return item;
    }
}

