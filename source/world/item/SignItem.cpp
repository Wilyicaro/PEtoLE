#include "SignItem.hpp"
#include "world/level/Level.hpp"
#include "world/entity/Player.hpp"
#include "world/tile/Tile.hpp"
#include <world/tile/entity/SignTileEntity.hpp>

SignItem::SignItem(int id) : Item(id)
{
	//I don't know why this max damage...
	m_maxDamage = 64;
	m_maxStackSize = 1;
}

bool SignItem::useOn(ItemInstance* inst, Player* player, Level* level, const TilePos& pos, Facing::Name face)
{
    if (face == Facing::DOWN)
        return false;
    else if (!level->getMaterial(pos)->isSolid())
        return false;
    else {
        TilePos tp = pos.relative(face);

        if (!Tile::sign->mayPlace(level, tp))
            return false;
        else
        {
            if (face == Facing::UP)
                level->setTileAndData(tp, Tile::sign->m_ID, Mth::floor(((player->m_rot.y + 180.0F) * 16.0F / 360.0F) + 0.5) & 15);
            else
                level->setTileAndData(tp, Tile::wallSign->m_ID, face);

            --inst->m_count;
            auto sign = std::dynamic_pointer_cast<SignTileEntity>(level->getTileEntity(tp));
            if (sign)
                player->openTextEdit(sign);

            return true;
        }
    }
}
