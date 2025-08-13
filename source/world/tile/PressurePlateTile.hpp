#pragma once

#include "Tile.hpp"

class PressurePlateTile : public Tile
{
public:
	enum Sensitivity {
		everything,
		mobs,
		players,
	};

	PressurePlateTile(int id, int texture, Sensitivity sensitivity);
	virtual AABB* getAABB(const Level*, const TilePos& pos) override;
	bool mayPlace(const Level*, const TilePos& pos) const override;
	void neighborChanged(Level* level, const TilePos& pos, TileID tile) override;
	int getTickDelay() const override;
	bool isSolidRender() const override;
	bool isCubeShaped() const override;
	void updateShape(const LevelSource* level, const TilePos& pos) override;
	void updateDefaultShape() override;
	void checkPressed(Level* pLevel, const TilePos& pos) const;
	void tick(Level*, const TilePos& pos, Random*) override;
	void entityInside(Level*, const TilePos& pos, Entity*) const override;
	void onPlace(Level* pLevel, const TilePos& pos) override;
	void onRemove(Level* pLevel, const TilePos& pos) override;
	int getSignal(const LevelSource*, const TilePos& pos, Facing::Name face) const override;
	int getDirectSignal(const Level*, const TilePos& pos, Facing::Name face) const override;
	bool isSignalSource() const override;

public:
	Sensitivity m_sensitivity;

};

