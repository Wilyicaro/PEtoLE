#pragma once

#include "Entity.hpp"
#include "world/item/SimpleContainer.hpp"

class Level;
class Player;

class Minecart : public Entity, SimpleContainer
{
public:
	Minecart(Level*);
	Minecart(Level*, const Vec3& pos, int);

	const AABB* getCollideBox() const override;
	AABB* getCollideAgainstBox(Entity* ent) const override;
	float getShadowHeightOffs() const override { return 0.0f; }
	bool interact(Player* player) override;
	bool isPickable() const override { return !m_bRemoved; }
	bool isPushable() const override { return true; }
	void tick() override;
	real getRideHeight() override;
	bool hurt(Entity*, int) override;
	void animateHurt() override;
	void remove() override;
	Vec3* getPosOffs(const Vec3&, real);
	Vec3* getPos(const Vec3&);
	void push(Entity*) override;
	void lerpTo(const Vec3& pos, const Vec2& rot, int steps) override;
	void lerpMotion(const Vec3& vel) override;
	bool stillValid(Player* var1) override;
	void addAdditionalSaveData(std::shared_ptr<CompoundTag> tag) override;
	void readAdditionalSaveData(std::shared_ptr<CompoundTag> tag) override;

	float getLootContent();

public:
	int m_damage;
	int m_hurtTime;
	int m_hurtDir;
	int m_type;
	int m_fuel;
	real m_xPush;
	real m_zPush;

private:
	static const constexpr int EXITS[10][2][3] = {{{0, 0, -1}, {0, 0, 1}}, {{-1, 0, 0}, {1, 0, 0}}, {{-1, -1, 0}, {1, 0, 0}}, {{-1, 0, 0}, {1, -1, 0}}, {{0, 0, -1}, {0, -1, 1}}, {{0, -1, -1}, {0, 0, 1}}, {{0, 0, 1}, {1, 0, 0}}, {{0, 0, 1}, {-1, 0, 0}}, {{0, 0, -1}, {-1, 0, 0}}, {{0, 0, -1}, {1, 0, 0}}};
	bool m_bFlipped;
	int m_lSteps;
	Vec3 m_lPos;
	Vec3 m_lPosD;
	Vec2 m_lRot;
};

