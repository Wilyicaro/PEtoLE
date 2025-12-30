/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>
#include <array>
#include "world/phys/Vec3T.hpp"
#include "world/phys/Vec2.hpp"
#include "world/gamemode/GameType.hpp"
#include "world/entity/Player.hpp"
#include "RakNetTypes.h"
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "NetEventCallback.hpp"
#include "PacketUtil.hpp"
#include "world/entity/Painting.hpp"

#define NETWORK_PROTOCOL_VERSION_MIN 1 // ?
#define NETWORK_PROTOCOL_VERSION 2	   // 0.1.1
//#define NETWORK_PROTOCOL_VERSION 3	   // 0.2.1

class NetEventCallback;
class Level;
class LevelChunk;

// RakNet requires this to be cast to an "unsigned char" before being written to the BitStream
enum ePacketType
#ifndef USE_OLD_CPP
	: uint8_t // this is compiled as a 32-bit integer in C++03 and earlier, and we obviously CANNOT afford a 24-bit inconsistency.
	// TODO: WritePacketType function that casts this down to a uint8_t / an unsigned 8-bit integer?
#endif
{
	PACKET_LOGIN = ID_USER_PACKET_ENUM,
	PACKET_MESSAGE,
	PACKET_START_GAME,
	PACKET_ADD_PLAYER,
	PACKET_REMOVE_ENTITY,
	PACKET_MOVE_PLAYER,
	PACKET_PLAYER_ACTION,
	PACKET_PLAYER_COMMAND,
	PACKET_LEVEL_EVENT,
	PACKET_ENTITY_EVENT,
	PACKET_GAME_EVENT,
	PACKET_TILE_UPDATE,
	PACKET_REQUEST_CHUNK,
	PACKET_BLOCK_REGION_UPDATE,
	PACKET_LEVEL_DATA = 200,
	PACKET_LOGIN_STATUS,
	PACKET_READY,
	PACKET_SET_TIME,
	PACKET_ADD_MOB,
	PACKET_ADD_ENTITY,
	PACKET_REMOVE_PLAYER,
	PACKET_ADD_ITEM_ENTITY,
	PACKET_TAKE_ITEM_ENTITY,
	PACKET_ADD_PAINTING,
	PACKET_MOVE_ENTITY,
	PACKET_MOVE_ENTITY_ROT,
	PACKET_MOVE_ENTITY_POS_ROT,
	PACKET_TELEPORT_ENTITY,
	PACKET_EXPLODE,
	PACKET_INTERACTION,
	PACKET_INTERACT,
	PACKET_USE_ITEM,
	PACKET_SET_ENTITY_DATA,
	PACKET_SET_ENTITY_MOTION,
	PACKET_SET_EQUIPPED_ITEM,
	PACKET_SET_CARRIED_ITEM,
	PACKET_SET_HEALTH,
	PACKET_ANIMATE,
	PACKET_PLAYER_CHANGE_DIMENSION,
	PACKET_SIGN_UPDATE,
	PACKET_CONTAINER_SET_CONTENT,
	PACKET_CONTAINER_SET_DATA,
	PACKET_CONTAINER_SET_SLOT,
	PACKET_CONTAINER_CLICK,
	PACKET_CONTAINER_ACK,
	PACKET_CONTAINER_CLOSE,
	PACKET_CONTAINER_OPEN
};

enum eAddEntityType
{
	ADD_BOAT = 1,
	ADD_MINECART = 10,
	ADD_CHEST_MINECART,
	ADD_FURNACE_MINECART,
	ADD_PRIMED_TNT = 50,
	ADD_ARROW = 60,
	ADD_SNOWBALL,
	ADD_THROWN_EGG,
	ADD_FIREBALL,
	ADD_FALLING_SAND = 70,
	ADD_FALLING_GRAVEL,
	ADD_FISHING_HOOK = 90,
	//why not?
	ADD_LIGHTNING_BOLT
};

class Packet
{
public:
	virtual ~Packet() {}
	virtual void write(RakNet::BitStream*) = 0;
	virtual void read(RakNet::BitStream*) = 0;
	virtual void handle(const RakNet::RakNetGUID&, NetEventCallback*) = 0;
};

