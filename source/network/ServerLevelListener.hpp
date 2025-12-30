 /********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>
#include "world/level/Level.hpp"
#include "world/level/LevelListener.hpp"

class ServerSideNetworkHandler;

class ServerLevelListener : public LevelListener
{
public:
	ServerLevelListener() : m_pConnection(nullptr), m_pLevel(nullptr) {};
	ServerLevelListener(ServerSideNetworkHandler* connection, Level* level);
	~ServerLevelListener();
	void tileChanged(const TilePos& pos) override;
	void entityAdded(const std::shared_ptr<Entity>&) override;
	void entityRemoved(const std::shared_ptr<Entity>&) override;
	void levelEvent(Player*, int event, const TilePos& pos, int data) override;

public:
	ServerSideNetworkHandler* m_pConnection;
	Level* m_pLevel;
};

