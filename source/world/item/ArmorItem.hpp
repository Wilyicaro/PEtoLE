#pragma once

#include "Item.hpp"

class ArmorItem : public Item
{
public:
	ArmorItem(int id, ArmorTier& tier, EquipmentSlot slot);

	EquipmentSlot getEquipmentSlot() const override;
	const std::string& getArmorTexture() const override;
	int getDefense() const override;
private:
	const std::string& m_texture;
	EquipmentSlot m_slot;
	int m_defense;

	static const constexpr int defensePerSlot[4] = { 3, 8, 6, 3 };
	static const constexpr int healthPerSlot[4] = { 11, 16, 15, 13 };
};
