/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ItemEntity.hpp"
#include "world/level/Level.hpp"

ItemEntity::ItemEntity(Level* level) : Entity(level)
{
	m_pType = EntityType::item;
	m_age = 0;
	m_throwTime = 0;
	m_tickCount = 0;
	m_health = 5;
	m_bMakeStepSound = false;

	m_bobOffs = 2 * float(M_PI) * Mth::random();
	setSize(0.25f, 0.25f);
	m_heightOffset = m_bbHeight * 0.5f;
}

ItemEntity::ItemEntity(Level* level, const Vec3& pos, std::shared_ptr<ItemInstance> itemInstance) : ItemEntity(level)
{
	m_itemInstance = itemInstance;
	setPos(pos);

	m_rot.y = 360.0f * Mth::random();

	m_vel.y = 0.2f;
	m_vel.x = Mth::random() * 0.2f - 0.1f;
	m_vel.z = Mth::random() * 0.2f - 0.1f;
}

void ItemEntity::burn(int damage)
{
	hurt(nullptr, damage);
}

bool ItemEntity::hurt(Entity* pCulprit, int damage)
{
	markHurt();
	m_health -= damage;
	if (m_health < 0)
		remove();

	return false;
}

bool ItemEntity::wasInWater()
{
	return m_pLevel->checkAndHandleWater(m_hitbox, Material::water, this);
}

void ItemEntity::playerTouch(Player* player)
{
	Inventory* pInventory = player->m_pInventory;

	int oldCount = m_itemInstance->m_count;
	if (m_pLevel->m_bIsOnline || m_throwTime || !pInventory->add(m_itemInstance))
		return;

	m_pLevel->playSound(this, "random.pop", 0.3f, (((m_random.nextFloat() - m_random.nextFloat()) * 0.7f) + 1.0f) + (((m_random.nextFloat() - m_random.nextFloat()) * 0.7f) + 1.0f));
	player->take(this, oldCount);
	remove();
}

void ItemEntity::tick()
{
	Entity::tick();

	if (m_throwTime > 0)
		m_throwTime--;

	m_oPos = m_pos;
	m_vel.y -= 0.04;

	if (m_pLevel->getMaterial(m_pos) == Material::lava)
	{
		// give it a small bounce upwards
		m_vel.y = 0.2;
		m_vel.x = 0.2 * (m_random.nextFloat() - m_random.nextFloat());
		m_vel.z = 0.2 * (m_random.nextFloat() - m_random.nextFloat());
		m_pLevel->playSound(this, "random.fizz", 0.4, 2.0 + m_random.nextFloat() * 0.4);
	}

	checkInTile(Vec3(m_pos.x, (m_hitbox.min.y + m_hitbox.max.y) / 2, m_pos.z));

	move(m_vel);

	float dragFactor = 0.98;

	if (m_bOnGround)
	{
		dragFactor = 0.588;
		TileID tile = m_pLevel->getTile(TilePos(m_pos.x, m_hitbox.min.y - 1, m_pos.z));
		if (tile > 0)
			dragFactor = Tile::tiles[tile]->m_friction * 0.98;
	}

	m_vel.x *= dragFactor;
	m_vel.z *= dragFactor;
	m_vel.y *= 0.98;

	if (m_bOnGround)
		m_vel.y *= -0.5;

	m_tickCount++;
	m_age++;

	// despawn after 5 minutes
	if (m_age >= 6000)
		remove();
}

void ItemEntity::addAdditionalSaveData(std::shared_ptr<CompoundTag> var1) {
	var1->putShort("Health", m_health);
	var1->putShort("Age", m_age);
	var1->put("Item", m_itemInstance->save(std::make_shared<CompoundTag>()));
}

void ItemEntity::readAdditionalSaveData(std::shared_ptr<CompoundTag> var1) {
	m_health = var1->getShort("Health") & 255;
	m_age = var1->getShort("Age");
	m_itemInstance = std::make_shared<ItemInstance>(var1->getOrMakeCompound("Item"));
}
