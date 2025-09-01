#pragma once

#include "Entity.hpp"

class Level;
class Player;

class Boat : public Entity
{
public:
	Boat(Level*);
	Boat(Level*, const Vec3& pos);

	const AABB* getCollideBox() const override;
	AABB* getCollideAgainstBox(Entity* ent) const override;
	float getShadowHeightOffs() const override { return 0.0f; }
	bool interact(Player* player) override;
	bool isPickable() const override { return !m_bRemoved; }
	bool isPushable() const override { return true; }
	void tick() override;
	real getRideHeight() override;
	void positionRider() override;
	bool hurt(Entity*, int) override;
	void animateHurt() override;
	void lerpTo(const Vec3& pos, const Vec2& rot, int steps) override;
	void lerpMotion(const Vec3& vel) override;
	void addAdditionalSaveData(CompoundIO tag) override;
	void readAdditionalSaveData(CompoundIO tag) override;

public:
	int m_damage;
	int m_hurtTime;
	int m_hurtDir;

private:
	static const constexpr int EXITS[10][2][3] = {{{0, 0, -1}, {0, 0, 1}}, {{-1, 0, 0}, {1, 0, 0}}, {{-1, -1, 0}, {1, 0, 0}}, {{-1, 0, 0}, {1, -1, 0}}, {{0, 0, -1}, {0, -1, 1}}, {{0, -1, -1}, {0, 0, 1}}, {{0, 0, 1}, {1, 0, 0}}, {{0, 0, 1}, {-1, 0, 0}}, {{0, 0, -1}, {-1, 0, 0}}, {{0, 0, -1}, {1, 0, 0}}};
	int m_lSteps;
	Vec3 m_lPos;
	Vec3 m_lPosD;
	Vec2 m_lRot;
};

