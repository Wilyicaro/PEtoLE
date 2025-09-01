#include "PigZombie.hpp"

PigZombie::PigZombie(Level* pLevel) : Zombie(pLevel)
{
	m_pType = EntityType::pigZombie;
	m_texture = "mob/pigzombie.png";
	m_bFireImmune = true;
}

void PigZombie::tick()
{
	m_runSpeed = m_pAttackTarget ? 0.95F : 0.5F;
	if (m_playAngrySoundIn > 0 && --m_playAngrySoundIn == 0)
		m_pLevel->playSound(this, "mob.zombiepig.zpigangry", getSoundVolume() * 2.0F, ((m_random.nextFloat() - m_random.nextFloat()) * 0.2F + 1.0F) * 1.8F);

	Zombie::tick();
}

bool PigZombie::canSpawn()
{
	return m_pLevel->m_difficulty > 0 && m_pLevel->isUnobstructed(&m_hitbox) && m_pLevel->getCubes(this, m_hitbox)->empty() && !m_pLevel->containsAnyLiquid(m_hitbox);
}

void PigZombie::addAdditionalSaveData(CompoundIO tag)
{
	Zombie::addAdditionalSaveData(tag);
	tag->putShort("Anger", m_angerTime);
}

void PigZombie::readAdditionalSaveData(CompoundIO tag)
{
	Zombie::readAdditionalSaveData(tag);
	m_angerTime = tag->getShort("Anger");
}

std::shared_ptr<Entity> PigZombie::findAttackTarget()
{
	return m_angerTime ? Zombie::findAttackTarget() : nullptr;
}

bool PigZombie::hurt(Entity* ent, int dmg)
{
	if (ent && ent->isPlayer())
	{
		AABB hit = m_hitbox;
		hit.grow(32.0, 32.0, 32.0);
		auto entities = m_pLevel->getEntities(this, hit);

		for (auto& entity : entities)
		{
			if (entity->getType() == EntityType::pigZombie)
			{
				((PigZombie*)entity.get())->alert(ent);
			}
		}

		alert(ent);
	}

	return Zombie::hurt(ent, dmg);
}

std::shared_ptr<ItemInstance> PigZombie::getCarriedItem()
{
	if (!sword) sword = std::make_shared<ItemInstance>(Item::goldSword, 1);
	return sword;
}

void PigZombie::alert(Entity* ent)
{
	m_pAttackTarget = ent->shared_from_this();
	m_angerTime = 400 + m_random.nextInt(400);
	m_playAngrySoundIn = m_random.nextInt(40);
}

std::shared_ptr<ItemInstance> PigZombie::sword = nullptr;
