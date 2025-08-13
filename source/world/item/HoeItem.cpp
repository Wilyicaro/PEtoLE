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
    int var9 = level->getTile(pos.above());
    if ((face == Facing::DOWN || var9 || var8 != Tile::grass->m_ID) && var8 != Tile::dirt->m_ID) {
        return false;
    }
    else {
        Tile* var10 = Tile::farmland;
        level->playSound(pos.center(), var10->m_pSound->m_name, (var10->m_pSound->volume + 1.0F) / 2.0F, var10->m_pSound->pitch * 0.8F);
        if (level->m_bIsOnline)
        {
            return true;
        }
        else
        {
            level->setTile(pos, var10->m_ID);
            item->hurt(1);
            return true;
        }
    }
}

bool HoeItem::isHandEquipped()
{
	return true;
}