class LoginPacket : public Packet
{
public:
	LoginPacket() {}
	LoginPacket(const std::string& uname)
	{
		m_str = RakNet::RakString(uname.c_str());
		m_clientNetworkVersion = 2;
		m_clientNetworkVersion2 = 2;
	}

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	RakNet::RakString m_str;
	int m_clientNetworkVersion;
	int m_clientNetworkVersion2;
};

class LoginStatusPacket : public Packet
{
public:
	enum LoginStatus
	{
		STATUS_SUCCESS,
		STATUS_CLIENT_OUTDATED,
		STATUS_SERVER_OUTDATED
	};

public:
	LoginStatusPacket(LoginStatus loginStatus = STATUS_SUCCESS)
	{
		m_loginStatus = loginStatus;
	}

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	LoginStatus m_loginStatus;
};

class ReadyPacket : public Packet
{
public:
	ReadyPacket(int ready = 0)
	{
		m_ready = ready;
	}

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	uint8_t m_ready;
};

class MessagePacket : public Packet
{
public:
	MessagePacket() {}
	MessagePacket(const std::string& msg)
	{
		m_str = msg.c_str();
	}

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	RakNet::RakString m_str;
};

class SetTimePacket : public Packet
{
public:
	SetTimePacket(int64_t time = 0)
	{
		m_time = time;
	}

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int64_t m_time;
};

class StartGamePacket : public Packet
{
public:
	StartGamePacket()
	{
		m_gameType = GAME_TYPES_MAX;
		m_serverVersion = 0;
		m_time = 0;
	}
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int64_t m_seed;
	int m_levelVersion;
	int m_dimension;
	GameType m_gameType;
	int m_entityId;
	Vec3 m_pos;
	uint16_t m_health;
	int m_serverVersion;
	int64_t m_time;
};

class AddPlayerPacket : public Packet
{
public:
	AddPlayerPacket() {}
	AddPlayerPacket(const std::shared_ptr<Player>& player);
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	RakNet::RakNetGUID m_guid;
	RakNet::RakString m_name;
	int m_id;
	Vec3i m_pos;
	int8_t m_rotX, m_rotY;
	uint16_t m_itemID;
};

class AddMobPacket : public Packet
{
public:
	AddMobPacket() {}
	AddMobPacket(const std::shared_ptr<Mob>& mob);
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int m_id;
	int m_type;
	Vec3i m_pos;
	int8_t m_rotX, m_rotY;
	SynchedEntityData m_entityData;
	std::vector<std::unique_ptr<SynchedEntityData::DataItem>> m_unpack;
};

class AddEntityPacket : public Packet
{
public:
	AddEntityPacket() {}
	AddEntityPacket(const std::shared_ptr<Entity>& ent, int id, int data = 0)
	{
		m_id = ent->m_EntityID;
		m_pos = ent->m_pos * 32;
		m_addId = id;
		m_data = data;
		if (data > 0)
			m_vel = PacketUtil::packMotion(ent->m_vel);
	}

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int m_id;
	int m_addId;
	int m_type;
	Vec3i m_pos;
	Vec3i m_vel;
	int m_data;
};

class AddItemEntityPacket : public Packet
{
public:
	AddItemEntityPacket() {}
	AddItemEntityPacket(const std::shared_ptr<ItemEntity>& item) : m_id(item->m_EntityID), m_pos(PacketUtil::packPos(item->m_pos)), m_vel(item->m_vel * 128), m_itemID(item->m_itemInstance->m_itemID), m_count(item->m_itemInstance->m_count), m_auxValue(item->m_itemInstance->getAuxValue()) {}
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int m_id;
	Vec3i m_pos;
	Vec3i m_vel;
	uint16_t m_itemID;
	//Why not unsigned? Well, there are glitches with negative item counts, so there won't be desync at least
	int8_t m_count;
	uint16_t m_auxValue;
};

class TakeItemEntityPacket : public Packet
{
public:
	TakeItemEntityPacket() {}
	TakeItemEntityPacket(uint16_t itemID, int id) : m_itemID(itemID), m_id(id) {}
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	uint16_t m_itemID;
	int m_id;
};

