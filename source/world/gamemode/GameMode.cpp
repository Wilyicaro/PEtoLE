/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "GameMode.hpp"
#include "client/app/Minecraft.hpp"

GameMode::GameMode(Minecraft* pMinecraft, Level& level) :
	m_pMinecraft(pMinecraft),
	_level(level),
	field_8(0)
{
}

GameMode::~GameMode()
{
}

void GameMode::initLevel(Level* pLevel)
{
}

bool GameMode::startDestroyBlock(Player* player, const TilePos& pos, Facing::Name face)
{
	//if (!player->getCarriedItem()) // && not a bow
	return destroyBlock(player, pos, face);
}

bool GameMode::destroyBlock(Player* player, const TilePos& pos, Facing::Name face)
{
	Tile* pTile = Tile::tiles[_level.getTile(pos)];
	if (!pTile)
		return false;

	m_pMinecraft->m_pParticleEngine->destroyEffect(pos);

	int tileData = _level.getData(pos);
	pTile->playerWillDestroy(player, pos, face);
	bool bChanged = _level.setTile(pos, 0);
	if (!bChanged)
		return false;


	_level.playSound(pos + 0.5f, pTile->m_pSound->m_destroy,
		(pTile->m_pSound->volume * 0.5f) + 0.5f, pTile->m_pSound->pitch * 0.8f);

	pTile->destroy(&_level, pos, tileData);

	if (m_pMinecraft->isOnline())
	{
		m_pMinecraft->m_pRakNetInstance->send(new RemoveBlockPacket(player->m_EntityID, pos));
	}

	return true;
}

bool GameMode::continueDestroyBlock(Player* player, const TilePos& pos, Facing::Name face)
{
	return false;
}

void GameMode::stopDestroyBlock()
{
}

void GameMode::tick()
{
}

void GameMode::render(float f)
{
}

float GameMode::getPickRange() const
{
/*
  if ( *inputMode == 1 )
	return 5.7;
  if ( *inputMode == 3 )
	return 5.6;
  if ( !player || player->isCreative() )
	return 12.0;
  return 5.0;
*/
	return 7.5f;
}

std::shared_ptr<LocalPlayer> GameMode::createPlayer(Level* pLevel)
{
	return std::make_shared<LocalPlayer>(m_pMinecraft, pLevel, m_pMinecraft->m_pUser, pLevel->getDefaultGameType(), _level.m_pDimension->m_ID);
}

void GameMode::initPlayer(Player* pPlayer)
{
}

void GameMode::adjustPlayer(Player* pPlayer)
{
}

bool GameMode::canHurtPlayer()
{
	return false;
}

void GameMode::interact(Player* player, Entity* entity)
{
	player->interact(entity);
}

void GameMode::attack(Player* player, Entity* entity)
{
	player->attack(entity);
}

std::shared_ptr<ItemInstance> GameMode::handleInventoryMouseClick(int containerId, int slotNum, int buttonNum, Player* player)
{
	return player->m_containerMenu->clicked(slotNum, buttonNum, player);
}

void GameMode::handleCloseInventory(int a, Player* player)
{
	player->m_containerMenu->removed(player);
	player->m_containerMenu = player->m_inventoryMenu;
}

bool GameMode::useItem(Player* player, Level* level, std::shared_ptr<ItemInstance> instance)
{
	int oldCount = instance->m_count;
	std::shared_ptr<ItemInstance> itemInstance = instance->use(level, player);
	if (itemInstance != instance || itemInstance && itemInstance->m_count != oldCount) {
		player->m_pInventory->setItem(player->m_pInventory->m_selected, itemInstance);
		if (!itemInstance->m_count) {
			player->m_pInventory->setItem(player->m_pInventory->m_selected, nullptr);
		}

		return true;
	}
	else {
		return false;
	}
}

bool GameMode::useItemOn(Player* player, Level* level, std::shared_ptr<ItemInstance> instance, const TilePos& pos, Facing::Name face)
{
	TileID tile = level->getTile(pos);
	if (tile && Tile::tiles[tile]->use(level, pos, player))
		return true;

	if (instance)
		return instance->useOn(player, level, pos, face);

	return false;
}

