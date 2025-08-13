/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <RakPeer.h>
#include "ClientSideNetworkHandler.hpp"
#include "common/Utils.hpp"
#include "client/gui/screens/StartMenuScreen.hpp"
#include "client/gui/screens/ProgressScreen.hpp"
#include "client/gui/screens/DisconnectionScreen.hpp"
#include <world/tile/entity/SignTileEntity.hpp>

// This lets you make the client shut up and not log events in the debug console.
#define VERBOSE_CLIENT

#if defined(ORIGINAL_CODE) || defined(VERBOSE_CLIENT)
#define puts_ignorable(str) LOG_I(str)
#define printf_ignorable(str, ...) LOG_I(str, __VA_ARGS__)
#else
#define puts_ignorable(str)
#define printf_ignorable(str, ...)
#endif

ClientSideNetworkHandler::ClientSideNetworkHandler(Minecraft* pMinecraft, RakNetInstance* pRakNetInstance)
{
	m_pMinecraft = pMinecraft;
	m_pRakNetInstance = pRakNetInstance;
	m_pServerPeer = m_pRakNetInstance->getPeer();
	m_chunksRequested = 0;
	m_serverProtocolVersion = 0;
	m_pLevel = nullptr;
	m_field_14 = 0;
	m_field_24 = 0;
}

void ClientSideNetworkHandler::levelGenerated(Level* level)
{
#if NETWORK_PROTOCOL_VERSION >= 3
	ReadyPacket* pReadyPkt = new ReadyPacket(1);
	m_pRakNetInstance->send(pReadyPkt);
#endif

	arrangeRequestChunkOrder();
	requestNextChunk();
}

void ClientSideNetworkHandler::onConnect(const RakNet::RakNetGUID& rakGuid) // server guid
{
	RakNet::RakNetGUID localGuid = ((RakNet::RakPeer*)m_pServerPeer)->GetMyGUID();
	printf_ignorable("onConnect, server guid: %s, local guid: %s", rakGuid.ToString(), localGuid.ToString());

	m_serverGUID = rakGuid;

	LoginPacket* pLoginPkt = new LoginPacket;
	pLoginPkt->m_str = RakNet::RakString(m_pMinecraft->m_pUser->m_guid.c_str());
	pLoginPkt->m_clientNetworkVersion = NETWORK_PROTOCOL_VERSION;
	pLoginPkt->m_clientNetworkVersion2 = NETWORK_PROTOCOL_VERSION;
	
	m_pRakNetInstance->send(pLoginPkt);
}

void ClientSideNetworkHandler::onUnableToConnect()
{
	puts_ignorable("onUnableToConnect");

	// On 0.2.1 this is handled in ProgressScreen::tick(), and this function is instead left blank, but I see no reason to do the same.
	m_pMinecraft->setScreen(new DisconnectionScreen("Could not connect to server. Try again."));
}

