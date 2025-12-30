#include "ServerEntity.hpp"
#include "Level.hpp"
#include "network/ServerSideNetworkHandler.hpp"
#include "world/entity/Minecart.hpp"
#include "world/entity/projectile/Arrow.hpp"
#include "world/entity/projectile/Fireball.hpp"
#include "world/entity/projectile/FishingHook.hpp"
#include "world/entity/Painting.hpp"
#include "world/entity/FallingTile.hpp"
#include "EntityTracker.hpp"

ServerEntity::ServerEntity() : m_range(0), m_updateInterval(0), m_bTrackDelta(false)
{
}

ServerEntity::ServerEntity(const std::shared_ptr<Entity>& entity, int range, int updateInterval, bool trackDelta) :
	m_entity(entity),
	m_range(range),
	m_updateInterval(updateInterval),
	m_bTrackDelta(trackDelta),
	m_tickCount(0),
	m_bForcedLoading(false),
	m_teleportDelay(0),
	m_bSentChanges(false),
	m_pPos(PacketUtil::packPos(entity->m_pos)),
	m_xRotp(PacketUtil::packRot(entity->m_rot.y)),
	m_yRotp(PacketUtil::packRot(entity->m_rot.x))
{
}

int ServerEntity::hashCode() const
{
	return m_entity->hashCode();
}

bool ServerEntity::operator==(const ServerEntity& other) const
{
	return other.hashCode() == hashCode();
}

void ServerEntity::sendChanges(const std::vector<std::shared_ptr<Player>>& players)
{
	m_bSentChanges = false;
	if (!m_bForcedLoading || m_entity->distanceToSqr(m_pos) > 16.0)
	{
		m_pos = m_entity->m_pos;
		m_bForcedLoading = true;
		m_bSentChanges = true;
		updatePlayers(players);
	}

	++m_teleportDelay;
	if (++m_tickCount % m_updateInterval == 0)
	{
		Vec3i pos = PacketUtil::packPos(m_entity->m_pos);
		int rotY = PacketUtil::packRot(m_entity->m_rot.y);
		int rotX = PacketUtil::packRot(m_entity->m_rot.x);
		Vec3i diff = pos - m_pPos;
		Packet* movePacket = nullptr;
		bool move = Mth::abs(pos.x) >= 8 || Mth::abs(pos.y) >= 8 || Mth::abs(pos.z) >= 8;
		bool rotate = Mth::abs(rotY - m_yRotp) >= 8 || Mth::abs(rotX - m_xRotp) >= 8;
		if (diff.x >= -128 && diff.x < 128 && diff.y >= -128 && diff.y < 128 && diff.z >= -128 && diff.z < 128 && m_teleportDelay <= 400)
		{
			if (move && rotate)
				movePacket = new MoveEntityPacketPosRot(m_entity->m_EntityID, diff, rotY, rotX);
			else if (move)
				movePacket = new MoveEntityPacketPos(m_entity->m_EntityID, diff);
			else if (rotate)
				movePacket = new MoveEntityPacketRot(m_entity->m_EntityID, rotY, rotX);
		}
		else
		{
			m_teleportDelay = 0;
			//m_entity->m_pos = PacketUtil::unpackPos(pos);
			movePacket = new TeleportEntityPacket(m_entity->m_EntityID, pos, rotY, rotX);
		}

		if (m_bTrackDelta)
		{
			Vec3 diff = m_entity->m_vel - m_vel;
			const constexpr real minLengthSqrt = 0.02f;
			real length = diff.lengthSqr();
			if (length > minLengthSqrt * minLengthSqrt || length > 0.0 && m_entity->m_vel == Vec3::ZERO)
			{
				m_vel = m_entity->m_vel;
				broadcast(new SetEntityMotionPacket(m_entity->m_EntityID, m_vel));
			}
		}

		if (movePacket)
			broadcast(movePacket);
		
		SynchedEntityData& data = m_entity->getEntityData();
		if (data.isDirty())
			broadcastAndSend(new SetEntityDataPacket(m_entity->m_EntityID, data));

		if (move)
			m_pPos = pos;

		if (rotate)
		{
			m_yRotp = rotY;
			m_xRotp = rotX;
		}
	}

	if (m_entity->m_bHurt)
	{
		broadcastAndSend(new SetEntityMotionPacket(m_entity));
		m_entity->m_bHurt = false;
	}
}

void ServerEntity::broadcast(Packet* packet, bool free)
{
	for (const std::shared_ptr<Player> p : m_seenBy)
	{
		p->getConnection()->send(p, packet, false);
	}
	if (free)
		delete packet;
}

void ServerEntity::broadcastAndSend(Packet* packet, bool free)
{
	broadcast(packet, false);
	if (m_entity->isPlayer())
	{
		std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(m_entity);
		if (!player->isLocalPlayer())
			m_entity->getConnection()->send(player, packet, false);
	}
	if (free)
		delete packet;
}

void ServerEntity::broadcastRemoved()
{
	broadcast(new RemoveEntityPacket(m_entity->m_EntityID));
}

bool ServerEntity::untrackEntity(const std::shared_ptr<Player>& player)
{
	auto it = m_seenBy.find(player);
	if (it != m_seenBy.end())
	{
		m_seenBy.erase(it);
		return true;
	}
	return false;
}

