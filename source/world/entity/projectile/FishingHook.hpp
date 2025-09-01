#pragma once

#include "../Entity.hpp"

class FishingHook : public Entity
{
public:
	FishingHook(Level*);
	FishingHook(Level*, const Vec3& pos);
	FishingHook(Level*, std::shared_ptr<Player>);

	void lerpTo(const Vec3& pos, const Vec2& rot, int steps) override;
	void lerpMotion(const Vec3& vel) override;

	virtual bool shouldRenderAtSqrDistance(real distSqr) const override;

	void shoot(const Vec3& vel, float pow, float randomPow);

	void tick() override;

	void addAdditionalSaveData(CompoundIO tag) override;

	void readAdditionalSaveData(CompoundIO tag) override;
	 
	float getShadowHeightOffs() const override;

	int retrieve();

public:
	std::shared_ptr<Player> m_owner = nullptr;
	std::shared_ptr<Entity> m_hookedIn = nullptr;
	int m_shakeTime = 0;

private:
	bool m_bInGround = false;
	int m_lastTile = 0;
	int m_life = 0;
	int m_flightTime = 0;
	int m_nibble = 0;
	TilePos m_tilePos = TilePos(-1, -1, -1);
	int m_lSteps = 0;
	Vec3 m_lPos;
	Vec3 m_lPosD;
	Vec2 m_lRot;
};