class AddPaintingPacket : public Packet
{
public:
	AddPaintingPacket() {}
	AddPaintingPacket(const std::shared_ptr<Painting>& painting) : m_id(painting->m_EntityID), m_pos(painting->m_tilePos), m_dir(painting->m_dir), m_motive(painting->getPaintingType().m_name.c_str()) {}
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int m_id;
	Vec3i m_pos;
	int m_dir;
	RakNet::RakString m_motive;
};

class RemoveEntityPacket : public Packet
{
public:
	RemoveEntityPacket() {}
	RemoveEntityPacket(int id) { m_id = id; }

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int m_id;
};

class MovePlayerPacket : public Packet
{
public:
	MovePlayerPacket() {}
	MovePlayerPacket(int id, const Vec3& pos, const Vec2& rot, const Vec3& vel = Vec3::ZERO): m_id(id), m_pos(pos), m_rot(rot), m_vel(vel) {}
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int m_id;
	Vec3 m_pos;
	Vec2 m_rot;
	Vec3 m_vel;
};

class MoveEntityPacket : public Packet
{
public:
	MoveEntityPacket() : m_bHasRot(false) {}
	MoveEntityPacket(int id) : m_id(id), m_bHasRot(false) {}
	virtual void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	virtual void write(RakNet::BitStream*) override;
	virtual void read(RakNet::BitStream*) override;
public:
	int m_id;
	Vec3i m_pos;
	int8_t m_rotY, m_rotX;
	bool m_bHasRot;
};

class MoveEntityPacketPos : public MoveEntityPacket
{
public:
	MoveEntityPacketPos()
	{
	}

	MoveEntityPacketPos(int id, const Vec3i& pos) : MoveEntityPacket(id)
	{
		m_pos = pos;
	}

	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
};

class MoveEntityPacketRot : public MoveEntityPacket
{
public:
	MoveEntityPacketRot()
	{
		m_bHasRot = true;
	}

	MoveEntityPacketRot(int id, int8_t rotY, int8_t rotX) : MoveEntityPacket(id)
	{
		m_bHasRot = true;
		m_rotY = rotY;
		m_rotX = rotX;
	}

	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
};

class MoveEntityPacketPosRot : public MoveEntityPacket
{
public:
	MoveEntityPacketPosRot()
	{
		m_bHasRot = true;
	}

	MoveEntityPacketPosRot(int id, const Vec3i& pos, int8_t rotY, int8_t rotX) : MoveEntityPacket(id)
	{
		m_bHasRot = true;
		m_pos = pos;
		m_rotY = rotY;
		m_rotX = rotX;
	}

	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
};

class TeleportEntityPacket : public Packet
{
public:
	TeleportEntityPacket() {}
	TeleportEntityPacket(const std::shared_ptr<Entity>& ent) : m_id(ent->m_EntityID), m_pos(PacketUtil::packPos(ent->m_pos)), m_rotY(PacketUtil::packRot(ent->m_rot.y)), m_rotX(PacketUtil::packRot(ent->m_rot.x)) {}
	TeleportEntityPacket(int id, const Vec3i& pos, int8_t rotY, int8_t rotX) : m_id(id), m_pos(pos), m_rotY(rotY), m_rotX(rotX) {}
	virtual void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	virtual void write(RakNet::BitStream*) override;
	virtual void read(RakNet::BitStream*) override;
public:
	int m_id;
	Vec3i m_pos;
	int8_t m_rotY, m_rotX;
};

class SetEntityMotionPacket : public Packet
{
public:
	SetEntityMotionPacket() {}
	SetEntityMotionPacket(const std::shared_ptr<Entity>& ent) : m_id(ent->m_EntityID), m_vel(PacketUtil::packMotion(ent->m_vel)) {}
	SetEntityMotionPacket(int id, const Vec3& vel) : m_id(id), m_vel(PacketUtil::packMotion(vel)) {}
	virtual void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	virtual void write(RakNet::BitStream*) override;
	virtual void read(RakNet::BitStream*) override;
public:
	int m_id;
	Vec3i m_vel;
};

class SetEntityDataPacket : public Packet
{
public:
	SetEntityDataPacket() {}
	SetEntityDataPacket(int id, SynchedEntityData& data) : m_id(id), m_packedItems(data.packDirty())
	{
	}
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int m_id;
	std::vector<SynchedEntityData::DataItem*> m_packedItems;
	std::vector<std::unique_ptr<SynchedEntityData::DataItem>> m_unpack;
};

