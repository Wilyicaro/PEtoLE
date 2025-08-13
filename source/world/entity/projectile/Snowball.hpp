#pragma once

#include "../Entity.hpp"

class Snowball : public Entity
{
public:
	Snowball(Level*);
	Snowball(Level*, const Vec3& pos);
	Snowball(Level*, std::shared_ptr<Mob> mob);

	virtual bool shouldRenderAtSqrDistance(real distSqr) const override;

	virtual void shoot(const Vec3& vel, float pow, float randomPow);

	virtual void lerpMotion(const Vec3& vel) override;

	virtual void tick() override;

	virtual void addAdditionalSaveData(std::shared_ptr<CompoundTag> tag) override;

	virtual void readAdditionalSaveData(std::shared_ptr<CompoundTag> tag) override;

	virtual float getShadowHeightOffs() const override;

	virtual void onHit();

public:
	std::shared_ptr<Mob> m_owner = nullptr;
	int m_shakeTime = 0;
	Vec3 m_power;

private:
	bool m_bInGround = false;
	int m_lastTile = 0;
	int m_life = 0;
	int m_flightTime = 0;
	TilePos m_tilePos = TilePos(-1, -1, -1);
};

