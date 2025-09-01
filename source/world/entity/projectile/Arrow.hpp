#pragma once

#include "../Entity.hpp"

class Arrow : public Entity
{
public:
	Arrow(Level*);
	Arrow(Level*, const Vec3& pos);
	Arrow(Level*, std::shared_ptr<Mob> mob);

	void shoot(const Vec3& vel, float pow, float randomPow);

	void lerpMotion(const Vec3& vel) override;

	void tick() override;

	void addAdditionalSaveData(CompoundIO tag) override;

	void readAdditionalSaveData(CompoundIO tag) override;

	void playerTouch(Player*) override;

	float getShadowHeightOffs() const override;

public:
	std::shared_ptr<Mob> m_owner = nullptr;
	int m_shakeTime = 0;
	bool m_bFromPlayer = false;

private:
	bool m_bInGround = false;
	int m_lastTile = 0;
	int m_life = 0;
	int m_flightTime = 0;
	TilePos m_tilePos = TilePos(-1, -1, -1);
};

