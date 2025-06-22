/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Player.hpp"
#include "world/level/Level.hpp"

Player::Player(Level* pLevel, GameType playerGameType) : Mob(pLevel)
{
	m_pEntityType = EntityType::player;
	m_pInventory = nullptr;
	m_userType = 0;
	m_score = 0;
    m_oBob = 0.0f;
    m_bob = 0.0f;
	m_name = "";
	m_dimension = 0;
	m_bHaveRespawnPos = false;
	m_destroyingBlock = false;
	m_dmgSpill = 0;

	m_renderType = RENDER_HUMANOID;

	setPlayerGameType(playerGameType);

	m_pInventory = new Inventory(this);
	
	m_containerMenu = m_inventoryMenu = new InventoryMenu(m_pInventory);

	setDefaultHeadHeight();

	Vec3 pos = m_pLevel->getSharedSpawnPos();

	moveTo(Vec3(pos.x + 0.5f, pos.y + 1.0f, pos.z + 0.5f));

	m_health = 20;

	m_texture = "mob/char.png";

	m_flameTime = 20;
	m_rotOffs = 180.0f;
}

Player::~Player()
{
	delete m_pInventory;
}

void Player::reset()
{
	Mob::reset();

	// TODO what fields to reset???
}

bool Player::hurt(Entity* pEnt, int damage)
{
	if (getAbilities().m_invulnerable)
		return false;

	m_noActionTime = 0;
	if (m_health <= 0)
	{
		return false;
	}

	if (pEnt) {
		auto& type = pEnt->getType();

		if (type.getCategory().contains(EntityCategories::MONSTER) || type.getCategory().contains(EntityCategories::ABSTRACT_ARROW))
		{
			switch (m_pLevel->m_difficulty)
			{
			case 0:
				damage = 0;
				break;
			case 1:
				damage = damage / 3 + 1;
				break;
			case 2:
				// Don't modify damage
				break;
			case 3:
				damage = damage * 3 / 2;
				break;
			default:
				assert(!"Unknown difficulty value");
				break;
			}
		}
	}

    return damage == 0 ? false : Mob::hurt(pEnt, damage);
}

void Player::actuallyHurt(int damage)
{
	int var2 = 25 - m_pInventory->getArmorValue();
	int var3 = damage * var2 + m_dmgSpill;
	m_pInventory->hurtArmor(damage);
	damage = var3 / 25;
	m_dmgSpill = var3 % 25;
	Mob::actuallyHurt(damage);
}

void Player::awardKillScore(Entity* pKilled, int score)
{
	m_score += score;
}

void Player::resetPos()
{
	setDefaultHeadHeight();
	setSize(0.6f, 1.8f);

	Entity::resetPos();

	m_health = 20;
	m_deathTime = 0;
}

void Player::die(Entity* pCulprit)
{
	Mob::die(pCulprit);
	setSize(0.2f, 0.2f);
	setPos(m_pos); // update hitbox
	m_vel.y = 0.1f;

	if (m_name == "Notch")
		drop(new ItemInstance(Item::apple), true);
	m_pInventory->dropAll();

	if (pCulprit)
	{
		m_vel.x = -0.1f * Mth::cos(float((m_hurtDir + m_rot.y) * M_PI / 180.0));
		m_vel.z = -0.1f * Mth::cos(float((m_hurtDir + m_rot.y) * M_PI / 180.0));
	}
	else
	{
		m_vel.x = 0;
		m_vel.z = 0;
	}
}

