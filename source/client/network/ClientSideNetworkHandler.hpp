/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "network/NetEventCallback.hpp"
#include "client/app/Minecraft.hpp"
#include "network/RakNetInstance.hpp"
#include "world/level/MultiPlayerLevel.hpp"

struct SBufferedBlockUpdate
{
	TilePos pos;
	uint8_t tile, data;

	SBufferedBlockUpdate(const TilePos& pos, TileID tile, int data) :
		pos(pos), tile(uint8_t(tile)), data(uint8_t(data))
	{}
};

// @TODO: Rename to ClientNetworkHandler?
class ClientSideNetworkHandler : public NetEventCallback
{
public:
	ClientSideNetworkHandler(Minecraft*, RakNetInstance*);

	void levelGenerated(Level*) override;
	void onConnect(const RakNet::RakNetGUID&) override;
	void onDisconnect(const RakNet::RakNetGUID&) override;
	void onUnableToConnect() override;
	void handle(const RakNet::RakNetGUID&, LoginStatusPacket*) override;
	void handle(const RakNet::RakNetGUID&, MessagePacket*) override;
	void handle(const RakNet::RakNetGUID&, SetTimePacket*) override;
	void handle(const RakNet::RakNetGUID&, StartGamePacket*) override;
	void handle(const RakNet::RakNetGUID&, AddPlayerPacket*) override;
	void handle(const RakNet::RakNetGUID&, AddMobPacket*) override;
	void handle(const RakNet::RakNetGUID&, AddEntityPacket*) override;
	void handle(const RakNet::RakNetGUID&, AddItemEntityPacket*) override;
	void handle(const RakNet::RakNetGUID&, AddPaintingPacket*) override;
	void handle(const RakNet::RakNetGUID&, TakeItemEntityPacket*) override;
	void handle(const RakNet::RakNetGUID&, RemoveEntityPacket*) override;
	void handle(const RakNet::RakNetGUID&, MovePlayerPacket*) override;
	void handle(const RakNet::RakNetGUID&, MoveEntityPacket*) override;
	void handle(const RakNet::RakNetGUID&, SetEntityMotionPacket*) override;
	void handle(const RakNet::RakNetGUID&, SetEntityDataPacket*) override;
	void handle(const RakNet::RakNetGUID&, TeleportEntityPacket*) override;
	void handle(const RakNet::RakNetGUID&, InteractionPacket*) override;
	void handle(const RakNet::RakNetGUID&, AnimatePacket*) override;
	void handle(const RakNet::RakNetGUID&, LevelEventPacket*) override;
	void handle(const RakNet::RakNetGUID&, EntityEventPacket*) override;
	void handle(const RakNet::RakNetGUID&, GameEventPacket*) override;
	void handle(const RakNet::RakNetGUID&, SetHealthPacket*) override;
	void handle(const RakNet::RakNetGUID&, PlayerChangeDimensionPacket*) override;
	void handle(const RakNet::RakNetGUID&, TileUpdatePacket*) override;
	void handle(const RakNet::RakNetGUID&, BlockRegionUpdatePacket*) override;
	void handle(const RakNet::RakNetGUID&, SetEquippedItemPacket*) override;
	void handle(const RakNet::RakNetGUID&, ContainerSetContentPacket*) override;
	void handle(const RakNet::RakNetGUID&, ContainerSetSlotPacket*) override;
	void handle(const RakNet::RakNetGUID&, ContainerAckPacket*) override;
	void handle(const RakNet::RakNetGUID&, ContainerSetDataPacket*) override;
	void handle(const RakNet::RakNetGUID&, ContainerOpenPacket*) override;
	void handle(const RakNet::RakNetGUID&, ContainerClosePacket*) override;
	void handle(const RakNet::RakNetGUID&, ExplodePacket*) override;
	void handle(const RakNet::RakNetGUID&, LevelDataPacket*) override;
	void handle(const RakNet::RakNetGUID&, SignUpdatePacket*) override;

	std::shared_ptr<Entity> getEntity(int);
	
	void arrangeRequestChunkOrder();
	void clearChunksLoaded();
	void requestNextChunk();
	void flushAllBufferedUpdates(); // inlined

private:
	Minecraft* m_pMinecraft;
	MultiPlayerLevel* m_pLevel;
	Random m_random;
	RakNetInstance* m_pRakNetInstance;
	RakNet::RakPeerInterface* m_pServerPeer;
	int m_field_14;
	RakNet::RakNetGUID m_serverGUID;
	int m_field_24;
	std::vector<SBufferedBlockUpdate> m_bufferedBlockUpdates;
	int m_chunksRequested;
	int m_serverProtocolVersion;
};

