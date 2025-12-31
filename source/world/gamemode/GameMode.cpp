/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "GameMode.hpp"
#include "network/ServerSideNetworkHandler.hpp"

GameMode::GameMode(Player* player) : 
	m_pPlayer(player),
	m_destroyingPos(-1, -1, -1),
	m_destroyProgress(0.0f),
	m_lastDestroyProgress(0.0f),
	m_destroyTicks(0),
	m_destroyCooldown(0),
	m_ticks(0),
	m_destroyProgressStart(0),
	m_delayedTickStart(0),
	m_bHasDelayedDestroy(false)
{
}

GameMode::~GameMode()
{
}

void GameMode::initLevel(Level* pLevel)
{
}

bool GameMode::startDestroyBlock(const TilePos& pos, Facing::Name face)
{
	if (m_pPlayer->isLocalPlayer() && m_pPlayer->m_pLevel->m_bIsOnline)
	{
		m_pPlayer->m_pLevel->m_pConnection->send(new PlayerActionPacket(0, pos, face));
	}
	m_pPlayer->m_pLevel->extinguishFire(m_pPlayer, pos, face);

	std::shared_ptr<ItemInstance> item = m_pPlayer->getSelectedItem();
	if (item && item->getItem() == Item::bow)
		return true;

	TileID tile = m_pPlayer->m_pLevel->getTile(pos);

	if (tile <= 0)
		return false;

	if (m_destroyProgress == 0.0f)
	{
		Tile::tiles[tile]->attack(m_pPlayer->m_pLevel, pos, m_pPlayer);
	}

	if (Tile::tiles[tile]->getDestroyProgress(m_pPlayer) >= 1.0f)
	{
		if (isCreativeType())
			m_destroyCooldown = 5;
		return destroyBlock(pos, face);
	}
	else if (m_pPlayer->isLocalPlayer() && m_pPlayer->m_pLevel->m_bIsOnline || m_pPlayer->isServerPlayer())
		m_destroyingPos = pos;

	return false;
}

bool GameMode::destroyBlock(const TilePos& pos, Facing::Name face)
{
	TileID tile = m_pPlayer->m_pLevel->getTile(pos);
	Tile* pTile = Tile::tiles[m_pPlayer->m_pLevel->getTile(pos)];
	if (!pTile)
		return false;

	int data = m_pPlayer->m_pLevel->getData(pos);

	m_pPlayer->m_pLevel->levelEvent(2001, pos, tile + data * 256);

	int tileData = m_pPlayer->m_pLevel->getData(pos);
	pTile->playerWillDestroy(m_pPlayer, pos, face);
	bool bChanged = m_pPlayer->m_pLevel->setTile(pos, 0);
	if (bChanged)
		pTile->destroy(m_pPlayer->m_pLevel, pos, tileData);

	if (isSurvivalType())
	{
		auto item = m_pPlayer->getSelectedItem();
		if (item)
		{
			item->mineBlock(tile, pos, face, m_pPlayer);
			if (!item->m_count)
			{
				item->snap(m_pPlayer);
				m_pPlayer->removeSelectedItem();
			}
		}
	}

	bool couldDestroy = m_pPlayer->canDestroy(Tile::tiles[tile]);

	if (bChanged && couldDestroy)
	{
		if (isSurvivalType())
			Tile::tiles[tile]->playerDestroy(m_pPlayer->m_pLevel, m_pPlayer, pos, data);
		if (m_pPlayer->isServerPlayer())
			m_pPlayer->getConnection()->send(m_pPlayer, new TileUpdatePacket(pos, m_pPlayer->m_pLevel));
	}

	return bChanged;
}

bool GameMode::continueDestroyBlock(const TilePos& pos, Facing::Name face)
{
	if (m_destroyCooldown > 0)
	{
		m_destroyCooldown--;
		return false;
	}

	if (m_destroyingPos != pos)
	{
		m_destroyProgress = 0.0f;
		m_lastDestroyProgress = 0.0f;
		m_destroyTicks = 0;
		m_destroyingPos = pos;
		if (m_pPlayer->isLocalPlayer() && m_pPlayer->m_pLevel->m_bIsOnline)
			startDestroyBlock(pos, face);
		return false;
	}

	TileID tile = m_pPlayer->m_pLevel->getTile(m_destroyingPos);
	if (!tile)
		return false;

	Tile* pTile = Tile::tiles[tile];
	float destroyProgress = pTile->getDestroyProgress(m_pPlayer);
	m_destroyProgress += getDestroyModifier() * destroyProgress;
	m_destroyTicks++;

	if (isSurvivalType() && (m_destroyTicks & 3) == 1)
	{
		m_pPlayer->m_pLevel->playSound(pos + 0.5f, pTile->m_pSound->m_name,
			0.125f * (1.0f + pTile->m_pSound->volume), 0.5f * pTile->m_pSound->pitch);
	}

	if (m_destroyProgress >= 1.0f)
	{
		if (m_pPlayer->isLocalPlayer() && m_pPlayer->m_pLevel->m_bIsOnline)
		{
			m_pPlayer->m_pLevel->m_pConnection->send(new PlayerActionPacket(2, pos, face));
		}
		m_destroyTicks = 0;
		m_destroyCooldown = 5;
		m_destroyProgress = 0.0f;
		m_lastDestroyProgress = 0.0f;
		return destroyBlock(m_destroyingPos, face);
	}

	return false;
}

void GameMode::stopDestroyBlock()
{
	m_destroyProgress = 0.0f;
	m_destroyCooldown = 0;
}

