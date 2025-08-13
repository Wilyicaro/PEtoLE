#include "Sheep.hpp"
//#include "common/Utils.hpp"
#include "world/level/Level.hpp"

#define DATA_WOOL_ID (16)

const float Sheep::COLOR[][3] = {
	{1.00f, 1.00f, 1.00f},
	{0.95f, 0.70f, 0.20f},
	{0.90f, 0.50f, 0.85f},
	{0.60f, 0.70f, 0.95f},
	{0.90f, 0.90f, 0.20f},
	{0.50f, 0.80f, 0.10f},
	{0.95f, 0.70f, 0.80f},
	{0.30f, 0.30f, 0.30f},
	{0.60f, 0.60f, 0.60f},
	{0.30f, 0.60f, 0.70f}, 
	{0.70f, 0.40f, 0.90f},
	{0.20f, 0.40f, 0.80f},
	{0.50f, 0.40f, 0.30f},
	{0.40f, 0.50f, 0.20f},
	{0.80f, 0.30f, 0.30f}, 
	{0.10f, 0.10f, 0.10f}
};

Sheep::Sheep(Level* pLevel) : Animal(pLevel)
{
	m_pType = EntityType::sheep;
	m_texture = "mob/sheep.png";
	setSize(0.9f, 1.3f);
}

void Sheep::defineSynchedData()
{
	m_entityData.define<int8_t>(DATA_WOOL_ID, 0);
}

bool Sheep::hurt(Entity* pEnt, int damage)
{
	if (!m_pLevel->m_bIsOnline && !isSheared() && (pEnt && pEnt->getCategory().contains(EntityCategories::MOB)))
	{
		setSheared(true);
		int var3 = 1 + m_random.nextInt(3);

		for (int i = 0; i < var3; i++)
		{
			auto item = spawnAtLocation(std::make_shared<ItemInstance>(TILE_CLOTH, 1, getColor()), 1.0f);
			item->m_vel.y += m_random.nextFloat() * 0.05f;
			item->m_vel.x += (m_random.nextFloat() - m_random.nextFloat()) * 0.1f;
			item->m_vel.z += (m_random.nextFloat() - m_random.nextFloat()) * 0.1f;
		}
	}

	return Mob::hurt(pEnt, damage);
}

void Sheep::addAdditionalSaveData(std::shared_ptr<CompoundTag> tag)
{
	tag->putBoolean("Sheared", isSheared());
	tag->putByte("Color", getColor());
}

void Sheep::readAdditionalSaveData(std::shared_ptr<CompoundTag> tag)
{
	setSheared(tag->getBoolean("Sheared"));
	setColor(tag->getByte("Color"));
}

int Sheep::getColor() const 
{
	return m_entityData.get<int8_t>(DATA_WOOL_ID) & 15;
}

void Sheep::setColor(int var1)
{
	int8_t var2 = m_entityData.get<int8_t>(DATA_WOOL_ID);
	m_entityData.set<int8_t>(DATA_WOOL_ID, (var2 & 240 | var1 & 15));
}

bool Sheep::isSheared() const
{
	return (m_entityData.get<int8_t>(DATA_WOOL_ID) & DATA_WOOL_ID) != 0;
}

void Sheep::setSheared(bool var1)
{
	int8_t var2 = m_entityData.get<int8_t>(DATA_WOOL_ID);
	if (var1)
	{
		m_entityData.set<int8_t>(DATA_WOOL_ID, (var2 | DATA_WOOL_ID));
	}
	else
	{
		m_entityData.set<int8_t>(DATA_WOOL_ID, (var2 & -17));
	}

}

int Sheep::getSheepColor(Random& random)
{
	int value = random.nextInt(100);
	if (value < 5)
	{
		return 15;
	}
	else if (value < 10)
	{
		return 7;
	}
	else if (value < 15)
	{
		return 8;
	}
	else if (value < 18)
	{
		return 12;
	}
	else return random.nextInt(500) == 0 ? 6 : 0;

}