void ClientSideNetworkHandler::onDisconnect(const RakNet::RakNetGUID& rakGuid)
{
	puts_ignorable("onDisconnect");

	if (m_pLevel)
		m_pLevel->m_bIsOnline = false;

	m_pMinecraft->m_gui.addMessage("Disconnected from server");
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, LoginStatusPacket* pPacket)
{
	puts_ignorable("LoginStatusPacket");

	switch (pPacket->m_loginStatus)
	{
	case LoginStatusPacket::STATUS_CLIENT_OUTDATED:
		m_pMinecraft->setScreen(new DisconnectionScreen("Could not connect: Outdated client!"));
		break;
	case LoginStatusPacket::STATUS_SERVER_OUTDATED:
		m_pMinecraft->setScreen(new DisconnectionScreen("Could not connect: Outdated server!"));
		break;
	}
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, MessagePacket* pMsgPkt)
{
	puts_ignorable("MessagePacket");

	m_pMinecraft->m_gui.addMessage(pMsgPkt->m_str.C_String());
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, SetTimePacket* pPacket)
{
	puts_ignorable("SetTimePacket");

	if (m_pLevel)
		m_pLevel->setTime(pPacket->m_time);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, StartGamePacket* pStartGamePkt)
{
	puts_ignorable("StartGamePacket");

	m_pLevel = new MultiPlayerLevel(
		pStartGamePkt->m_seed,
		Dimension::getNew(pStartGamePkt->m_dimension));

	m_pLevel->m_bIsOnline = true;

	GameType gameType = pStartGamePkt->m_gameType != GAME_TYPES_MAX ? pStartGamePkt->m_gameType : m_pLevel->getDefaultGameType();
	auto pLocalPlayer = std::make_shared<LocalPlayer>(m_pMinecraft, m_pLevel, m_pMinecraft->m_pUser, gameType, m_pLevel->m_pDimension->m_ID);
	pLocalPlayer->m_guid = ((RakNet::RakPeer*)m_pServerPeer)->GetMyGUID();
	
	pLocalPlayer->moveTo(
		pStartGamePkt->m_pos,
		pLocalPlayer->m_rot);

	if (gameType == GAME_TYPE_CREATIVE)
		pLocalPlayer->m_pInventory->prepareCreativeInventory();
	else
		pLocalPlayer->m_pInventory->prepareSurvivalInventory();

	m_pLevel->setTime(pStartGamePkt->m_time);

	m_serverProtocolVersion = pStartGamePkt->m_serverVersion;

	m_pMinecraft->setScreen(new ProgressScreen);
	m_pMinecraft->m_delayed.emplace_back(std::bind(&Minecraft::setLevel, m_pMinecraft, m_pLevel, "Downloading terrain", pLocalPlayer));
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, AddPlayerPacket* pAddPlayerPkt)
{
	puts_ignorable("AddPlayerPacket");

	if (!m_pLevel) return;

	auto pPlayer = std::make_shared<Player>(m_pLevel, m_pLevel->getDefaultGameType());
	m_pLevel->putEntity(pPlayer, pAddPlayerPkt->m_id);

	pPlayer->moveTo(
		pAddPlayerPkt->m_pos,
		pPlayer->m_rot);

	pPlayer->m_name = pAddPlayerPkt->m_name;
	pPlayer->m_guid = pAddPlayerPkt->m_guid;

	if (pPlayer->getPlayerGameType() == GAME_TYPE_CREATIVE)
		pPlayer->m_pInventory->prepareCreativeInventory();
	else
		pPlayer->m_pInventory->prepareSurvivalInventory();

	m_pMinecraft->m_gui.addMessage(pPlayer->m_name + " joined the game");
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, AddMobPacket* pAddMobPkt)
{
	puts_ignorable("AddMobPacket");

	if (!m_pLevel) return;

	
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, RemoveEntityPacket* pRemoveEntityPkt)
{
	if (!m_pLevel) return;

	auto pEnt = getEntity(pRemoveEntityPkt->m_id);

	if (pEnt)
		m_pLevel->removeEntity(pEnt);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, MovePlayerPacket* packet)
{
	if (!m_pLevel) return;

	auto pEntity = getEntity(packet->m_id);
	if (!pEntity)
	{
		LOG_E("No player with id %d", packet->m_id);
		return;
	}
	
	pEntity->lerpTo(packet->m_pos, packet->m_rot, 3);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, PlaceBlockPacket* pPlaceBlockPkt)
{
	puts_ignorable("PlaceBlockPacket");

	auto pPlayer = std::dynamic_pointer_cast<Player>(getEntity(pPlaceBlockPkt->m_playerID));
	if (!pPlayer)
	{
		LOG_E("No player with id %d", pPlaceBlockPkt->m_playerID);
		return;
	}

	pPlayer->swing();

	// @BUG: Not buffering the update.
	if (!areAllChunksLoaded())
		return;

	const TilePos& pos = pPlaceBlockPkt->m_pos;
	TileID tile = pPlaceBlockPkt->m_tile;
	Facing::Name face = (Facing::Name)pPlaceBlockPkt->m_face;

	if (!m_pLevel->mayPlace(tile, pos, true, face))
		return;

	Tile* pTile = Tile::tiles[tile];
	if (!m_pLevel->setTile(pos, tile))
		return;

	Tile::tiles[tile]->setPlacedOnFace(m_pLevel, pos, face);
	Tile::tiles[tile]->setPlacedBy(m_pLevel, pos, pPlayer.get(), face);

	const Tile::SoundType* pSound = pTile->m_pSound;
	m_pLevel->playSound(pos + 0.5f, pSound->m_name, 0.5f * (1.0f + pSound->volume), 0.8f * pSound->pitch);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, RemoveBlockPacket* pRemoveBlockPkt)
{
	puts_ignorable("RemoveBlockPacket");

	auto pPlayer = std::dynamic_pointer_cast<Player>(getEntity(pRemoveBlockPkt->m_playerID));
	if (!pPlayer)
	{
		LOG_E("No player with id %d", pRemoveBlockPkt->m_playerID);
		return;
	}

	pPlayer->swing();

	// @BUG: Not buffering the update.
	if (!areAllChunksLoaded())
		return;

	const TilePos& pos = pRemoveBlockPkt->m_pos;

	Tile* pTile = Tile::tiles[m_pLevel->getTile(pos)];
	int data = m_pLevel->getData(pos);
	bool setTileResult = m_pLevel->setTile(pos, TILE_AIR);

	if (pTile && setTileResult)
	{
		const Tile::SoundType* pSound = pTile->m_pSound;
		m_pLevel->playSound(pos + 0.5f, pSound->m_destroy, 0.5f * (1.0f + pSound->volume), 0.8f * pSound->pitch);

		pTile->destroy(m_pLevel, pos, data);
	}
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, UpdateBlockPacket* pkt)
{
	if (!areAllChunksLoaded())
	{
		m_bufferedBlockUpdates.push_back(SBufferedBlockUpdate(pkt->m_pos, pkt->m_tile, pkt->m_data));
		return;
	}

	m_pLevel->doSetTileAndDataNoUpdate(pkt->m_pos, pkt->m_tile, pkt->m_data);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, ChunkDataPacket* pChunkDataPkt)
{
	if (!m_pLevel)
	{
		LOG_E("Level @ handle ChunkDataPacket is 0");
		return;
	}
	m_pLevel->clearResetRegion(pChunkDataPkt->m_pos, TilePos(pChunkDataPkt->m_pos.x + pChunkDataPkt->m_xs - 1, pChunkDataPkt->m_pos.y + pChunkDataPkt->m_ys - 1, pChunkDataPkt->m_pos.z + pChunkDataPkt->m_zs - 1));
	m_pLevel->setBlocksAndData(pChunkDataPkt->m_pos, pChunkDataPkt->m_xs, pChunkDataPkt->m_ys, pChunkDataPkt->m_zs, pChunkDataPkt->m_data.GetData());
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, PlayerEquipmentPacket* pPlayerEquipmentPkt)
{
	if (!m_pLevel)
		return;

	auto pPlayer = std::dynamic_pointer_cast<Player>(getEntity(pPlayerEquipmentPkt->m_playerID));
	if (!pPlayer)
		return;

	if (pPlayer->m_guid == m_pServerPeer->GetMyGUID())
	{
		LOG_W("Attempted to modify local player's inventory");
		return;
	}

	pPlayer->m_pInventory->selectItemById(pPlayerEquipmentPkt->m_itemID, C_MAX_HOTBAR_ITEMS);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, LevelDataPacket* packet)
{
	const int uncompMagic = 12847812, compMagic = 58712758, chunkSepMagic = 284787658;
	RakNet::BitStream* bs = &packet->m_data, bs2;

	int magicNum = 0;
	bs->Read(magicNum);
	if (magicNum != compMagic && magicNum != uncompMagic)
	{
		LOG_E("Invalid level data packet with magic %d", magicNum);
		return;
	}

	// If our data is compressed
	if (magicNum == compMagic)
	{
		// Decompress it before we handle it.
		int uncompSize = 0, compSize = 0;
		bs->Read(uncompSize);
		bs->Read(compSize);

		LOG_I("Decompressing level data. Compressed: %d bytes, uncompressed: %d bytes", compSize, uncompSize);

		// Read the compressed data.
		uint8_t* pCompData = new uint8_t[compSize];
		bs->Read((char*)pCompData, compSize);

		// Inflate it.
		uint8_t* pUncompData = ZlibInflateToMemory(pCompData, compSize, uncompSize);
		SAFE_DELETE_ARRAY(pCompData);

		// If we couldn't, bail
		if (!pUncompData)
		{
			LOG_E("Failed to decompress level data!");
			return;
		}

		// Do some small scale hacks to get bs2 contain the uncompressed data.
		bs2.Reset();
		bs2.Write((const char*)pUncompData, uncompSize);
		bs2.ResetReadPointer();
		bs = &bs2;

		// Delete the uncompressed data, since we've written it to our bitstream.
		SAFE_DELETE_ARRAY(pUncompData);

		bs->Read(magicNum);
	}

	int chunksX = 0, chunksZ = 0;
	bs->Read(chunksX);
	bs->Read(chunksZ);

	if (chunksX != C_MAX_CHUNKS_X || chunksZ != C_MAX_CHUNKS_Z)
	{
		LOG_E("We don't yet support a level of size %d x %d chunks. Some chunks may disappear or be regenerated.", chunksX, chunksZ);
	}

	ChunkPos cp(0, 0);
	for (cp.x = C_MIN_CHUNKS_X; cp.x < chunksX; cp.x++)
	{
		for (cp.z = C_MIN_CHUNKS_Z; cp.z < chunksZ; cp.z++)
		{
			bs->Read(magicNum);

			if (magicNum != chunkSepMagic)
			{
			_FAIL_BECAUSE_INVALID:
				LOG_E("Aborting because level data is invalid, reading chunk %d, %d. Magic: %d", cp.x, cp.z, magicNum);
				return;
			}
			
			uint8_t ptype = 0;

			// read the data size. This'll let us know how much to read.
			int dataSize = 0;
			bs->Read(dataSize);

			LevelChunk* pChunk = m_pLevel->getChunk(cp);
			if (!pChunk || pChunk->isEmpty())
				LOG_E("No chunk at %d, %d", cp.x, cp.z);
			
			// continue reading anyway to skip over the offending chunk

			// Seems like reading a bitstream from another bitstream reads all the way
			// to the end, so we need to duplicate in this fashion.
			RakNet::BitStream bs2;
			bs2.Write(*bs, 8 * dataSize);
			
			// Ensure the packet type is correct.
			bs2.Read(ptype);
			if (ptype != PACKET_CHUNK_DATA)
				goto _FAIL_BECAUSE_INVALID;

			// Read the chunk data packet itself, and handle it.
			ChunkDataPacket cdp(pChunk);
			cdp.read(&bs2);

			if (pChunk)
				handle(guid, &cdp);

			// Handle lighting immediately, to ensure it doesn't get out of control.
			while (m_pLevel->updateLights());
		}
	}

	// All chunks are loaded. Also flush all the updates we've buffered.
	m_chunksRequested = C_MAX_CHUNKS;;
	flushAllBufferedUpdates();
}

