#pragma once

#include "Entity.hpp"
#include "PaintingType.hpp"

class Painting : public Entity
{
public:
	Painting(Level*);
	Painting(Level*, const TilePos& pos, int);

	PaintingType& getPaintingType()
	{
		return PaintingType::types[m_paintingType];
	}

	void setDir(int face);

	void tick() override;

	bool survives();

	void addAdditionalSaveData(CompoundIO tag) override;

	void readAdditionalSaveData(CompoundIO tag) override;

	bool hurt(Entity*, int) override;

	bool isPickable() const override { return true; }

private:
	float offs(int);

public:
	int m_dir;
	TilePos m_tilePos;
	int m_paintingType;

private:
	int m_checkInterval;
};
