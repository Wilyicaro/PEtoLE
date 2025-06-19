#include "HoeItem.hpp"
#include "world/entity/Player.hpp"
#include "world/level/Level.hpp"

HoeItem::HoeItem(int id, Tier& tier) : Item(id)
{
	m_maxStackSize = 1;
	m_maxDamage = tier.m_uses;
}

bool HoeItem::useOn(ItemInstance* item, Player*, Level* level, const TilePos& pos, Facing::Name face)
{
    int var8 = level->getTile(pos);
    Material* var9 = level->getMaterial(pos.above());
    if ((var9->isSolid() || var8 != Tile::grass->m_ID) && var8 != Tile::grass->m_ID) {
        return false;
    }
    else {
        Tile* var10 = Tile::farmland;
        level->playSound(pos.center(), var10->m_pSound->m_name, (var10->m_pSound->volume + 1.0F) / 2.0F, var10->m_pSound->pitch * 0.8F);
        if (level->m_bIsOnline) {
            return true;
        }
        else {
            level->setTile(pos, var10->m_ID);
            item->hurt(1);
            if (level->m_random.nextInt(8) == 0 && var8 == Tile::grass->m_ID) {
                uint8_t var11 = 1;

                for (int var12 = 0; var12 < var11; ++var12) {
                    float var13 = 0.7F;
                    float var14 = level->m_random.nextFloat() * var13 + (1.0F - var13) * 0.5F;
                    float var15 = 1.2F;
                    float var16 = level->m_random.nextFloat() * var13 + (1.0F - var13) * 0.5F;
                    auto var17 = std::make_shared<ItemEntity>(level, Vec3(pos.x + var14, pos.y + var15, pos.z + var16), new ItemInstance(Item::seeds));
                    var17->m_throwTime = 10;
                    level->addEntity(var17);
                }
            }

            return true;
        }
    }
}

bool HoeItem::isHandEquipped()
{
	return true;
}