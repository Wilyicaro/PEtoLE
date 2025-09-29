/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ServerSideNetworkHandler.hpp"
#include "common/Utils.hpp"
#include <world/entity/EntityIO.hpp>
#include <client/locale/Language.hpp>

// This lets you make the server shut up and not log events in the debug console.
#define VERBOSE_SERVER

#if defined(ORIGINAL_CODE) || defined(VERBOSE_SERVER)
#define puts_ignorable(str) LOG_I(str)
#define printf_ignorable(str, ...) LOG_I(str, __VA_ARGS__)
#else
#define puts_ignorable(str)
#define printf_ignorable(str, ...)
#endif

ServerSideNetworkHandler::ServerSideNetworkHandler(Minecraft* minecraft, RakNetInstance* rakNetInstance)
{
	m_pMinecraft = minecraft;
	m_pRakNetInstance = rakNetInstance;
	allowIncomingConnections(false);
	m_pRakNetPeer = m_pRakNetInstance->getPeer();
	m_bAllowIncoming = false;

	setupCommands();
}

ServerSideNetworkHandler::~ServerSideNetworkHandler()
{
	m_onlinePlayers.clear();
}

void ServerSideNetworkHandler::levelGenerated(Level* level)
{
	if (m_pMinecraft->m_pPlayer)
	{
		m_pMinecraft->m_pPlayer->m_guid = m_pRakNetPeer->GetMyGUID();
	}

	allowIncomingConnections(m_pMinecraft->getOptions()->m_bServerVisibleDefault);

	m_onlinePlayers[m_pMinecraft->m_pPlayer->m_guid] = new OnlinePlayer(m_pMinecraft->m_pPlayer, m_pMinecraft->m_pPlayer->m_guid);
}

void ServerSideNetworkHandler::onNewClient(const RakNet::RakNetGUID& guid)
{
	printf_ignorable("onNewClient, client guid: %s", guid.ToString());
}

void ServerSideNetworkHandler::onDisconnect(const RakNet::RakNetGUID& guid)
{
	puts_ignorable("onDisconnect");

	OnlinePlayer* pOnlinePlayer = getPlayerByGUID(guid);

	if (!pOnlinePlayer)
		return;

	auto& pPlayer = pOnlinePlayer->m_pPlayer;

	// erase it from the map
	m_onlinePlayers.erase(m_onlinePlayers.find(guid)); // it better be in our map

	// tell everyone that they left the game
	displayGameMessage(pPlayer->m_name + " disconnected from the game");
	m_pRakNetInstance->send(new RemoveEntityPacket(pPlayer->m_EntityID));

	// remove it from our world
	pPlayer->m_pLevel->removeEntity(pPlayer);

	// delete the online player's entry.
	delete pOnlinePlayer;
}

void ServerSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, LoginPacket* packet)
{
	if (!m_bAllowIncoming)
		return;

	puts_ignorable("LoginPacket");

	// if they're already online, fail
	if (getPlayerByGUID(guid))
	{
		LOG_E("That player is already in the world!");
		return;
	}

#if NETWORK_PROTOCOL_VERSION >= 3
	LoginStatusPacket::LoginStatus loginStatus = LoginStatusPacket::STATUS_SUCCESS;
	if (packet->m_clientNetworkVersion < NETWORK_PROTOCOL_VERSION_MIN)
	{
		loginStatus = LoginStatusPacket::STATUS_CLIENT_OUTDATED;
	}
	else if (packet->m_clientNetworkVersion2 > NETWORK_PROTOCOL_VERSION)
	{
		loginStatus = LoginStatusPacket::STATUS_SERVER_OUTDATED;
	}

	if (loginStatus != LoginStatusPacket::STATUS_SUCCESS)
	{
		LoginStatusPacket lsp = LoginStatusPacket(loginStatus);

		RakNet::BitStream lspbs;
		lsp.write(&lspbs);
		m_pRakNetPeer->Send(&lspbs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, guid, false);

		return;
	}
#endif
	Level* level = m_pMinecraft->m_pMinecraftServer->getLevel();
	level->validateSpawn();
	auto pPlayer = std::make_shared<Player>(level, level->getLevelData().getGameType());
	pPlayer->resetPos();
	pPlayer->m_guid = guid;
	pPlayer->m_name = std::string(packet->m_str.C_String());

	m_onlinePlayers[guid] = new OnlinePlayer(pPlayer, guid);

	StartGamePacket sgp;
	sgp.m_seed = level->getSeed();
	sgp.m_levelVersion = level->getLevelData().getVersion();
	sgp.m_gameType = pPlayer->getPlayerGameType();
	sgp.m_entityId = pPlayer->m_EntityID;
	sgp.m_pos = pPlayer->m_pos;
	sgp.m_dimension = pPlayer->m_dimension;
	sgp.m_serverVersion = NETWORK_PROTOCOL_VERSION;
	sgp.m_time = level->getTime();
	
	RakNet::BitStream sgpbs;
	sgp.write(&sgpbs);
	m_pRakNetPeer->Send(&sgpbs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, guid, false);

	// @TODO: Move everything below into response to ReadyPacket

	// send the connecting player info about all other players in the world
	for (int i = 0; i < int(level->m_players.size()); i++)
	{
		auto& player = level->m_players[i];
		AddPlayerPacket app(player.get());
		RakNet::BitStream appbs;
		app.write(&appbs);
		m_pRakNetPeer->Send(&appbs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, guid, false);
	}

	level->addEntity(pPlayer);

	if (pPlayer->isCreative())
		pPlayer->m_pInventory->prepareCreativeInventory();

	m_pMinecraft->m_gui.addMessage(pPlayer->m_name + " joined the game");

	AddPlayerPacket app(pPlayer.get());
	RakNet::BitStream appbs;
	app.write(&appbs);
	m_pRakNetPeer->Send(&appbs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, guid, true);
}

void ServerSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, MessagePacket* packet)
{
	OnlinePlayer* pOP = getPlayerByGUID(guid);
	if (!pOP)
	{
		LOG_W("MessagePacket: That jerk %s doesn't actually exist", guid.ToString());
		return;
	}

	// don't let players send empty messages
	std::string msg(packet->m_str.C_String());
	if (msg.empty())
		return;

	if (msg[0] == '/')
	{
		LOG_I("CMD: %s: %s", pOP->m_pPlayer->m_name.c_str(), msg.c_str());

		std::stringstream ss(msg);
		ss.get(); // skip the /
		std::vector<std::string> parms;
		std::string currparm, cmdname;
		// get cmd name
		std::getline(ss, cmdname, ' ');
		// get cmd parms
		while (std::getline(ss, currparm, ' '))
			parms.push_back(currparm);

		CommandFunction func;
		CommandMap::iterator iter = m_commands.find(cmdname);
		if (iter == m_commands.end())
		{
			sendMessage(pOP, "Unknown command. Type /? for a list of commands.");
			return;
		}

		func = iter->second;

		(this->*func)(pOP, parms);

		return;
	}

	LOG_I("MSG: <%s> %s", pOP->m_pPlayer->m_name.c_str(), msg.c_str());

	// send everyone the message
	std::string gameMessage = "<" + pOP->m_pPlayer->m_name + "> " + msg;
	displayGameMessage(gameMessage);
}

void ServerSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, MovePlayerPacket* packet)
{
	//not in the original
	puts_ignorable("MovePlayerPacket");


	auto& pEntity = getPlayerByGUID(guid)->m_pPlayer;
	if (pEntity)
		pEntity->lerpTo(packet->m_pos, packet->m_rot, 3);

	redistributePacket(packet, guid);
}

void ServerSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, PlaceBlockPacket* packet)
{
	auto& pMob = getPlayerByGUID(guid)->m_pPlayer;
	if (!pMob)
		return;

	TileID tile = packet->m_tile;
	Facing::Name face = (Facing::Name)packet->m_face;
	TilePos pos = packet->m_pos;

	printf_ignorable("PlaceBlockPacket: %d", tile);

	if (!pMob->m_pLevel->mayPlace(tile, pos, true, face))
		return;

	if (pMob->m_pLevel->setTile(pos, tile))
	{
		Tile::tiles[tile]->setPlacedOnFace(pMob->m_pLevel, pos, face);
		Tile::tiles[tile]->setPlacedBy(pMob->m_pLevel, pos, pMob.get(), face);

		const Tile::SoundType* pSound = Tile::tiles[tile]->m_pSound;
		pMob->m_pLevel->playSound(pos + 0.5f, pSound->m_name, 0.5f * (pSound->volume + 1.0f), pSound->pitch * 0.8f);
	}

	redistributePacket(packet, guid);
}

void ServerSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, RemoveBlockPacket* packet)
{
	puts_ignorable("RemoveBlockPacket");

	auto& pPlayer = getPlayerByGUID(guid)->m_pPlayer;
	if (!pPlayer)
		return;

	TilePos pos = packet->m_pos;
	Tile* pTile = Tile::tiles[pPlayer->m_pLevel->getTile(pos)];
	//int data = m_pLevel->getData(pos);
	bool setTileResult = pPlayer->m_pLevel->setTile(pos, TILE_AIR);
	if (pTile && setTileResult)
	{
		const Tile::SoundType* pSound = pTile->m_pSound;
		pPlayer->m_pLevel->playSound(pos + 0.5f, pSound->m_destroy, 0.5f * (pSound->volume + 1.0f), pSound->pitch * 0.8f);

		// redistribute the packet only if needed
		redistributePacket(packet, guid);
	}
}

void ServerSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, PlayerEquipmentPacket* packet)
{
	auto& pPlayer = getPlayerByGUID(guid)->m_pPlayer;
	if (!pPlayer)
	{
		LOG_W("No player with id %d", packet->m_playerID);
		return;
	}

	if (pPlayer->m_guid == m_pRakNetPeer->GetMyGUID())
	{
		LOG_W("Attempted to modify local player's inventory");
		return;
	}

	pPlayer->m_pInventory->selectItem(packet->m_itemID);

	redistributePacket(packet, guid);
}

void ServerSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, RequestChunkPacket* packet)
{
	puts_ignorable("RequestChunkPacket");

	Level* level = getPlayerByGUID(guid)->m_pPlayer->m_pLevel;

	if (packet->m_chunkPos.x == -9999)
	{
		m_pRakNetInstance->send(guid, new LevelDataPacket(level));
		return;
	}

	LevelChunk* pChunk = level->getChunk(packet->m_chunkPos);
	if (!pChunk)
	{
		LOG_E("No chunk at %d, %d", packet->m_chunkPos.x, packet->m_chunkPos.z);
		return;
	}

	// @NOTE: this allows the client to request empty chunks. Is that okay?
	ChunkDataPacket cdp(pChunk);

	RakNet::BitStream bs;
	cdp.write(&bs);

	m_pRakNetPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, guid, false);
}

void ServerSideNetworkHandler::allowIncomingConnections(bool b)
{
	if (b)
	{
		m_pRakNetInstance->announceServer(m_pMinecraft->getOptions()->m_playerName.get());
	}
	else
	{
		m_pRakNetInstance->announceServer("");
	}

	m_bAllowIncoming = b;
}

void ServerSideNetworkHandler::displayGameMessage(const std::string& msg)
{
	m_pMinecraft->m_gui.addMessage(msg);
	m_pRakNetInstance->send(new MessagePacket(msg));
}

void ServerSideNetworkHandler::sendMessage(const RakNet::RakNetGUID& guid, const std::string& msg)
{
	if (m_pRakNetPeer->GetMyGUID() == guid)
	{
		m_pMinecraft->m_gui.addMessage(msg);
		return;
	}

	m_pRakNetInstance->send(guid, new MessagePacket(msg));
}

void ServerSideNetworkHandler::sendMessage(OnlinePlayer* player, const std::string& msg)
{
	sendMessage(player->m_guid, msg);
}

void ServerSideNetworkHandler::redistributePacket(Packet* packet, const RakNet::RakNetGUID& source)
{
	RakNet::BitStream bs;
	packet->write(&bs);

	m_pRakNetPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, source, true);
}

OnlinePlayer* ServerSideNetworkHandler::getPlayerByGUID(const RakNet::RakNetGUID& guid)
{
	OnlinePlayerMap::iterator iter = m_onlinePlayers.find(guid);
	if (iter == m_onlinePlayers.end())
		return nullptr;

	return iter->second;
}

///////////////// In-Game Commands /////////////////

void ServerSideNetworkHandler::setupCommands()
{
	m_commands["?"]      = &ServerSideNetworkHandler::commandHelp;
	m_commands["help"]   = &ServerSideNetworkHandler::commandHelp;
	m_commands["stats"]  = &ServerSideNetworkHandler::commandStats;
	m_commands["time"]   = &ServerSideNetworkHandler::commandTime;
	m_commands["seed"]   = &ServerSideNetworkHandler::commandSeed;
	m_commands["tp"]     = &ServerSideNetworkHandler::commandTP;
	m_commands["summon"] = &ServerSideNetworkHandler::commandSummon;
	m_commands["gamemode"] = &ServerSideNetworkHandler::commandGamemode;
	m_commands["give"] = &ServerSideNetworkHandler::commandGive;
	m_commands["toggledownfall"] = &ServerSideNetworkHandler::commandToggledownfall;
}

