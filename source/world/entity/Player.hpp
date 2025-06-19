/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "common/Utils.hpp"
#include "thirdparty/raknet/RakNetTypes.h"
#include "world/item/Inventory.hpp"
#include "world/entity/Mob.hpp"
#include "world/entity/ItemEntity.hpp"
#include "world/gamemode/GameType.hpp"
#include "world/item/ContainerMenu.hpp"
#include "world/item/InventoryMenu.hpp"

class Inventory; // in case we're included from Inventory.hpp
class FurnaceTileEntity;

class Player : public Mob
{
private:
	GameType _playerGameType;
	int m_dmgSpill;

public:
	Player(Level* pLevel, GameType gameType);
	virtual ~Player();

	virtual void reset() override;
	virtual float getHeadHeight() const override { return 0.12f; /*@HUH: what ?*/ }
	virtual bool isShootable() const override { return true; }
	virtual bool isPlayer() const override { return true; }
	virtual bool isCreativeModeAllowed() const override { return true; }
	virtual bool hurt(Entity*, int) override;
	virtual void actuallyHurt(int) override;
	virtual void awardKillScore(Entity* pKilled, int score) override;
	virtual void resetPos() override;
	virtual void die(Entity* pCulprit) override;
	virtual void aiStep() override;
	virtual void tick() override;
	virtual bool isImmobile() const override { return m_health <= 0; }
	virtual void updateAi() override;
	virtual void take(Entity* pEnt, int x);

	virtual void animateRespawn();
	virtual void drop(const ItemInstance* pItemInstance, bool b = false);
	virtual void startCrafting(const TilePos& pos);
	virtual void openFurnace(std::shared_ptr<FurnaceTileEntity> tileEntity);
	virtual void openContainer(Container* container);
	virtual void startDestroying();
	virtual void stopDestroying();
	virtual bool isLocalPlayer() const { return false; }
	virtual void closeContainer();
	virtual void remove() override;
	virtual void carriedChanged(std::shared_ptr<ItemInstance> instance);
	virtual void removeSelectedItem();

	int addResource(int);
	void animateRespawn(Player*, Level*);
	void attack(Entity* pEnt);
	bool canDestroy(const Tile*) const;
	void displayClientMessage(const std::string& msg);
	void drop();
	float getDestroySpeed(const Tile* tile) const;
	int getInventorySlot(int x) const;
	TilePos getRespawnPosition() { return m_respawnPos; }
	int getScore() const { return m_score; }
	void prepareCustomTextures();
	void reallyDrop(std::shared_ptr<ItemEntity> pEnt);
	void respawn();
	void rideTick();
	void setDefaultHeadHeight();
	void setRespawnPos(const TilePos& pos);

	void touch(Entity* pEnt);
	GameType getPlayerGameType() const { return _playerGameType; }
	void setPlayerGameType(GameType playerGameType) { _playerGameType = playerGameType; }
	bool isSurvival() const { return getPlayerGameType() == GAME_TYPE_SURVIVAL; }
	bool isCreative() const { return getPlayerGameType() == GAME_TYPE_CREATIVE; }
	std::shared_ptr<ItemInstance> getSelectedItem() const;
	virtual void addAdditionalSaveData(std::shared_ptr<CompoundTag> tag) override;
	virtual void readAdditionalSaveData(std::shared_ptr<CompoundTag> tag) override;

	// QUIRK: Yes, I did mean it like that, as did Mojang.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
	void interact(Entity* pEnt);
#pragma GCC diagnostic pop

public:
	ContainerMenu* m_containerMenu;
	InventoryMenu* m_inventoryMenu;
	Inventory* m_pInventory;
	uint8_t m_userType;
	int m_score;
	float m_oBob; // field_B9C
	float m_bob;
	std::string m_name;
	int m_dimension;
	RakNet::RakNetGUID m_guid;
	//TODO
	TilePos m_respawnPos;
	//TODO
	bool m_bHaveRespawnPos;
	//TODO
	bool m_destroyingBlock;
};

