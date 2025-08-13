#pragma once

#include "Snowball.hpp"

class ThrownEgg : public Snowball
{
public:
	ThrownEgg(Level*);
	ThrownEgg(Level*, const Vec3& pos);
	ThrownEgg(Level*, std::shared_ptr<Mob> mob);

	virtual void onHit() override;
};