void Player::aiStep()
{
	if (m_jumpTriggerTime > 0)
		m_jumpTriggerTime--;

    if (m_pLevel->m_difficulty == 0 &&
        m_health < 20 &&
        m_tickCount % 20 * 12 == 0)
    {
       heal(1);
    }

#ifdef ENH_GUI_ITEM_POP
    m_pInventory->tick();
#endif
	m_oBob = m_bob;
	float velLen = Mth::sqrt(m_vel.x * m_vel.x + m_vel.z * m_vel.z);
	float velYAtan = Mth::atan(m_vel.y * -0.2f), x1 = 0.0f;

	if (velLen > 0.1f)
		velLen = 0.1f;

	if (!m_onGround)
	{
		if (m_health > 0)
		{
			velLen = 0;
			x1 = velYAtan * 15.0f;
		}
		else
		{
			velLen = x1 = 0.0f;
		}
	}

	if (m_health <= 0)
		velLen = 0.0f;

	m_bob += (velLen - m_bob) * 0.4f;
	m_tilt += (x1 - m_tilt) * 0.8f;

	if (m_health <= 0)
		return;

	AABB scanAABB = m_hitbox;
	scanAABB.grow(1, 1, 1);

	EntityVector ents = m_pLevel->getEntities(this->shared_from_this(), scanAABB);

	for (EntityVector::iterator it = ents.begin(); it != ents.end(); it++)
	{
		auto& pEnt = *it;
		if (pEnt->m_bRemoved)
			continue;

		touch(pEnt.get());
	}
}

void Player::tick()
{
	Mob::tick();
	if (!m_pLevel->m_bIsOnline && m_containerMenu && !m_containerMenu->stillValid(this)) {
		closeContainer();
	}
}

void Player::updateAi()
{
	if (m_bSwinging)
	{
		m_swingTime++;
		if (m_swingTime >= 8)
		{
			m_swingTime = 0;
			m_bSwinging = false;
		}
	}
	else
	{
		m_swingTime = 0;
	}

	m_attackAnim = m_swingTime / 8.0f;
}

void Player::animateRespawn()
{
	
}

int Player::addResource(int x)
{
	return 0;
}

void Player::animateRespawn(Player*, Level*)
{

}

void Player::attack(Entity* pEnt)
{
	int atkDmg = m_pInventory->getAttackDamage(pEnt);
	if (atkDmg > 0) 
	{
		pEnt->hurt(this, atkDmg);
		auto var3 = getSelectedItem();
		if (var3 && pEnt->getType().getCategory().contains(EntityCategories::MOB)) {
			var3->hurtEnemy((Mob*)pEnt);
			if (var3->m_count <= 0) {
				var3->snap(this);
				removeSelectedItem();
			}
		}
	}
}

bool Player::canDestroy(const Tile* pTile) const
{
	if (pTile->m_pMaterial != Material::stone && pTile->m_pMaterial != Material::metal && pTile->m_pMaterial != Material::snow && pTile->m_pMaterial != Material::topSnow)
		return true;
	auto item = getSelectedItem();
	return item && item->canDestroySpecial(pTile);
}

void Player::closeContainer()
{
	m_containerMenu = m_inventoryMenu;
}

void Player::remove()
{
	m_inventoryMenu->removed(this);
	if (m_containerMenu) {
		m_containerMenu->removed(this);
	}
}

void Player::carriedChanged(std::shared_ptr<ItemInstance> instance)
{
}

void Player::removeSelectedItem()
{
	m_pInventory->setSelectedItem(nullptr);
}

void Player::travel(const Vec2& pos)
{
	if (getAbilities().m_flying)
	{
		real yd = m_vel.y;
		float oldFlyingFriction = m_flyingFriction;
		m_flyingFriction = 0.05f;
		Mob::travel(pos);
		m_flyingFriction = oldFlyingFriction;
		m_vel.y = yd * 0.6;
	}
	else Mob::travel(pos);
}

void Player::displayClientMessage(const std::string& msg)
{

}