void GameMode::stopDestroyBlock(const TilePos& pos)
{
	if (pos == m_destroyingPos)
	{
		int tickDiff = m_ticks - m_destroyProgressStart;
		TileID tileID = m_pPlayer->m_pLevel->getTile(pos);
		if (tileID)
		{
			Tile* tile = Tile::tiles[tileID];
			float p = tile->getDestroyProgress(m_pPlayer) * (tickDiff + 1);
			if (p >= 0.7F)
			{
				destroyBlock(pos);
			}
			else if (!m_bHasDelayedDestroy)
			{
				m_bHasDelayedDestroy = true;
				m_delayedDestroyPos = pos;
				m_delayedTickStart = m_destroyProgressStart;
			}
		}
	}
}

void GameMode::sendCarriedItem(int selected)
{
	if (m_pPlayer->m_pLevel->m_bIsOnline)
		m_pPlayer->m_pLevel->m_pConnection->send(new SetCarriedItemPacket(selected));
}

void GameMode::tick()
{
	if (m_pPlayer->isLocalPlayer())
		m_lastDestroyProgress = m_destroyProgress;
	else
	{
		++m_ticks;
		if (m_bHasDelayedDestroy)
		{
			int tickDiff = m_ticks - m_delayedTickStart;
			TileID delayedTile = m_pPlayer->m_pLevel->getTile(m_delayedDestroyPos);
			if (delayedTile) {
				Tile* tile = Tile::tiles[delayedTile];
				float p = tile->getDestroyProgress(m_pPlayer) * (tickDiff + 1);
				if (p >= 1.0F)
				{
					m_bHasDelayedDestroy = false;
					destroyBlock(m_delayedDestroyPos);
				}
			}
			else
			{
				m_bHasDelayedDestroy = false;
			}
		}
	}
}

float GameMode::getPickRange() const
{
	return 4.0f;
}

void GameMode::initPlayer(Player* p)
{
	p->m_rot.y = -180.0f;
	if (isCreativeType())
		p->m_pInventory->prepareCreativeInventory();
}

void GameMode::adjustPlayer(Player* pPlayer)
{
}

bool GameMode::canHurtPlayer()
{
	return !m_pPlayer->getAbilities().m_bInvulnerable;
}

void GameMode::interact(Player* player, Entity* entity)
{
	if (player->isLocalPlayer() && player->m_pLevel->m_bIsOnline)
		player->m_pLevel->m_pConnection->send(new InteractPacket(player->m_EntityID, entity->m_EntityID, 0));
	player->interact(entity);
}

void GameMode::attack(Player* player, Entity* entity)
{
	if (player->isLocalPlayer() && player->m_pLevel->m_bIsOnline)
		player->m_pLevel->m_pConnection->send(new InteractPacket(player->m_EntityID, entity->m_EntityID, 1));
	player->attack(entity);
}

std::shared_ptr<ItemInstance> GameMode::handleInventoryMouseClick(int containerId, int slotNum, int buttonNum, bool quickMove, Player* player)
{
	std::shared_ptr<ItemInstance> result = player->m_containerMenu->clicked(slotNum, buttonNum, quickMove, player);
	if (player->isLocalPlayer() && player->m_pLevel->m_bIsOnline)
		player->m_pLevel->m_pConnection->send(new ContainerClickPacket(containerId, slotNum, buttonNum, quickMove, result, player->m_containerMenu->backup(player->m_pInventory)));
	return result;
}

void GameMode::handleCloseInventory(int a, Player* player)
{
	player->m_containerMenu->removed(player);
	if (player->m_containerMenu != player->m_inventoryMenu)
	{
		delete player->m_containerMenu;
		player->m_containerMenu = player->m_inventoryMenu;
	}
}

void GameMode::handleInventoryTick(const std::shared_ptr<ItemInstance>& item, Level* level, Player* player, int, bool)
{
	if (player->isServerPlayer() && item->getItem()->isComplex())
	{
		Packet* packet = item->getItem()->getUpdatePacket(item, level, std::dynamic_pointer_cast<Player>(player->shared_from_this()));
		if (packet)
			player->getConnection()->send(player, packet);
	}
}

bool GameMode::useItem(Player* player, Level* level, std::shared_ptr<ItemInstance> instance)
{
	if (player->isLocalPlayer() && player->m_pLevel->m_bIsOnline)
		player->m_pLevel->m_pConnection->send(new UseItemPacket(TilePos(-1, -1, -1), 255, instance));
	int oldCount = instance->m_count;
	std::shared_ptr<ItemInstance> itemInstance = instance->use(level, player);
	if (itemInstance != instance || itemInstance && itemInstance->m_count != oldCount)
	{
		player->m_pInventory->setItem(player->m_pInventory->m_selected, itemInstance);
		if (!itemInstance->m_count)
			player->m_pInventory->setItem(player->m_pInventory->m_selected, nullptr);

		return true;
	}
	else
		return false;
}

bool GameMode::useItemOn(Player* player, Level* level, std::shared_ptr<ItemInstance> instance, const TilePos& pos, Facing::Name face)
{
	if (player->isLocalPlayer() && player->m_pLevel->m_bIsOnline)
		player->m_pLevel->m_pConnection->send(new UseItemPacket(pos, face, instance));
	TileID tile = level->getTile(pos);
	if (tile && Tile::tiles[tile]->use(level, pos, player))
		return true;

	if (instance)
	{
		int oldCount = instance->m_count;
		bool used = instance->useOn(player, level, pos, face);
		if (player->isCreative() && instance->m_count != oldCount)
		{
			instance->m_count = oldCount;
		}
		return used;
	}

	return false;
}