class UseItemPacket : public Packet
{
public:
	UseItemPacket() {}
	UseItemPacket(const TilePos& pos, int face, const std::shared_ptr<ItemInstance>& item) : m_pos(pos), m_face(face), m_item(item)
	{
	}

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	TilePos m_pos;
	int m_face;
	std::shared_ptr<ItemInstance> m_item;
};

class InteractPacket : public Packet
{
public:
	InteractPacket() {}
	InteractPacket(int source, int target, uint8_t action) : m_source(source), m_target(target), m_action(action)
	{
	}

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int m_source;
	int m_target;
	uint8_t m_action;
};

class InteractionPacket : public Packet
{
public:
	InteractionPacket() {}
	InteractionPacket(int id, uint8_t type, const Vec3i& pos) : m_id(id), m_pos(pos), m_type(type)
	{
	}

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int m_id;
	Vec3i m_pos;
	uint8_t m_type;
};

class AnimatePacket : public Packet
{
public:
	AnimatePacket() {}
	AnimatePacket(int id, uint8_t action) : m_id(id), m_action(action)
	{
	}

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int m_id;
	uint8_t m_action;
};

class PlayerActionPacket : public Packet
{
public:
	PlayerActionPacket() {}
	PlayerActionPacket(int action, const TilePos& pos, Facing::Name face) : m_action(action), m_pos(pos), m_face(face)
	{
	}

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int m_action;
	TilePos m_pos;
	uint8_t m_face;
};

class LevelEventPacket : public Packet
{
public:
	LevelEventPacket() {}
	LevelEventPacket(int type, const TilePos& pos, int data) : m_type(type), m_pos(pos), m_data(data) {}

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int m_type;
	TilePos m_pos;
	int m_data;
};

class EntityEventPacket : public Packet
{
public:
	EntityEventPacket() {}
	EntityEventPacket(int id, int8_t event) : m_id(id), m_event(event) {}

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int m_id;
	int8_t m_event;
};

class GameEventPacket : public Packet
{
public:
	GameEventPacket() {}
	GameEventPacket(int8_t event) : m_event(event) {}

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int8_t m_event;
};

class SetHealthPacket : public Packet
{
public:
	SetHealthPacket() {}
	SetHealthPacket(int16_t health) : m_health(health) {}

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int16_t m_health;
};

class PlayerChangeDimensionPacket : public Packet
{
public:
	PlayerChangeDimensionPacket() {}
	PlayerChangeDimensionPacket(int8_t dim) : m_dim(dim) {}

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int8_t m_dim;
};

class TileUpdatePacket : public Packet
{
public:
	TileUpdatePacket() {}
	TileUpdatePacket(const TilePos& pos, Level* level);
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	TilePos m_pos;
	TileID m_tile;
	uint8_t m_data;
};

class RequestChunkPacket : public Packet
{
public:
	RequestChunkPacket() {}
	RequestChunkPacket(const ChunkPos& pos) { m_chunkPos = pos; }
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	ChunkPos m_chunkPos;
};

class PlayerCommandPacket : public Packet
{
public:
	PlayerCommandPacket() {}
	PlayerCommandPacket(int id, int action) : m_id(id), m_action(action) {}

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int m_id;
	int m_action;
};

class BlockRegionUpdatePacket : public Packet
{
public:
	BlockRegionUpdatePacket() {}
	BlockRegionUpdatePacket(Level* level, const TilePos& minPos, int xs, int ys, int zs);
	BlockRegionUpdatePacket(LevelChunk* c) : BlockRegionUpdatePacket(c->m_pLevel, c->m_chunkPos, 16, 128, 16)
	{
	}
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	TilePos m_pos;
	int m_xs, m_ys, m_zs, m_size;
	RakNet::BitStream m_data;
};

class ExplodePacket : public Packet
{
public:
	ExplodePacket() {}
	ExplodePacket(const Vec3& pos, float range, const std::unordered_set<TilePos>& toBlow) : m_pos(pos), m_range(range), m_toBlow(toBlow) {}

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	Vec3 m_pos;
	float m_range;
	std::unordered_set<TilePos> m_toBlow;
};

