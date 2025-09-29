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
class DispenserTileEntity;
class SignTileEntity;
class FishingHook;

struct Abilities
{
	bool m_bInvulnerable = false;
	bool m_bFlying = false;
	bool m_bMayFly = false;
};

class Player : public Mob
{
public:
	enum BedSleepingProblem
	{
		OK,
		NOT_POSSIBLE_HERE,
		NOT_POSSIBLE_NOW,
		TOO_FAR_AWAY,
		OTHER_PROBLEM
	};

private:
	GameType _playerGameType;
	int m_dmgSpill;
	Abilities m_abilities;
	int m_sleepTimer;

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
	virtual void alertWolves(Mob*, bool);
	virtual void awardKillScore(Entity* pKilled, int score) override;
	virtual void resetPos() override;
	virtual void die(Entity* pCulprit) override;
	virtual void aiStep() override;
	virtual void tick() override;
	virtual bool isImmobile() const override;
	virtual void updateAi() override;
	virtual void take(Entity* pEnt, int x);

	virtual void animateRespawn();
	virtual void drop(std::shared_ptr<ItemInstance> pItemInstance, bool b = false);
	virtual void startCrafting(const TilePos& pos);
	virtual void openFurnace(std::shared_ptr<FurnaceTileEntity> tileEntity);
	virtual void openContainer(Container* container);
	virtual void openTrap(std::shared_ptr<DispenserTileEntity> tileEntity);
	virtual void openTextEdit(std::shared_ptr<SignTileEntity> tileEntity);
	virtual void startDestroying();
	virtual void stopDestroying();
	virtual bool isLocalPlayer() const { return false; }
	virtual void closeContainer();
	virtual void remove() override;
	virtual void carriedChanged(std::shared_ptr<ItemInstance> instance);
	virtual void removeSelectedItem();
	virtual void travel(const Vec2& pos) override;
	virtual real getRidingHeight() override;
	virtual void handleInsidePortal() override;

	int addResource(int);
	void animateRespawn(Player*, Level*);
	void attack(Entity* pEnt);
	bool canDestroy(const Tile*) const;
	virtual void displayClientMessage(const std::string& msg);
	void drop();
	float getDestroySpeed(const Tile* tile) const;
	int getInventorySlot(int x) const;
	TilePos getRespawnPosition() const { return m_respawnPos; }
	int getScore() const { return m_score; }
	void prepareCustomTextures();
	void reallyDrop(std::shared_ptr<ItemEntity> pEnt);
	void respawn();
	void rideTick() override;
	void setDefaultHeadHeight();
	virtual int getItemIcon(ItemInstance*) override;
	void setRespawnPos(const TilePos& pos);
	void setBedSleepPos(const TilePos& pos);
	void updateSleepingPos(int);
	static TilePos checkRespawnPos(Level*, const TilePos&);
	bool isSleeping() const override { return m_bSleeping; }
	bool isSleepingLongEnough() const { return isSleeping() && m_sleepTimer >= 100; }
	float getBedSleepRot();
	Abilities& getAbilities();

	void touch(Entity* pEnt);
	GameType getPlayerGameType() const { return _playerGameType; }
	void setPlayerGameType(GameType playerGameType) 
	{ 
		_playerGameType = playerGameType; 
		getAbilities().m_bInvulnerable = m_abilities.m_bMayFly = playerGameType == GameType::GAME_TYPE_CREATIVE;
		if (playerGameType == GAME_TYPE_SURVIVAL) m_abilities.m_bFlying = false;
	}
	virtual void wake(bool, bool, bool);
	virtual BedSleepingProblem sleep(const TilePos&);
	bool isSurvival() const { return getPlayerGameType() == GAME_TYPE_SURVIVAL; }
	bool isCreative() const { return getPlayerGameType() == GAME_TYPE_CREATIVE; }
	std::shared_ptr<ItemInstance> getSelectedItem() const;
	std::shared_ptr<ItemInstance> getCarriedItem() override;
	virtual void addAdditionalSaveData(CompoundIO tag) override;
	virtual void readAdditionalSaveData(CompoundIO tag) override;

	// QUIRK: Yes, I did mean it like that, as did Mojang.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
	void interact(Entity* pEnt);
#pragma GCC diagnostic pop

protected:
	int m_jumpTriggerTime = 0;
	bool m_bSleeping;
	bool m_bIsInsidePortal;

private:
	bool isInBed();


public:
	ContainerMenu* m_containerMenu;
	InventoryMenu* m_inventoryMenu;
	Inventory* m_pInventory;
	std::shared_ptr<FishingHook> m_fishing;
	uint8_t m_userType;
	int m_score;
	float m_oBob; // field_B9C
	float m_bob;
	std::string m_name;
	int m_dimension;
	RakNet::RakNetGUID m_guid;
	TilePos m_respawnPos;
	bool m_bHasRespawnPos;
	bool m_bHasBedSleepPos;
	//TODO
	bool m_destroyingBlock;
	TilePos m_bedSleepPos;
	Vec3 m_sleepingPos;
	int m_changingDimensionDelay;
	float m_portalTime;
	float m_oPortalTime;
};