void Player::drop(const ItemInstance* pItemInstance, bool b)
{
	if (pItemInstance->isNull())
		return;

	auto pItemEntity = std::make_shared<ItemEntity>(m_pLevel, Vec3(m_pos.x, m_pos.y - 0.3f + getHeadHeight(), m_pos.z), pItemInstance);
	pItemEntity->m_throwTime = 40;

	if (b)
	{
		float throwPower = 0.5f * m_random.nextFloat();
		float throwAngle = m_random.nextFloat();

		pItemEntity->m_vel.x = -(throwPower * Mth::sin(2 * real(M_PI) * throwAngle));
		pItemEntity->m_vel.z =  (throwPower * Mth::cos(2 * real(M_PI) * throwAngle));
		pItemEntity->m_vel.y = 0.2f;
	}
	else
	{
		pItemEntity->m_vel.x = -(Mth::sin(m_rot.y / 180.0f * real(M_PI)) * Mth::cos(m_rot.x / 180.0f * real(M_PI))) * 0.3f;
		pItemEntity->m_vel.z =  (Mth::cos(m_rot.y / 180.0f * real(M_PI)) * Mth::cos(m_rot.x / 180.0f * real(M_PI))) * 0.3f;
		pItemEntity->m_vel.y = 0.1 - Mth::sin(m_rot.x / 180.0f * real(M_PI)) * 0.3f;

		float f1 = m_random.nextFloat();
		float f2 = m_random.nextFloat();

		pItemEntity->m_vel.x += 0.02 * f2 * Mth::cos(2 * real(M_PI) * f1);
		pItemEntity->m_vel.y += 0.1 * (m_random.nextFloat() - m_random.nextFloat());
		pItemEntity->m_vel.z += 0.02 * f2 * Mth::sin(2 * real(M_PI) * f1);
	}

	reallyDrop(pItemEntity);
}

void Player::drop()
{

}

float Player::getDestroySpeed(const Tile* tile) const
{
	float speed = 1.0f;
	auto item = getSelectedItem();
	if (item) speed *= item->getDestroySpeed(tile);
	if (isUnderLiquid(Material::water)) {
		speed /= 5.0F;
	}

	if (!m_onGround) {
		speed /= 5.0F;
	}

	return speed;
}


int Player::getInventorySlot(int x) const
{
	return 0;
}

void Player::prepareCustomTextures()
{

}

void Player::reallyDrop(std::shared_ptr<ItemEntity> pEnt)
{
	m_pLevel->addEntity(pEnt);
}

void Player::respawn()
{

}

void Player::rideTick()
{

}

void Player::setDefaultHeadHeight()
{
	m_heightOffset = 1.62f;
}

void Player::setRespawnPos(const TilePos& pos)
{
	/*if (!pos)
	{
		m_bHaveRespawnPos = false;
		return;
	}*/

	m_bHaveRespawnPos = true;
	m_respawnPos = pos;
}

Abilities& Player::getAbilities()
{
	return m_abilities;
}

void Player::startCrafting(const TilePos& pos)
{

}

void Player::openFurnace(std::shared_ptr<FurnaceTileEntity> tileEntity)
{
}

void Player::openContainer(Container* container)
{
}

void Player::startDestroying()
{
	m_destroyingBlock = true;
}

void Player::stopDestroying()
{
	m_destroyingBlock = false;
}

void Player::take(Entity* pEnt, int x)
{

}

void Player::touch(Entity* pEnt)
{
	pEnt->playerTouch(this);
}

void Player::interact(Entity* pEnt)
{
	pEnt->interact(this);
}

std::shared_ptr<ItemInstance> Player::getSelectedItem() const
{
	return m_pInventory->getSelected();
}

void Player::addAdditionalSaveData(std::shared_ptr<CompoundTag> tag) {
	Mob::addAdditionalSaveData(tag);
	tag->put("Inventory", m_pInventory->save(std::make_shared<ListTag>()));
}

void Player::readAdditionalSaveData(std::shared_ptr<CompoundTag> tag) {
	Mob::readAdditionalSaveData(tag);
	std::shared_ptr <ListTag> var2 = tag->getList("Inventory");
	m_pInventory->load(var2);
}