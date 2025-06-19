/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "CameraItem.hpp"
#include "world/level/Level.hpp"
#include "world/entity/TripodCamera.hpp"
#include "world/entity/Pig.hpp"
#include "world/entity/Chicken.hpp"
#include "world/entity/Cow.hpp"
#include "world/entity/Player.hpp"

CameraItem::CameraItem(int id) : Item(id)
{
}

std::shared_ptr<ItemInstance> CameraItem::use(std::shared_ptr<ItemInstance> inst, Level* level, Player* player)
{
#ifndef ORIGINAL_CODE
	// prevent players from using this in multiplayer, to prevent a desync of entity IDs
	if (level->m_bIsOnline)
		return inst;
#endif

	/*Mob* entity = new Pig(level);
	entity->setPos(player->m_pos);
	level->addEntity(entity);

 	entity = new Chicken(level);
	entity->setPos(player->m_pos);
	level->addEntity(entity);
	*/
	auto entity = std::make_shared<Chicken>(level);
	entity->setPos(player->m_pos);
	level->addEntity(entity);


	entity->m_vel.x = -(Mth::sin(player->m_rot.y / 180.0f * real(M_PI)) * Mth::cos(player->m_rot.x / 180.0f * real(M_PI))) * 0.3f;
	entity->m_vel.z = (Mth::cos(player->m_rot.y / 180.0f * real(M_PI)) * Mth::cos(player->m_rot.x / 180.0f * real(M_PI))) * 0.3f;
	entity->m_vel.y = 0.1 - Mth::sin(player->m_rot.x / 180.0f * real(M_PI)) * 0.3f;

	float f1 = level->m_random.nextFloat();
	float f2 = level->m_random.nextFloat();

	entity->m_vel.x += 0.02 * f2 * Mth::cos(2 * real(M_PI) * f1);
	entity->m_vel.y += 0.1 * (level->m_random.nextFloat() - level->m_random.nextFloat());
	entity->m_vel.z += 0.02 * f2 * Mth::sin(2 * real(M_PI) * f1);


	return inst;
}
