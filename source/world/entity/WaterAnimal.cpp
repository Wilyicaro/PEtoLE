#include "WaterAnimal.hpp"
#include "world/entity/WaterAnimal.hpp"

WaterAnimal::WaterAnimal(Level* pLevel) : PathfinderMob(pLevel)
{
}

bool WaterAnimal::isWaterMob() const
{
    return true;
}

bool WaterAnimal::canSpawn()
{
    return m_pLevel->isUnobstructed(&m_hitbox);
}

int WaterAnimal::getAmbientSoundInterval() const
{
    return 120;
}