class LevelDataPacket : public Packet
{
public:
	LevelDataPacket() {}
	LevelDataPacket(Level* level) : m_pLevel(level) {}
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	RakNet::BitStream m_data;
	Level* m_pLevel;
};

class SetEquippedItemPacket : public Packet
{
public:
	SetEquippedItemPacket() {}
	SetEquippedItemPacket(int id, int slot, const std::shared_ptr<ItemInstance>& item): m_id(id), m_slot(slot), m_itemID(item ? item->m_itemID : -1), m_auxValue(item ? item->getAuxValue() : 0) {}
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int m_id;
	uint16_t m_slot;
	int16_t m_itemID;
	int16_t m_auxValue;
};


class SetCarriedItemPacket : public Packet
{
public:
	SetCarriedItemPacket() {}
	SetCarriedItemPacket(int slot) : m_slot(slot) {}
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	uint16_t m_slot;
};

class ContainerSetContentPacket : public Packet
{
public:
	ContainerSetContentPacket() {}
	ContainerSetContentPacket(int containerId, const std::vector<std::shared_ptr<ItemInstance>>& items) : m_containerId(containerId), m_items(items.size())
	{
		for (int i = 0; i < items.size(); i++)
		{
			const std::shared_ptr<ItemInstance> item = items[i];
			m_items[i] = item ? item->copy() : nullptr;
		}
	}
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int8_t m_containerId;
	std::vector<std::shared_ptr<ItemInstance>> m_items;
};

class ContainerSetSlotPacket : public Packet
{
public:
	ContainerSetSlotPacket() {}
	ContainerSetSlotPacket(int8_t containerId, int16_t slot, const std::shared_ptr<ItemInstance>& item) : m_containerId(containerId), m_slot(slot), m_item(item ? item->copy() : nullptr)
	{
	}

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int8_t m_containerId;
	int16_t m_slot;
	std::shared_ptr<ItemInstance> m_item;
};

class ContainerClickPacket : public Packet
{
public:
	ContainerClickPacket() {}
	ContainerClickPacket(int8_t containerId, int16_t slot, int8_t button, bool quickMove, const std::shared_ptr<ItemInstance>& item, int16_t uid) : m_containerId(containerId), m_slot(slot), m_button(button), m_uid(uid), m_bQuickMove(quickMove), m_item(item)
	{
	}

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int8_t m_containerId;
	int16_t m_slot;
	int8_t m_button;
	int16_t m_uid;
	bool m_bQuickMove;
	std::shared_ptr<ItemInstance> m_item;
};

class ContainerAckPacket : public Packet
{
public:
	ContainerAckPacket() {}
	ContainerAckPacket(int8_t containerId, int16_t uid, bool accepted) : m_containerId(containerId), m_uid(uid), m_bAccepted(accepted)
	{
	}

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int8_t m_containerId;
	int16_t m_uid;
	bool m_bAccepted;
};

class ContainerSetDataPacket : public Packet
{
public:
	ContainerSetDataPacket() {}
	ContainerSetDataPacket(int8_t containerId, int16_t id, int16_t value) : m_containerId(containerId), m_id(id), m_value(value)
	{
	}

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int8_t m_containerId;
	int16_t m_id;
	int16_t m_value;
};

class ContainerOpenPacket : public Packet
{
public:
	ContainerOpenPacket() {}
	ContainerOpenPacket(int8_t containerId, int8_t type, const std::string& title, int8_t size) : m_containerId(containerId), m_type(type), m_title(title.c_str()), m_size(size) {}
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int8_t m_containerId;
	int8_t m_type;
	RakNet::RakString m_title;
	int8_t m_size;
};

class ContainerClosePacket : public Packet
{
public:
	ContainerClosePacket() {}
	ContainerClosePacket(int8_t containerId) : m_containerId(containerId) {}
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int8_t m_containerId;
};

class SignUpdatePacket : public Packet
{
public:
	SignUpdatePacket() {};
	SignUpdatePacket(const TilePos& pos, const std::array<std::string, 4>& messages) : m_pos(pos) 
	{
		for (int i = 0; i < messages.size(); ++i)
		{
			m_messages[i] = messages[i].c_str();
		}
	}
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	TilePos m_pos;
	std::array<RakNet::RakString, 4> m_messages;
};
