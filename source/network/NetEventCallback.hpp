/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "RakNetTypes.h"
#include "Packet.hpp"
class Packet;
class LoginPacket;
class LoginStatusPacket;
class ReadyPacket;
class MessagePacket;
class SetTimePacket;
class StartGamePacket;
class AddPlayerPacket;
class AddMobPacket;
class AddEntityPacket;
class AddItemEntityPacket;
class AddPaintingPacket;
class RemoveEntityPacket;
class MovePlayerPacket;
class MoveEntityPacket;
class TeleportEntityPacket;
class SetEntityMotionPacket;
class SetEntityDataPacket;
class UseItemPacket;
class InteractPacket;
class InteractionPacket;
class AnimatePacket;
class PlayerActionPacket;
class PlayerCommandPacket;
class TakeItemEntityPacket;
class LevelEventPacket;
class EntityEventPacket;
class GameEventPacket;
class SetHealthPacket;
class SetRidingPacket;
class PlayerChangeDimensionPacket;
class TileUpdatePacket;
class RequestChunkPacket;
class BlockRegionUpdatePacket;
class SetEquippedItemPacket;
class SetCarriedItemPacket;
class ContainerSetContentPacket;
class ContainerSetSlotPacket;
class ContainerSetDataPacket;
class ContainerClickPacket;
class ContainerAckPacket;
class ContainerOpenPacket;
class ContainerClosePacket;
class ExplodePacket;
class LevelDataPacket;
class MapItemDataPacket;
class SignUpdatePacket;
#include "world/level/Level.hpp"
class Level;
class LevelChunk;

class NetEventCallback
{
public:
	virtual ~NetEventCallback() {}
	virtual void levelGenerated(Level*);
	virtual void onConnect(const RakNet::RakNetGUID&);
	virtual void onUnableToConnect();
	virtual void onNewClient(const RakNet::RakNetGUID&);
	virtual void onDisconnect(const RakNet::RakNetGUID&);
	// TODO: macro this with a global PacketType list or something
	virtual void handle(const RakNet::RakNetGUID&, LoginPacket*);
	virtual void handle(const RakNet::RakNetGUID&, LoginStatusPacket*);
	virtual void handle(const RakNet::RakNetGUID&, ReadyPacket*);
	virtual void handle(const RakNet::RakNetGUID&, MessagePacket*);
	virtual void handle(const RakNet::RakNetGUID&, SetTimePacket*);
	virtual void handle(const RakNet::RakNetGUID&, StartGamePacket*);
	virtual void handle(const RakNet::RakNetGUID&, AddPlayerPacket*);
	virtual void handle(const RakNet::RakNetGUID&, AddMobPacket*);
	virtual void handle(const RakNet::RakNetGUID&, AddEntityPacket*);
	virtual void handle(const RakNet::RakNetGUID&, AddItemEntityPacket*);
	virtual void handle(const RakNet::RakNetGUID&, AddPaintingPacket*);
	virtual void handle(const RakNet::RakNetGUID&, TakeItemEntityPacket*);
	virtual void handle(const RakNet::RakNetGUID&, RemoveEntityPacket*);
	virtual void handle(const RakNet::RakNetGUID&, MovePlayerPacket*);
	virtual void handle(const RakNet::RakNetGUID&, MoveEntityPacket*);
	virtual void handle(const RakNet::RakNetGUID&, SetEntityMotionPacket*);
	virtual void handle(const RakNet::RakNetGUID&, SetEntityDataPacket*);
	virtual void handle(const RakNet::RakNetGUID&, TeleportEntityPacket*);
	virtual void handle(const RakNet::RakNetGUID&, AnimatePacket*);
	virtual void handle(const RakNet::RakNetGUID&, PlayerActionPacket*);
	virtual void handle(const RakNet::RakNetGUID&, PlayerCommandPacket*);
	virtual void handle(const RakNet::RakNetGUID&, UseItemPacket*);
	virtual void handle(const RakNet::RakNetGUID&, InteractPacket*);
	virtual void handle(const RakNet::RakNetGUID&, InteractionPacket*);
	virtual void handle(const RakNet::RakNetGUID&, LevelEventPacket*);
	virtual void handle(const RakNet::RakNetGUID&, EntityEventPacket*);
	virtual void handle(const RakNet::RakNetGUID&, GameEventPacket*);
	virtual void handle(const RakNet::RakNetGUID&, SetHealthPacket*);
	virtual void handle(const RakNet::RakNetGUID&, SetRidingPacket*);
	virtual void handle(const RakNet::RakNetGUID&, PlayerChangeDimensionPacket*);
	virtual void handle(const RakNet::RakNetGUID&, TileUpdatePacket*);
	virtual void handle(const RakNet::RakNetGUID&, RequestChunkPacket*);
	virtual void handle(const RakNet::RakNetGUID&, BlockRegionUpdatePacket*);
	virtual void handle(const RakNet::RakNetGUID&, SetEquippedItemPacket*);
	virtual void handle(const RakNet::RakNetGUID&, SetCarriedItemPacket*);
	virtual void handle(const RakNet::RakNetGUID&, ContainerSetContentPacket*);
	virtual void handle(const RakNet::RakNetGUID&, ContainerSetSlotPacket*);
	virtual void handle(const RakNet::RakNetGUID&, ContainerClickPacket*);
	virtual void handle(const RakNet::RakNetGUID&, ContainerAckPacket*);
	virtual void handle(const RakNet::RakNetGUID&, ContainerSetDataPacket*);
	virtual void handle(const RakNet::RakNetGUID&, ContainerOpenPacket*);
	virtual void handle(const RakNet::RakNetGUID&, ContainerClosePacket*);
	virtual void handle(const RakNet::RakNetGUID&, ExplodePacket*);
	virtual void handle(const RakNet::RakNetGUID&, LevelDataPacket*);
	virtual void handle(const RakNet::RakNetGUID&, MapItemDataPacket*);
	virtual void handle(const RakNet::RakNetGUID&, SignUpdatePacket*);
};

