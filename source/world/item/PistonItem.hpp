#pragma once

#include "TileItem.hpp"

class PistonItem : public TileItem
{
public:
	PistonItem(int id);
	int getLevelDataForAuxValue(int x) override;
};
