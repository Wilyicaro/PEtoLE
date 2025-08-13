#include "Skeleton.hpp"

Skeleton::Skeleton(Level* pLevel) : Monster(pLevel)
{
	m_pType = EntityType::skeleton;
	m_texture = "mob/skeleton.png";
}

void Skeleton::aiStep()
{
	if (m_pLevel->isDay())
	{
		float brightness = getBrightness(1.0f);
		if (brightness > 0.5f
			&& m_pLevel->canSeeSky(this->m_pos)
			&& m_random.nextFloat() * 30.0f < (brightness - 0.4f) * 2.0f)
		{
			this->m_fireTicks = 300;
		}
	}

	Monster::aiStep();
}

void Skeleton::checkHurtTarget(Entity* ent, float f)
{
	if (f < 10.0f)
	{
		Vec3 vel(ent->m_pos.x - m_pos.x, 0, ent->m_pos.z - m_pos.z);

		if (m_attackTime == 0)
		{
			std::shared_ptr<Arrow> arrow = std::make_shared<Arrow>(m_pLevel, std::dynamic_pointer_cast<Mob>(shared_from_this()));
			arrow->m_pos.y += 1;
			float var8 = ent->m_pos.y - 0.2f - arrow->m_pos.y;
			float var10 = vel.length() * 0.2f;
			m_pLevel->playSound(this, "random.bow", 1.0f, 1.0f / (m_random.nextFloat() * 0.4f + 0.8f));
			m_pLevel->addEntity(arrow);
			vel.y = var8 + var10;
			arrow->shoot(vel, 0.6f, 12.0f);
			m_attackTime = 30;
		}

		m_rot.y = (Mth::atan2(vel.z, vel.x) * 180.0f / M_PI) - 90.0f;
		m_bHoldGround = true;
	}

}

void Skeleton::dropDeathLoot()
{
	for (int i = 0; i < m_random.nextInt(3); ++i)
		spawnAtLocation(Item::arrow->m_itemID, 1);

	for (int i = 0; i < m_random.nextInt(3); ++i)
		spawnAtLocation(Item::bone->m_itemID, 1);
}

std::shared_ptr<ItemInstance> Skeleton::bow = nullptr;
std::shared_ptr<ItemInstance> Skeleton::getCarriedItem()
{
	if (!bow)
		bow = std::make_shared<ItemInstance>(Item::bow, 1);

	return bow;
}