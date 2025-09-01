/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Player.hpp"
#include "world/level/Level.hpp"
#include "world/tile/BedTile.hpp"
#include "projectile/Arrow.hpp"
#include "Wolf.hpp"

Player::Player(Level* pLevel, GameType playerGameType) : Mob(pLevel), m_portalTime(0), m_oPortalTime(0)
{
	m_pType = EntityType::player;
	m_pInventory = nullptr;
	m_userType = 0;
	m_score = 0;
    m_oBob = 0.0f;
    m_bob = 0.0f;
	m_name = "";
	m_dimension = 0;
	m_bHasRespawnPos = false;
	m_bHasBedSleepPos = false;
	m_destroyingBlock = false;
	m_dmgSpill = 0;
	m_sleepTimer = 0;
	m_bSleeping = false;
	m_changingDimensionDelay = 20;
	m_bIsInsidePortal = false;

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
		if (pEnt->getCategory().contains(EntityCategories::MONSTER) || pEnt->getCategory().contains(EntityCategories::ABSTRACT_ARROW))
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

	if (damage == 0)
		return false;

	Entity* wolfAttack = nullptr;

	if (pEnt && pEnt->getType() == EntityType::arrow && ((Arrow*)pEnt)->m_owner)
		wolfAttack = ((Arrow*)pEnt)->m_owner.get();

	if (wolfAttack && wolfAttack->getCategory().contains(EntityCategories::MOB))
		alertWolves((Mob*) wolfAttack, false);

    return Mob::hurt(pEnt, damage);
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

void Player::alertWolves(Mob* mob, bool mandatory)
{
	if (mob->getType() != EntityType::creeper && mob->getType() != EntityType::ghast)
	{
		if (mob->getType() == EntityType::wolf)
		{
			Wolf* wolf = (Wolf*)mob;
			if (wolf->isTamed() && m_name == wolf->getOwner())
				return;
		}

		if (mob != this)
		{
			AABB hit(m_pos, m_pos + 1);
			hit.grow(16.0, 4.0, 16.0);
			auto entities = m_pLevel->getEntitiesOfType(EntityType::wolf, hit);


			for (auto& ent : entities)
			{
				Wolf* wolf = (Wolf*)ent.get();

				if (wolf->isTamed() && !wolf->getAttackTarget() && wolf->getOwner() == m_name && !(mandatory && wolf->isOrderedToSit()))
				{
					wolf->setOrderedToSit(false);
					wolf->setAttackTarget(mob->shared_from_this());
				}
			}
		}
	}
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
		drop(std::make_shared<ItemInstance>(Item::apple), true);
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
	Mob::aiStep();
	float velLen = Mth::sqrt(m_vel.x * m_vel.x + m_vel.z * m_vel.z);
	float velYAtan = Mth::atan(m_vel.y * -0.2f), x1 = 0.0f;

	if (velLen > 0.1f)
		velLen = 0.1f;

	if (!m_bOnGround)
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

		EntityVector ents = m_pLevel->getEntities(this, scanAABB);

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
	if (isSleeping()) {
		++m_sleepTimer;
		if (m_sleepTimer > 100)
			m_sleepTimer = 100;

		if (!m_pLevel->m_bIsOnline)
		{
			if (!isInBed()) {
				wake(true, true, false);
			}
			else if (m_pLevel->isDay()) {
				wake(false, true, true);
			}
		}
	}
	else if (m_sleepTimer > 0)
	{
		++m_sleepTimer;
		if (m_sleepTimer >= 110)
			m_sleepTimer = 0;
	}

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
		if (m_vel.y < 0.0)
			++atkDmg;
		pEnt->hurt(this, atkDmg);
		auto var3 = getSelectedItem();
		if (var3 && pEnt->getCategory().contains(EntityCategories::MOB))
		{
			var3->hurtEnemy((Mob*)pEnt);
			if (var3->m_count <= 0) {
				var3->snap(this);
				removeSelectedItem();
			}
		}

		if (pEnt->getCategory().contains(EntityCategories::MOB))
		{
			if (pEnt->isAlive())
				alertWolves((Mob*)pEnt, true);
		}
	}
}

