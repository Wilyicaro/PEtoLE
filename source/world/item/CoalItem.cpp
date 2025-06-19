#include "CoalItem.hpp"

CoalItem::CoalItem(int id) : Item(id)
{
	m_maxDamage = 0;
	m_bStackedByData = true;
}

std::string CoalItem::getDescriptionId(ItemInstance* inst)
{
	return inst->getAuxValue() == 1 ? "item.charcoal" : "item.coal";
}