#pragma once

#include "Snowball.hpp"

class Fireball : public Snowball
{
public:
	Fireball(Level*);
	Fireball(Level*, std::shared_ptr<Mob> mob, const Vec3& pos);
	Fireball(Level*, const Vec3& pos, const Vec3& power);

	virtual void tick() override;
	virtual void onHit() override;

	virtual bool isPickable() const override;
	virtual float getPickRadius() const override;
	virtual bool hurt(Entity*, int) override;
};