bool Player::canDestroy(const Tile* pTile) const
{
	if (pTile->m_pMaterial->isMineable())
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
	Mob::remove();
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

real Player::getRidingHeight()
{
	return m_heightOffset - 0.5f;
}

void Player::handleInsidePortal()
{
	if (m_changingDimensionDelay > 0)
		m_changingDimensionDelay = 10;
	else
		m_bIsInsidePortal = true;
}

void Player::displayClientMessage(const std::string& msg)
{

}

void Player::drop(std::shared_ptr<ItemInstance> itemInstance, bool b)
{

	auto pItemEntity = std::make_shared<ItemEntity>(m_pLevel, Vec3(m_pos.x, m_pos.y - 0.3f + getHeadHeight(), m_pos.z), itemInstance);
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

	if (!m_bOnGround) {
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
	Mob::rideTick();
	m_oBob = m_bob;
	m_bob = 0.0F;
}

void Player::setDefaultHeadHeight()
{
	m_heightOffset = 1.62f;
}

int Player::getItemIcon(ItemInstance* instance)
{
	return instance->m_itemID == Item::fishingRod->m_itemID && m_fishing ? Item::stick->getIcon(instance) : Mob::getItemIcon(instance);
}

void Player::setRespawnPos(const TilePos& pos)
{
	m_bHasRespawnPos = true;
	m_respawnPos = pos;
}

void Player::setBedSleepPos(const TilePos& pos)
{
	m_bHasBedSleepPos = true;
	m_bedSleepPos = pos;
}

void Player::updateSleepingPos(int data)
{
	m_sleepingPos.x = 0.0F;
	m_sleepingPos.z = 0.0F;
	switch (data) {
	case 0:
		m_sleepingPos.z = -1.8F;
		break;
	case 1:
		m_sleepingPos.x = 1.8F;
		break;
	case 2:
		m_sleepingPos.z = 1.8F;
		break;
	case 3:
		m_sleepingPos.x = -1.8F;
	}
}

bool Player::isImmobile() const
{
	return m_health <= 0 || isSleeping();
}

TilePos Player::checkRespawnPos(Level* level, const TilePos& pos)
{
	ChunkPos ch(pos);
	level->getChunkSource()->getChunk(ch - 1);
	level->getChunkSource()->getChunk(ChunkPos(ch.x + 1, ch.z - 1));
	level->getChunkSource()->getChunk(ChunkPos(ch.x - 1, ch.z + 1));
	level->getChunkSource()->getChunk(ch + 1);
	if (level->getTile(pos) != Tile::bed->m_ID)
		return pos;
	else
		return BedTile::getRespawnTilePos(level, pos, 0);
}

float Player::getBedSleepRot()
{
	if (m_bHasBedSleepPos) {
		int var1 = m_pLevel->getData(m_bedSleepPos);
		int var2 = BedTile::getDirectionFromData(var1);
		switch (var2) {
		case 0:
			return 90.0F;
		case 1:
			return 0.0F;
		case 2:
			return 270.0F;
		case 3:
			return 180.0F;
		}
	}

	return 0.0F;
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

void Player::openTrap(std::shared_ptr<DispenserTileEntity> tileEntity)
{
}

void Player::openTextEdit(std::shared_ptr<SignTileEntity> tileEntity)
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
	if (!pEnt->interact(this))
	{
		auto var2 = getSelectedItem();
		if (var2 && pEnt->getCategory().contains(EntityCategories::MOB)) {
			var2->interactEnemy((Mob*) pEnt);
			if (var2->m_count <= 0) {
				var2->snap(this);
				m_pInventory->setItem(m_pInventory->m_selected, nullptr);
			}
		}
	}
}

bool Player::isInBed()
{
	return m_pLevel->getTile(m_bedSleepPos) == Tile::bed->m_ID;
}

void Player::wake(bool var1, bool var2, bool var3)
{
	setSize(0.6F, 1.8F);
	setDefaultHeadHeight();
	TilePos checkBedPos = m_bedSleepPos;
	if (m_bHasBedSleepPos && m_pLevel->getTile(checkBedPos) == Tile::bed->m_ID) {
		BedTile::setBedOccupied(m_pLevel, checkBedPos, false);
		checkBedPos = BedTile::getRespawnTilePos(m_pLevel, checkBedPos, 0);
		if (checkBedPos == m_bedSleepPos)
			checkBedPos = checkBedPos.above();

		setPos(Vec3(checkBedPos.x + 0.5F, checkBedPos.y + m_heightOffset + 0.1F, checkBedPos.z + 0.5F));
	}

	m_bSleeping = false;
	if (!m_pLevel->m_bIsOnline && var2)
		m_pLevel->updateSleeping();

	if (var1) {
		m_sleepTimer = 0;
	}
	else {
		m_sleepTimer = 100;
	}

	if (var3) {
		setRespawnPos(m_bedSleepPos);
	}
}

Player::BedSleepingProblem Player::sleep(const TilePos& pos)
{
	if (!m_pLevel->m_bIsOnline)
	{
		if (isSleeping() || !isAlive())
			return Player::BedSleepingProblem::OTHER_PROBLEM;

		if (m_pLevel->m_pDimension->m_bFoggy)
			return Player::BedSleepingProblem::NOT_POSSIBLE_HERE;

		if (m_pLevel->isDay())
			return Player::BedSleepingProblem::NOT_POSSIBLE_NOW;

		if (Mth::abs(m_pos.x - pos.x) > 3.0 || Mth::abs(m_pos.y - pos.y) > 2.0 || Mth::abs(m_pos.z - pos.z) > 3.0)
			return Player::BedSleepingProblem::TOO_FAR_AWAY;
	}

	setSize(0.2F, 0.2F);
	m_heightOffset = 0.2F;
	if (!m_pLevel->isEmptyTile(pos)) {
		int var4 = m_pLevel->getData(pos);
		int var5 = BedTile::getDirectionFromData(var4);
		float var6 = 0.5F;
		float var7 = 0.5F;
		switch (var5) {
		case 0:
			var7 = 0.9F;
			break;
		case 1:
			var6 = 0.1F;
			break;
		case 2:
			var7 = 0.1F;
			break;
		case 3:
			var6 = 0.9F;
		}

		updateSleepingPos(var5);
		setPos(Vec3(pos.x + var6, pos.y + 15.0F / 16.0F, pos.z + var7));
	}
	else
		setPos(Vec3(pos.x + 0.5f, pos.y + 15.0F / 16.0F, pos.z + 0.5f));
	
	m_bSleeping = true;
	m_sleepTimer = 0;
	setBedSleepPos(pos);
	m_vel = Vec3::ZERO;
	if (!m_pLevel->m_bIsOnline)
		m_pLevel->updateSleeping();

	return Player::BedSleepingProblem::OK;
}

std::shared_ptr<ItemInstance> Player::getSelectedItem() const
{
	return m_pInventory->getSelected();
}

std::shared_ptr<ItemInstance> Player::getCarriedItem()
{
	return m_pInventory->getSelected();
}

void Player::addAdditionalSaveData(CompoundIO tag) {
	Mob::addAdditionalSaveData(tag);
	tag->put("Inventory", m_pInventory->save(std::make_shared<ListTag>()));
	tag->putInt("playerGameType", getPlayerGameType());
	tag->putInt("Dimension", m_dimension);
	tag->putBoolean("Sleeping", m_bSleeping);
	tag->putShort("SleepTimer", m_sleepTimer);
	if (m_bHasRespawnPos)
	{
		tag->putInt("SpawnX", m_respawnPos.x);
		tag->putInt("SpawnY", m_respawnPos.y);
		tag->putInt("SpawnZ", m_respawnPos.z);
	}
	if (m_bSleeping)
	{
		setBedSleepPos(m_pos);
		wake(true, true, false);
	}

}

void Player::readAdditionalSaveData(CompoundIO tag) {
	Mob::readAdditionalSaveData(tag);
	m_pInventory->load(tag->getList("Inventory"));
	m_dimension = tag->getInt("Dimension");
	m_sleepTimer = tag->getInt("SleepTimer");
	if (tag->contains("SpawnX") && tag->contains("SpawnY") && tag->contains("SpawnZ"))
		setRespawnPos(TilePos(tag->getInt("SpawnX"), tag->getInt("SpawnY"), tag->getInt("SpawnZ")));
	setPlayerGameType((GameType)tag->getInt("playerGameType"));
}