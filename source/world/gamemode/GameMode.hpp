/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "world/level/Level.hpp"
#include "world/item/ItemInstance.hpp"
#include "world/entity/Player.hpp"

class GameMode
{
protected:
	Player* m_pPlayer;

public:
	GameMode(Player*);
	virtual ~GameMode();
	virtual void initLevel(Level*);
	//virtual bool isDestroyingBlock() const;
	virtual bool startDestroyBlock(const TilePos& pos, Facing::Name face = Facing::UP);
	virtual bool destroyBlock(const TilePos& pos, Facing::Name face = Facing::UP);
	virtual bool continueDestroyBlock(const TilePos& pos, Facing::Name face);
	virtual void stopDestroyBlock();
	virtual void stopDestroyBlock(const TilePos& pos);
	virtual void sendCarriedItem(int selected);
	virtual void tick();
	virtual float getPickRange() const;
	virtual bool useItem(Player*, Level*, std::shared_ptr<ItemInstance>);
	virtual bool useItemOn(Player*, Level*, std::shared_ptr<ItemInstance>, const TilePos& pos, Facing::Name face);
	virtual void initPlayer(Player*);
	virtual void adjustPlayer(Player*);
	virtual bool canHurtPlayer();
	virtual void interact(Player*, Entity*);
	virtual void attack(Player*, Entity*);
	virtual std::shared_ptr<ItemInstance> handleInventoryMouseClick(int, int, int, bool, Player*);
	virtual void handleCloseInventory(int, Player*);
	virtual bool isCreativeType() const { return m_pPlayer->getPlayerGameType() == GameType::GAME_TYPE_CREATIVE; }
	virtual bool isSurvivalType() const { return m_pPlayer->getPlayerGameType() == GameType::GAME_TYPE_SURVIVAL; }
	virtual float getDestroyModifier() const { return 1.0; }

public:
	TilePos m_destroyingPos;
	float m_destroyProgress;
	float m_lastDestroyProgress;
	int m_destroyTicks;
	int m_destroyCooldown;
	//Fields merged with ServerPlayerGameMode
	int m_ticks;
	int m_destroyProgressStart;
	int m_delayedTickStart;
	TilePos m_delayedDestroyPos;
	bool m_bHasDelayedDestroy;
};