std::shared_ptr<Entity> ClientSideNetworkHandler::getEntity(int id)
{
	return id == m_pMinecraft->m_pPlayer->m_EntityID == id ? m_pMinecraft->m_pPlayer : m_pLevel->getEntity(id);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID&, SignUpdatePacket* packet)
{
	if (m_pMinecraft->m_pLevel->hasChunkAt(packet->m_pos)) {
		auto te = std::dynamic_pointer_cast<SignTileEntity>(m_pMinecraft->m_pLevel->getTileEntity((packet->m_pos)));
		if (te) {
			for (int i = 0; i < packet->m_messages.size(); ++i)
				te->m_messages[i] = packet->m_messages[i];

			te->setChanged();
		}
	}
}

bool ClientSideNetworkHandler::areAllChunksLoaded()
{
	return m_chunksRequested >= C_MAX_CHUNKS;
}

void ClientSideNetworkHandler::arrangeRequestChunkOrder()
{
	clearChunksLoaded();
	// @TODO: Implement arrangeRequestChunkOrder()
}

void ClientSideNetworkHandler::clearChunksLoaded()
{
	// @TODO: Implement clearChunksLoaded()
}

void ClientSideNetworkHandler::requestNextChunk()
{
}

void ClientSideNetworkHandler::flushAllBufferedUpdates()
{
	for (int i = 0; i < int(m_bufferedBlockUpdates.size()); i++)
	{
		SBufferedBlockUpdate& u = m_bufferedBlockUpdates[i];
		m_pLevel->setTileAndData(u.pos, u.tile, u.data);
	}
}
