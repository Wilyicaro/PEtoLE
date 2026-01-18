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
#include "world/tile/entity/SignTileEntity.hpp"
#include "world/entity/EntityIO.hpp"
#include "world/entity/Minecart.hpp"
#include "world/entity/projectile/FishingHook.hpp"
#include "world/entity/projectile/Arrow.hpp"
#include "world/entity/projectile/Snowball.hpp"
#include "world/entity/projectile/Fireball.hpp"
#include "world/entity/projectile/ThrownEgg.hpp"
#include "world/entity/LightningBolt.hpp"
#include "world/entity/Boat.hpp"
#include "world/entity/PrimedTnt.hpp"
#include "world/entity/FallingTile.hpp"
#include "world/item/Slot.hpp"
#include "world/tile/entity/FurnaceTileEntity.hpp"
#include "world/tile/entity/DispenserTileEntity.hpp"
#include "world/level/Explosion.hpp"
#include "world/item/MapItem.hpp"

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
	m_pDataStorage = new DimensionDataStorage(nullptr);
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

}

void ClientSideNetworkHandler::onConnect(const RakNet::RakNetGUID& rakGuid) // server guid
{
	RakNet::RakNetGUID localGuid = ((RakNet::RakPeer*)m_pServerPeer)->GetMyGUID();
	printf_ignorable("onConnect, server guid: %s, local guid: %s", rakGuid.ToString(), localGuid.ToString());

	m_serverGUID = rakGuid;

	LoginPacket* pLoginPkt = new LoginPacket;
	pLoginPkt->m_str = RakNet::RakString(m_pMinecraft->m_pUser->m_username.c_str());
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

	m_pMinecraft->leaveGame();
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
	m_pLevel->m_pDataStorage = m_pDataStorage;
	m_pLevel->m_pConnection = m_pRakNetInstance;
	m_pLevel->m_bIsOnline = true;

	GameType gameType = pStartGamePkt->m_gameType != GAME_TYPES_MAX ? pStartGamePkt->m_gameType : m_pLevel->getDefaultGameType();
	auto pLocalPlayer = std::make_shared<LocalPlayer>(m_pMinecraft, m_pLevel, m_pMinecraft->m_pUser, gameType, m_pLevel->m_pDimension->m_ID);
	pLocalPlayer->m_health = pStartGamePkt->m_health;
	pLocalPlayer->m_EntityID = pStartGamePkt->m_entityId;
	pLocalPlayer->m_guid = ((RakNet::RakPeer*)m_pServerPeer)->GetMyGUID();
	
	pLocalPlayer->absMoveTo(
		pStartGamePkt->m_pos,
		pLocalPlayer->m_rot);

	m_pLevel->setTime(pStartGamePkt->m_time);

	m_serverProtocolVersion = pStartGamePkt->m_serverVersion;

	m_pMinecraft->setScreen(new ProgressScreen);
	m_pMinecraft->m_delayed.emplace_back(std::bind(&Minecraft::setLevel, m_pMinecraft, m_pLevel, "Downloading terrain", pLocalPlayer));
	m_pMinecraft->m_pStatsCounter->addStat(Stats::joinMultiplayer, 1);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, AddPlayerPacket* pAddPlayerPkt)
{
	puts_ignorable("AddPlayerPacket");

	if (!m_pLevel) return;

	auto pPlayer = std::make_shared<Player>(m_pLevel, m_pLevel->getDefaultGameType());
	pPlayer->m_bNoMove = true;
	pPlayer->m_pPos = pAddPlayerPkt->m_pos;
	pPlayer->m_name = pAddPlayerPkt->m_name;
	pPlayer->m_guid = pAddPlayerPkt->m_guid;

	int item = pAddPlayerPkt->m_itemID;
	pPlayer->m_pInventory->setSelectedItem(item ? std::make_shared<ItemInstance>(item, 1, 0) : nullptr);

	pPlayer->absMoveTo(
		PacketUtil::unpackPos(pAddPlayerPkt->m_pos),
		Vec2(PacketUtil::unpackRot(pAddPlayerPkt->m_rotY), PacketUtil::unpackRot(pAddPlayerPkt->m_rotX)));
	m_pLevel->putEntity(pAddPlayerPkt->m_id, pPlayer);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, AddMobPacket* pAddMobPkt)
{
	if (!m_pLevel) return;

	Vec2 rot = Vec2(PacketUtil::unpackRot(pAddMobPkt->m_rotY), PacketUtil::unpackRot(pAddMobPkt->m_rotX));
	std::shared_ptr<Mob> mob = std::dynamic_pointer_cast<Mob>(EntityIO::newById(pAddMobPkt->m_type, m_pLevel));
	
	mob->m_pPos = pAddMobPkt->m_pos;
	mob->m_EntityID = pAddMobPkt->m_id;
	mob->absMoveTo(PacketUtil::unpackPos(pAddMobPkt->m_pos), rot);
	mob->m_bInterpolateOnly = true;
	mob->m_bNoMove = true;
	m_pLevel->putEntity(pAddMobPkt->m_id, mob);
	if (!pAddMobPkt->m_unpack.empty())
		mob->getEntityData().assignValues(pAddMobPkt->m_unpack);

}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, AddEntityPacket* packet)
{
	Vec3 pos = PacketUtil::unpackPos(packet->m_pos);
	std::shared_ptr<Entity> ent = nullptr;
	if (packet->m_addId == AddEntityPacket::MINECART)
		ent = std::make_shared<Minecart>(m_pLevel, pos, Minecart::Type::DEFAULT);
	else if (packet->m_addId == AddEntityPacket::CHEST_MINECART)
		ent = std::make_shared<Minecart>(m_pLevel, pos, Minecart::Type::CHEST);
	else if (packet->m_addId == AddEntityPacket::FURNACE_MINECART)
		ent = std::make_shared<Minecart>(m_pLevel, pos, Minecart::Type::FURNACE);
	else if (packet->m_addId == AddEntityPacket::FISHING_HOOK)
		ent = std::make_shared<FishingHook>(m_pLevel, pos);
	else if (packet->m_addId == AddEntityPacket::ARROW)
		ent = std::make_shared<Arrow>(m_pLevel, pos);
	else if (packet->m_addId == AddEntityPacket::SNOWBALL)
		ent = std::make_shared<Snowball>(m_pLevel, pos);
	else if (packet->m_addId == AddEntityPacket::FIREBALL)
	{
		ent = std::make_shared<Fireball>(m_pLevel, pos, PacketUtil::unpackMotion(packet->m_vel));
		packet->m_data = 0;
	}
	else if (packet->m_addId == AddEntityPacket::THROWN_EGG)
		ent = std::make_shared<ThrownEgg>(m_pLevel, pos);
	else if (packet->m_addId == AddEntityPacket::BOAT)
		ent = std::make_shared<Boat>(m_pLevel, pos);
	else if (packet->m_addId == AddEntityPacket::PRIMED_TNT)
		ent = std::make_shared<PrimedTnt>(m_pLevel, pos);
	else if (packet->m_addId == AddEntityPacket::LIGHTNING_BOLT)
		ent = std::make_shared<LightningBolt>(m_pLevel, pos);
	else if (packet->m_addId == AddEntityPacket::FALLING_SAND)
		ent = std::make_shared<FallingTile>(m_pLevel, pos, Tile::sand->m_ID);
	else if (packet->m_addId == AddEntityPacket::FALLING_GRAVEL)
		ent = std::make_shared<FallingTile>(m_pLevel, pos, Tile::gravel->m_ID);
	
	if (ent)
	{
		ent->m_pPos = packet->m_pos;
		ent->m_rot = Vec2::ZERO;
		ent->m_EntityID = packet->m_id;
		m_pLevel->putEntity(packet->m_id, ent);
		if (packet->m_data > 0)
		{
			if (packet->m_addId == AddEntityPacket::ARROW)
			{
				std::shared_ptr<Entity> owner = getEntity(packet->m_data);
				if (owner->getCategory().contains(EntityCategories::MOB))
					std::dynamic_pointer_cast<Arrow>(ent)->m_owner = std::dynamic_pointer_cast<Mob>(owner);
			}

			if (packet->m_addId == AddEntityPacket::FISHING_HOOK)
			{
				std::shared_ptr<Entity> owner = getEntity(packet->m_data);
				if (owner->isPlayer())
				{
					std::shared_ptr<FishingHook> fishing = std::dynamic_pointer_cast<FishingHook>(ent);
					std::shared_ptr<Player> p = std::dynamic_pointer_cast<Player>(owner);
					fishing->m_owner = p;
					p->m_fishing = fishing;

				}
			}

			ent->lerpMotion(PacketUtil::unpackMotion(packet->m_vel));
		}
	}
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, AddItemEntityPacket* packet)
{
	Vec3 pos = PacketUtil::unpackPos(packet->m_pos);
	std::shared_ptr<ItemEntity> item = std::make_shared<ItemEntity>(m_pLevel, pos, std::make_shared<ItemInstance>(packet->m_itemID, packet->m_count, packet->m_auxValue));
	item->m_vel = packet->m_vel / 128.0f;
	item->m_pPos = packet->m_pos;
	m_pLevel->putEntity(packet->m_id, item);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, AddPaintingPacket* packet)
{
	std::shared_ptr<Painting> painting = std::make_shared<Painting>(m_pLevel, packet->m_pos, packet->m_dir, std::string(packet->m_motive));
	m_pLevel->putEntity(packet->m_id, painting);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, TakeItemEntityPacket* packet)
{
	std::shared_ptr<Entity> thrown = getEntity(packet->m_itemID);
	std::shared_ptr<Entity> thrower = getEntity(packet->m_id);
	if (!thrower)
		thrower = m_pMinecraft->m_pPlayer;

	if (thrown)
	{
		m_pLevel->playSound(thrown.get(), "random.pop", 0.2F, ((m_random.nextFloat() - m_random.nextFloat()) * 0.7F + 1.0F) * 2.0F);
		m_pMinecraft->m_pParticleEngine->add(new PickupParticle(m_pLevel, thrown, thrower, -0.5F));
		m_pLevel->removeEntity(packet->m_itemID);
	}
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
	if (!m_pMinecraft->m_pPlayer) return;

	m_pMinecraft->m_pPlayer->lerpTo(packet->m_pos, packet->m_rot, 3);
	if (m_pMinecraft->m_pScreen && m_pMinecraft->m_pPlayer->m_lastHealth <= 0)
		m_pMinecraft->setScreen(nullptr);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID&, MoveEntityPacket* packet)
{
	std::shared_ptr<Entity> ent = getEntity(packet->m_id);
	if (ent)
	{
		ent->m_pPos += packet->m_pos;
		ent->lerpTo(PacketUtil::unpackPos(ent->m_pPos), packet->m_bHasRot ? Vec2(PacketUtil::unpackRot(packet->m_rotY), PacketUtil::unpackRot(packet->m_rotX)) : ent->m_rot, 3);
	}
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, SetEntityMotionPacket* packet)
{
	std::shared_ptr<Entity> ent = getEntity(packet->m_id);
	if (ent)
		ent->lerpMotion(PacketUtil::unpackMotion(packet->m_vel));
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID&, SetEntityDataPacket* packet)
{
	std::shared_ptr<Entity> ent = getEntity(packet->m_id);
	if (ent && !packet->m_unpack.empty())
		ent->getEntityData().assignValues(packet->m_unpack);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID&, TeleportEntityPacket* packet)
{
	std::shared_ptr<Entity> ent = getEntity(packet->m_id);
	if (ent)
	{
		ent->m_pPos = packet->m_pos;
		Vec3 pos = PacketUtil::unpackPos(ent->m_pPos);
		pos.y += 1 / 64.0f;
		ent->lerpTo(pos, Vec2(PacketUtil::unpackRot(packet->m_rotY), PacketUtil::unpackRot(packet->m_rotX)), 3);
	}
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID&, InteractionPacket* packet)
{
	std::shared_ptr<Entity> ent = getEntity(packet->m_id);
	if (ent && packet->m_type == 0)
		std::dynamic_pointer_cast<Player>(ent)->startSleepInBed(packet->m_pos);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, AnimatePacket* packet)
{
	std::shared_ptr<Entity> ent = getEntity(packet->m_id);
	if (ent)
	{
		if (packet->m_action == 1)
			std::dynamic_pointer_cast<Player>(ent)->swing();
		else if (packet->m_action == 2)
			ent->animateHurt();
		else if (packet->m_action == 3)
			std::dynamic_pointer_cast<Player>(ent)->stopSleepInBed(false, false, false);
		else if (packet->m_action == 4)
		{
			//This method is only on client-side, and doesn't do anything for some reason, so we aren't going to implement it
			//std::dynamic_pointer_cast<Player>(ent)->method_494();
		}
	}
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, LevelEventPacket* packet)
{
	if (!m_pLevel) return;

	m_pLevel->levelEvent(packet->m_type, packet->m_pos, packet->m_data);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, EntityEventPacket* packet)
{
	std::shared_ptr<Entity> ent = getEntity(packet->m_id);
	if (!ent) return;
	ent->handleEntityEvent(packet->m_event);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, GameEventPacket* packet)
{
	if (!m_pLevel) return;
	switch (packet->m_event)
	{
	case GameEventPacket::NO_RESPAWN_TILE_AVAILABLE:
	{
		if (m_pMinecraft->m_pPlayer)
			m_pMinecraft->m_pPlayer->displayClientMessage("tile.bed.notValid");
		break;
	}
	case GameEventPacket::START_RAINING:
		m_pLevel->getLevelData().setRaining(true);
		m_pLevel->setRainLevel(1.0f);
		break;
	case GameEventPacket::STOP_RAINING:
		m_pLevel->getLevelData().setRaining(false);
		m_pLevel->setRainLevel(0.0f);
		break;
	default:
		break;
	}
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, SetHealthPacket* packet)
{
	if (m_pMinecraft->m_pPlayer)
		m_pMinecraft->m_pPlayer->hurtTo(packet->m_health);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, SetRidingPacket* packet)
{
	std::shared_ptr<Entity> rider = getEntity(packet->m_rider);
	std::shared_ptr<Entity> ridden = getEntity(packet->m_ridden);

	if (rider)
		rider->ride(ridden);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, PlayerChangeDimensionPacket* packet)
{
	int oldDim = m_pMinecraft->m_pPlayer->m_dimension;
	if (packet->m_dim != oldDim)
	{
		int64_t seed = m_pLevel->getLevelData().getSeed();
		m_pLevel = new MultiPlayerLevel(seed, Dimension::getNew(packet->m_dim));
		m_pLevel->m_pDataStorage = m_pDataStorage;
		m_pLevel->m_pConnection = m_pRakNetInstance;
		m_pLevel->m_bIsOnline = true;
		m_pMinecraft->setScreen(new ProgressScreen);
		m_pMinecraft->m_delayed.emplace_back(std::bind(&Minecraft::changeLevel, m_pMinecraft, m_pLevel));
	}
	else
	{
		m_pMinecraft->setScreen(new ProgressScreen);
		m_pMinecraft->m_progress.progressStart("Respawning");
		m_pMinecraft->m_pPlayer->reset();
		m_pMinecraft->m_pPlayer->resetPos();
	}
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, TileUpdatePacket* pkt)
{
	if (m_pLevel)
		m_pLevel->doSetTileAndDataNoUpdate(pkt->m_pos, pkt->m_tile, pkt->m_data);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, BlockRegionUpdatePacket* pChunkDataPkt)
{
	if (!m_pLevel)
	{
		LOG_E("Level @ handle BlockRegionUpdatePacket is 0");
		return;
	}
	m_pLevel->clearResetRegion(pChunkDataPkt->m_pos, TilePos(pChunkDataPkt->m_pos.x + pChunkDataPkt->m_xs - 1, pChunkDataPkt->m_pos.y + pChunkDataPkt->m_ys - 1, pChunkDataPkt->m_pos.z + pChunkDataPkt->m_zs - 1));
	m_pLevel->setBlocksAndData(pChunkDataPkt->m_pos, pChunkDataPkt->m_xs, pChunkDataPkt->m_ys, pChunkDataPkt->m_zs, pChunkDataPkt->m_data.GetData());
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, SetEquippedItemPacket* packet)
{
	if (!m_pLevel)
		return;

	auto ent = getEntity(packet->m_id);
	if (!ent)
		return;

	ent->setItemSlot(packet->m_slot, packet->m_itemID, packet->m_auxValue);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, ContainerSetContentPacket* packet)
{
	if (packet->m_containerId == 0)
		m_pMinecraft->m_pPlayer->m_inventoryMenu->setAll(packet->m_items);
	else if (packet->m_containerId == m_pMinecraft->m_pPlayer->m_containerMenu->m_containerId)
		m_pMinecraft->m_pPlayer->m_containerMenu->setAll(packet->m_items);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, ContainerSetSlotPacket* packet)
{
	if (!m_pMinecraft->m_pPlayer) return;

	if (packet->m_containerId == -1)
		m_pMinecraft->m_pPlayer->m_pInventory->setCarried(packet->m_item);
	else if (packet->m_containerId == 0 && packet->m_slot >= 36 && packet->m_slot < 45)
	{
		std::shared_ptr<ItemInstance> slotItem = m_pMinecraft->m_pPlayer->m_inventoryMenu->getSlot(packet->m_slot)->getItem();
		if (packet->m_item && (!slotItem || slotItem->m_count < packet->m_item->m_count))
			packet->m_item->m_popTime = 5;

		m_pMinecraft->m_pPlayer->m_inventoryMenu->setItem(packet->m_slot, packet->m_item);
	}
	else if (packet->m_containerId == m_pMinecraft->m_pPlayer->m_containerMenu->m_containerId)
		m_pMinecraft->m_pPlayer->m_containerMenu->setItem(packet->m_slot, packet->m_item);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, ContainerAckPacket* packet)
{
	ContainerMenu* menu = nullptr;
	if (packet->m_containerId == 0)
		menu = m_pMinecraft->m_pPlayer->m_inventoryMenu;
	else if (packet->m_containerId == m_pMinecraft->m_pPlayer->m_containerMenu->m_containerId)
		menu = m_pMinecraft->m_pPlayer->m_containerMenu;

	if (menu)
	{
		if (packet->m_bAccepted)
			menu->deleteBackup(packet->m_uid);
		else
		{
			menu->rollbackToBackup(packet->m_uid);
			m_pRakNetInstance->send(new ContainerAckPacket(packet->m_containerId, packet->m_uid, true));
		}
	}
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, ContainerSetDataPacket* packet)
{
	if (m_pMinecraft->m_pPlayer->m_containerMenu && m_pMinecraft->m_pPlayer->m_containerMenu->m_containerId == packet->m_containerId)
		m_pMinecraft->m_pPlayer->m_containerMenu->setData(packet->m_id, packet->m_value);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, ContainerOpenPacket* packet)
{
	if (packet->m_type == 0)
	{
		m_pMinecraft->m_pPlayer->openContainer(new SimpleContainer(packet->m_size, packet->m_title.C_String()));
		m_pMinecraft->m_pPlayer->m_containerMenu->m_containerId = packet->m_containerId;
	}
	else if (packet->m_type == 2)
	{
		m_pMinecraft->m_pPlayer->openFurnace(std::make_shared<FurnaceTileEntity>());
		m_pMinecraft->m_pPlayer->m_containerMenu->m_containerId = packet->m_containerId;
	}
	else if (packet->m_type == 3)
	{
		m_pMinecraft->m_pPlayer->openTrap(std::make_shared<DispenserTileEntity>());
		m_pMinecraft->m_pPlayer->m_containerMenu->m_containerId = packet->m_containerId;
	}
	else if (packet->m_type == 1)
	{
		m_pMinecraft->m_pPlayer->startCrafting(m_pMinecraft->m_pPlayer->m_pos);
		m_pMinecraft->m_pPlayer->m_containerMenu->m_containerId = packet->m_containerId;
	}
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, ContainerClosePacket* packet)
{
	m_pMinecraft->m_pPlayer->closeContainer();
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, ExplodePacket* packet)
{
	Explosion explosion(m_pLevel, nullptr, packet->m_pos, packet->m_range);
	explosion.m_toBlow = packet->m_toBlow;
	explosion.addParticles();
}

