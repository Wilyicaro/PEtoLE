/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "world/entity/Player.hpp"
#include "client/player/input/IMoveInput.hpp"
#include "client/player/input/User.hpp"

class Minecraft;

class LocalPlayer : public Player
{
public:
	LocalPlayer(Minecraft*, Level*, User*, GameType, int);
	virtual ~LocalPlayer();

	virtual bool isSneaking() const override;
	virtual void animateRespawn() override;
	virtual void aiStep() override;
	virtual int move(const Vec3& pos) override;
	virtual void tick() override;
	virtual void swing() override;
	virtual void updateAi() override;
	virtual bool isLocalPlayer() const override { return true; }
	virtual void drop(std::shared_ptr<ItemInstance>, bool b = false) override;
	virtual bool isImmobile() const override;
	virtual void startCrafting(const TilePos& pos) override;
	virtual void openFurnace(std::shared_ptr<FurnaceTileEntity> furnace) override;
	virtual void openContainer(Container* container) override;
	virtual void openTrap(std::shared_ptr<DispenserTileEntity> tileEntity) override;
	virtual void openTextEdit(std::shared_ptr<SignTileEntity> tileEntity) override;
	virtual void displayClientMessage(const std::string& msg) override;
	virtual int getItemIcon(ItemInstance*) override;
	virtual void hurtTo(int);

	void calculateFlight(const Vec3& pos);
	void closeContainer() override;
	void respawn() override;
	void toggleDimension(int dim = -1) override;

private:
	Vec3 m_lastSentPos;
	Vec2 m_lastSentRot;

public:
	int field_BEC;
	Vec3 field_BF0;
	float field_BFC;
	float field_C00;
	float field_C04;
	float field_C08;
	float field_C0C;
	float field_C10;
	float field_C14;
	float field_C18;
	float field_C1C;
	int m_nAutoJumpFrames;
	bool m_bLastSneaked;
	int field_C38;
	Minecraft* m_pMinecraft;
	IMoveInput* m_pMoveInput;
	Vec2 m_renderArmRot;
	Vec2 m_lastRenderArmRot;
};