bool ServerSideNetworkHandler::checkPermissions(OnlinePlayer* player)
{
	if (player->m_pPlayer != m_pMinecraft->m_pPlayer)
	{
		sendMessage(player, "Sorry, only the host can use this command at the moment");
		return false;
	}

	return true;
}

void ServerSideNetworkHandler::commandHelp(OnlinePlayer* player, const std::vector<std::string>& parms)
{
	std::stringstream ss;
	ss << ">> Available commands:";

	for (CommandMap::iterator it = m_commands.begin(); it != m_commands.end(); ++it)
	{
		ss << " /" << it->first;
	}

	sendMessage(player, ss.str());
}

void ServerSideNetworkHandler::commandStats(OnlinePlayer* player, const std::vector<std::string>& parms)
{
	std::stringstream ss;
	ss << "Server uptime: " << getTimeS() << " seconds.\n";
	ss << "Host's name: " << m_pMinecraft->m_pUser->m_guid << "\n";

	int nPlayers = int(m_onlinePlayers.size());
	if (nPlayers == 1)
		ss << "There is 1 player online.";
	else
		ss << "There are " << nPlayers << " players online.";

	sendMessage(player, ss.str());
}

void ServerSideNetworkHandler::commandTime(OnlinePlayer* player, const std::vector<std::string>& parms)
{
	if (!parms.empty())
	{
		int t = 0;
		if (!sscanf(parms[0].c_str(), "%d", &t))
		{
			sendMessage(player, "Usage: /time [new time value]");
			return;
		}

		if (!checkPermissions(player)) return;

		player->m_pPlayer->m_pLevel->setTime(t);

		sendMessage(player, "Time has been set to " + parms[0]);

		return;
	}

	std::stringstream ss;
	ss << "In-game time: ";
	ss << player->m_pPlayer->m_pLevel->getTime();
	ss << ". Day " << player->m_pPlayer->m_pLevel->getTime() / 24000;
	sendMessage(player, ss.str());
}

void ServerSideNetworkHandler::commandSeed(OnlinePlayer* player, const std::vector<std::string>& parms)
{
	std::stringstream ss;
	ss << "World generation seed: ";
	ss << player->m_pPlayer->m_pLevel->getSeed();
	sendMessage(player, ss.str());
}

void ServerSideNetworkHandler::commandTP(OnlinePlayer* player, const std::vector<std::string>& parms)
{
	if (parms.size() != 3)
	{
		sendMessage(player, "Usage: /tp <x> <y> <z>");
		return;
	}
    
	if (!checkPermissions(player)) return;
    
	Vec3 pos = player->m_pPlayer->getPos(1.0f);
    
	std::stringstream ss;
	if (parms[0] != "~")
	{
		ss.str(parms[0]);
		ss.clear();
		ss >> pos.x;
	}
	if (parms[1] != "~")
	{
		ss.str(parms[1]);
		ss.clear();
		ss >> pos.y;
	}
	if (parms[2] != "~")
	{
		ss.str(parms[2]);
		ss.clear();
		ss >> pos.z;
	}
    
	ss.clear();
    
	player->m_pPlayer->setPos(pos);
	pos = player->m_pPlayer->getPos(1.0f);

	ss << "Teleported to " << pos.x << ", " << pos.y << ", " << pos.z;
    
	sendMessage(player, ss.str());
}

