#pragma once

#include "Entity.hpp"

class LightningBolt : public Entity
{

public:
	LightningBolt(Level* level);
	LightningBolt(Level* level, const Vec3& pos);

	void tick() override;
	virtual bool shouldRender(Vec3& camPos) const override;

public:
	int64_t m_seed;

private:
	int m_life;
	int m_flashes;
};