void ServerEntity::updatePlayer(const std::shared_ptr<Player>& player)
{
	if (player != m_entity)
	{
		real x = player->m_pos.x - real(m_pPos.x / 32);
		real z = player->m_pos.z - real(m_pPos.z / 32);
		if (x >= (-m_range) && x <= m_range && z >= (-m_range) && z <= m_range)
		{
			if (m_seenBy.find(player) == m_seenBy.end())
			{
				m_seenBy.insert(player);
				player->getConnection()->send(player, getAddEntityPacket());
				if (m_bTrackDelta)
					player->getConnection()->send(player, new SetEntityMotionPacket(m_entity->m_EntityID, m_entity->m_vel));
				
				auto slots = m_entity->getEquipmentSlots();
				if (slots)
				{
					for (int slot = 0; slot < slots->size(); ++slot)
						player->getConnection()->send(player, new SetEquippedItemPacket(m_entity->m_EntityID, slot, slots->at(slot)));
				}

				if (m_entity->isPlayer())
				{
					std::shared_ptr<Player> p = std::dynamic_pointer_cast<Player>(m_entity);
					if (p->isSleeping())
						player->getConnection()->send(player, new InteractionPacket(m_entity->m_EntityID, 0, m_entity->m_pos));
				}
			}
		}
		else
		{
			auto it = m_seenBy.find(player);
			if (it != m_seenBy.end())
			{
				m_seenBy.erase(it);
				player->getConnection()->send(player, new RemoveEntityPacket(m_entity->m_EntityID));
			}
		}

	}
}

void ServerEntity::updatePlayers(const std::vector<std::shared_ptr<Player>>& players)
{
	for (const std::shared_ptr<Player>& player : players)
		updatePlayer(player);
}

void ServerEntity::removePairing(const std::shared_ptr<Player>& player)
{
	if (untrackEntity(player))
		player->getConnection()->send(player, new RemoveEntityPacket(m_entity->m_EntityID));
}

Packet* ServerEntity::getAddEntityPacket()
{
	if (m_entity->getType() == EntityType::item)
	{
		std::shared_ptr<ItemEntity> item = std::dynamic_pointer_cast<ItemEntity>(m_entity);
		AddItemEntityPacket* packet = new AddItemEntityPacket(item);
		//idk why this, maybe to not desync the position?
		item->m_pos = PacketUtil::unpackPos(packet->m_pos);
		return packet;
	}
	else if (m_entity->isPlayer())
		return new AddPlayerPacket(std::dynamic_pointer_cast<Player>(m_entity));
	else
	{
		if (m_entity->getType() == EntityType::minecart)
		{
			std::shared_ptr<Minecart> minecart = std::dynamic_pointer_cast<Minecart>(m_entity);
			if (minecart->m_type == Minecart::Type::DEFAULT)
				return new AddEntityPacket(m_entity, ADD_MINECART);
			else if (minecart->m_type == Minecart::Type::CHEST)
				return new AddEntityPacket(m_entity, ADD_CHEST_MINECART);
			else if (minecart->m_type == Minecart::Type::FURNACE)
				return new AddEntityPacket(m_entity, ADD_FURNACE_MINECART);
		}
		else if (m_entity->getType() == EntityType::boat)
			return new AddEntityPacket(m_entity, ADD_BOAT);
		else if (m_entity->getCategory().contains(EntityCategories::MOB))
			return new AddMobPacket(std::dynamic_pointer_cast<Mob>(m_entity));
		else if (m_entity->getType() == EntityType::fishingHook)
		{
			std::shared_ptr<Player>& owner = std::dynamic_pointer_cast<FishingHook>(m_entity)->m_owner;
			return new AddEntityPacket(m_entity, ADD_FISHING_HOOK, owner ? owner->m_EntityID : m_entity->m_EntityID);
		}
		else if (m_entity->getType() == EntityType::arrow)
		{
			std::shared_ptr<Mob>& owner = std::dynamic_pointer_cast<Arrow>(m_entity)->m_owner;
			return new AddEntityPacket(m_entity, ADD_ARROW, owner ? owner->m_EntityID : m_entity->m_EntityID);
		}
		else if (m_entity->getType() == EntityType::snowball)
			return new AddEntityPacket(m_entity, ADD_SNOWBALL);
		else if (m_entity->getType() == EntityType::fireball)
		{
			std::shared_ptr<Fireball> fireball = std::dynamic_pointer_cast<Fireball>(m_entity);
			AddEntityPacket* packet = new AddEntityPacket(m_entity, ADD_FIREBALL, fireball->m_owner->m_EntityID);
			packet->m_vel.x = int(fireball->m_power.x * 8000);
			packet->m_vel.y = int(fireball->m_power.y * 8000);
			packet->m_vel.z = int(fireball->m_power.z * 8000);
			return packet;
		}
		else if (m_entity->getType() == EntityType::thrownEgg)
			return new AddEntityPacket(m_entity, ADD_THROWN_EGG);
		else if (m_entity->getType() == EntityType::primedTnt)
			return new AddEntityPacket(m_entity, ADD_PRIMED_TNT);
		else if (m_entity->getType() == EntityType::lightningBolt)
			return new AddEntityPacket(m_entity, ADD_LIGHTNING_BOLT);
		else if (m_entity->getType() == EntityType::fallingTile)
		{
			std::shared_ptr<FallingTile> fallingTile = std::dynamic_pointer_cast<FallingTile>(m_entity);
			if (fallingTile->m_id == Tile::sand->m_ID)
				return new AddEntityPacket(m_entity, ADD_FALLING_SAND);
			if (fallingTile->m_id == Tile::gravel->m_ID)
				return new AddEntityPacket(m_entity, ADD_FALLING_GRAVEL);
		}
		else if (m_entity->getType() == EntityType::painting)
			return new AddPaintingPacket(std::dynamic_pointer_cast<Painting>(m_entity));
		else
		{
			LOG_W("Don\'t know how to add %s !", m_entity->getEncodeId().c_str());
			return nullptr;
		}
	}
}