void ServerSideNetworkHandler::commandSummon(OnlinePlayer* player, const std::vector<std::string>& parms)
{
	if (!player->m_pPlayer->m_pLevel)
		return;

	const int parmsSize = parms.size();

	if (parmsSize != 1 && parmsSize != 4 && parmsSize != 5)
	{
		sendMessage(player, "Usage: /summon <entity> <x> <y> <z> <amount>");
		return;
	}

	if (!checkPermissions(player)) return;

	std::string entityName;
	std::stringstream ss;
	ss.str(parms[0]);
	ss >> entityName;

	if (EntityIO::contains(entityName))
	{
		Vec3 pos = player->m_pPlayer->getPos(1.0f);
		pos.y -= player->m_pPlayer->m_heightOffset + player->m_pPlayer->m_ySlideOffset;

		if (parmsSize >= 4)
		{
			if (parms[1] != "~")
			{
				ss.str(parms[1]);
				ss.clear();
				ss >> pos.x;
			}
			if (parms[2] != "~")
			{
				ss.str(parms[2]);
				ss.clear();
				ss >> pos.y;
			}
			if (parms[3] != "~")
			{
				ss.str(parms[3]);
				ss.clear();
				ss >> pos.z;
			}
		}

		int amount = 1;
		if (parmsSize >= 5)
		{
			ss.str(parms[4]);
			ss.clear();
			ss >> amount;
		}

		ss.clear();
		bool success = false;

		for (int i = 0; i++ < amount;)
		{
			auto mob = EntityIO::newEntity(entityName, player->m_pPlayer->m_pLevel);
			if (mob == nullptr)
			{
				ss << "Unable to summon object";
				break;
			}
			mob->setPos(pos);
			player->m_pPlayer->m_pLevel->addEntity(mob);
			if (!success) success = true;
		}

		if (success)
		{
			ss << "Object successfully summoned";
		}
	}
	else
	{
		ss.clear();
		ss << "Unable to summon invalid entity type";
	}

	sendMessage(player, ss.str());
}

void ServerSideNetworkHandler::commandGamemode(OnlinePlayer* player, const std::vector<std::string>& args)
{
	if (!checkPermissions(player)) return;

	const int argsSize = args.size();

	if (!argsSize || argsSize > 2)
	{
		sendMessage(player, "Usage: /gamemode <gamemode> <player>");
		return;
	}

	OnlinePlayer* cPlayer = player;

	if (argsSize == 2 && args[1] != cPlayer->m_pPlayer->m_name)
	{
		bool success = false;
		for (auto p : m_onlinePlayers)
		{
			if (p.second == cPlayer) continue;
			if (p.second->m_pPlayer->m_name == args[1]) {
				success = true;
				break;
			}
		}

		if (!success)
			sendMessage(player, "Unable to change gamemode of unknown player");
	}

	std::stringstream ss;
	ss.str(args[0]);
	int mode = 0;
	ss >> mode;

	if (mode == 0 || mode == 1)
	{
		cPlayer->m_pPlayer->setPlayerGameType((GameType) mode);
		m_pMinecraft->setGameMode(cPlayer->m_pPlayer->getPlayerGameType());
		sendMessage(player, "Gamemode sucessfully changed");
	}
	else
	{
		sendMessage(player, "Unable to change to invalid gamemode");
	}
}

void ServerSideNetworkHandler::commandGive(OnlinePlayer* player, const std::vector<std::string>& args)
{
	if (!checkPermissions(player)) return;

	const int argsSize = args.size();

	if (!argsSize || argsSize > 4)
	{
		sendMessage(player, "Usage: /give <item id> <count> <aux> <player>");
		return;
	}

	OnlinePlayer* cPlayer = player;

	if (argsSize >= 4 && args[2] != cPlayer->m_pPlayer->m_name)
	{
		bool success = false;
		for (auto p : m_onlinePlayers)
		{
			if (p.second == cPlayer) continue;
			if (p.second->m_pPlayer->m_name == args[2]) {
				success = true;
				break;
			}
		}

		if (!success)
			sendMessage(player, "Unable to give to unknown player");
	}



	std::stringstream ss;
	ss.str(args[0]);
	int itemId = 0;
	ss >> itemId;

	auto item = Item::items[itemId];


	if (!item)
	{
		sendMessage(player, "Unable to give an invalid item id");
		return;
	}

	int count = 1;
	if (argsSize >= 2)
	{
		ss.clear();
		ss.str(args[1]);
		ss >> count;
	}

	int data = 0;
	if (argsSize >= 3)
	{
		ss.clear();
		ss.str(args[2]);
		ss >> data;
	}

	if (count)
	{
		cPlayer->m_pPlayer->m_pInventory->add(std::make_shared<ItemInstance>(item, count, data));
		sendMessage(player, Util::format("Given %s", Language::getInstance()->get(item->getDescriptionId()).c_str()));
	}
	else
	{
		sendMessage(player, "Unable to give an invalid item count");
	}
}

void ServerSideNetworkHandler::commandToggledownfall(OnlinePlayer* player, const std::vector<std::string>&)
{
	player->m_pPlayer->m_pLevel->getLevelData().setRaining(!player->m_pPlayer->m_pLevel->isRaining());
	std::stringstream ss;
	ss << "Toggling rain and snow, hold on...";
	sendMessage(player, ss.str());
}
