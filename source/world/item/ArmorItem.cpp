#include "ArmorItem.hpp"

ArmorItem::ArmorItem(int id, ArmorTier& tier, EquipmentSlot slot) : Item(id), m_texture(tier.getTexture(slot)), m_defense(defensePerSlot[3 - slot]), m_slot(slot)
{
	m_maxDamage = healthPerSlot[3 - slot] * 3 << tier.m_level;
	m_maxStackSize = 1;
}

Item::EquipmentSlot ArmorItem::getEquipmentSlot() const
{
	return m_slot;
}

const std::string& ArmorItem::getArmorTexture() const
{
	return m_texture;
}

int ArmorItem::getDefense() const
{
	return m_defense;
}