//@Note: this will be removed
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
			if (ptype != PACKET_BLOCK_REGION_UPDATE)
				goto _FAIL_BECAUSE_INVALID;

			// Read the chunk data packet itself, and handle it.
			BlockRegionUpdatePacket cdp(pChunk);
			cdp.read(&bs2);

			if (pChunk)
				handle(guid, &cdp);

			// Handle lighting immediately, to ensure it doesn't get out of control.
			while (m_pLevel->updateLights());
		}
	}

	// All chunks are loaded. Also flush all the updates we've buffered.
	flushAllBufferedUpdates();
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, MapItemDataPacket* packet)
{
	if (packet->m_item == Item::map->m_itemID)
		MapItem::createSavedData(packet->m_mapId, m_pLevel)->applyToMap(packet->m_data);
	else
		LOG_I("Unknown itemid: %i", packet->m_mapId);
}

std::shared_ptr<Entity> ClientSideNetworkHandler::getEntity(int id)
{
	return m_pMinecraft->m_pPlayer && id == m_pMinecraft->m_pPlayer->m_EntityID ? m_pMinecraft->m_pPlayer : m_pLevel->getEntity(id);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID&, SignUpdatePacket* packet)
{
	if (m_pMinecraft->m_pLevel->hasChunkAt(packet->m_pos))
	{
		auto te = std::dynamic_pointer_cast<SignTileEntity>(m_pMinecraft->m_pLevel->getTileEntity((packet->m_pos)));
		if (te)
		{
			for (int i = 0; i < packet->m_messages.size(); ++i)
				te->m_messages[i] = packet->m_messages[i];

			te->setChanged();
		}
	}
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID&, UpdateStatPacket* packet)
{
	if (m_pMinecraft->m_pPlayer)
		m_pMinecraft->m_pPlayer->awardClientStat(Stats::byId(packet->m_id), packet->m_amount);
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
