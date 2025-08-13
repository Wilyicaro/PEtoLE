#include "BedItem.hpp"
#include "world/level/Level.hpp"
#include "world/entity/Player.hpp"
#include "world/tile/Tile.hpp"

BedItem::BedItem(int id) : Item(id)
{
	m_maxStackSize = 1;
}

bool BedItem::useOn(ItemInstance* inst, Player* player, Level* level, const TilePos& pos, Facing::Name face)
{
	if (face != Facing::UP)
	{
		return false;
	}
	else {
		TilePos tp = pos.above();
		Tile* var8 = Tile::bed;
		int dir = Mth::floor((player->m_rot.y * 4.0F / 360.0F) + 0.5) & 3;
		TilePos offTp(tp);
		if (dir == 0) {
			offTp.z += 1;
		}

		if (dir == 1) {
			offTp.x -= 1;
		}

		if (dir == 2) {
			offTp.z -= 1;
		}

		if (dir == 3) {
			offTp.x += 1;
		}

		if (level->isEmptyTile(tp) && level->isEmptyTile(offTp) && level->isSolidTile(tp.below()) && level->isSolidTile(offTp.below()))
		{
			level->setTileAndData(tp, var8->m_ID, dir);
			level->setTileAndData(offTp, var8->m_ID, dir + 8);
			--inst->m_count;
			return true;
		}
		else
			return false;
	}
}
