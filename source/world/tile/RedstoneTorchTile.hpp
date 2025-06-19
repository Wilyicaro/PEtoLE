#pragma once

#include "TorchTile.hpp"

class RedstoneTorchTile : public TorchTile
{
public:
	RedstoneTorchTile(int ID, int texture, Material* pMtl, bool on);

	int getTickDelay() const override;
	int getResource(int, Random*) const override;
	int getTexture(Facing::Name face, int data) const override;
	void animateTick(Level*, const TilePos& pos, Random*) override;
	void neighborChanged(Level*, const TilePos& pos, TileID tile) override;
	void onPlace(Level*, const TilePos& pos) override;
	void onRemove(Level*, const TilePos& pos) override;
	int getDirectSignal(const Level*, const TilePos& pos, Facing::Name face) const override;
	int getSignal(const LevelSource*, const TilePos& pos, Facing::Name face) const override;
	bool isSignalSource() const override;
	void tick(Level*, const TilePos& pos, Random*) override;

private:
	struct Toggle {
		TilePos pos;
		int32_t when;

		Toggle(const TilePos& p, int32_t w) : pos(p), when(w) {}
	};

	bool isToggledTooFrequently(Level* level, const TilePos& pos, bool addRecent);
	bool hasNeighborSignal(const Level* level, const TilePos& pos);
	bool m_bOn;
	std::vector<Toggle> m_recentToggles;

};
