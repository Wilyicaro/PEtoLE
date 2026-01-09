#include "Achievement.hpp"

#include "client/locale/Language.hpp"

Achievement::Achievement(int id, const std::string name, int x, int y, Item* item, Achievement* parent) : Achievement(id, name, x, y, std::make_shared<ItemInstance>(item), parent)
{
}

Achievement::Achievement(int id, const std::string name, int x, int y, Tile* tile, Achievement* parent) : Achievement(id, name, x, y, std::make_shared<ItemInstance>(tile), parent)
{
}

Achievement::Achievement(int id, const std::string name, int x, int y, std::shared_ptr<ItemInstance> icon, Achievement* parent) : Stat(5242880 + id, name),
m_x(x),
m_y(y),
m_icon(icon),
m_pParent(parent)
{
	if (x < Achievements::minX)
		Achievements::minX = x;

	if (y < Achievements::minY)
		Achievements::minY = y;

	if (x > Achievements::maxX)
		Achievements::maxX = x;

	if (y > Achievements::maxY)
		Achievements::maxY = y;
}
