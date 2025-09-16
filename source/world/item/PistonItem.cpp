#include "PistonItem.hpp"
#include "world/tile/Tile.hpp"

PistonItem::PistonItem(int id) : TileItem(id)
{
}

int PistonItem::getLevelDataForAuxValue(int x)
{
	return 7;
}
