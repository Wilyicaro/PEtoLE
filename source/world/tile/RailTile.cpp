#include "RailTile.hpp"

RailTile::RailTile(int id, int texture, bool isPowered) : Tile(id, texture, Material::decoration), m_bIsPowered(isPowered)
{
	setShape(0.0F, 0.0F, 0.0F, 1.0F, 2.0F / 16.0F, 1.0F);
}

bool RailTile::mayPlace(const Level* level, const TilePos& pos) const
{
	return level->isSolidTile(pos.below());
}

void RailTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
	if (!level->m_bIsOnline) {
		int data = level->getData(pos);

		if (m_bIsPowered) {
			data = data & 7;
		}

		bool var7 = false;

		if (!level->isSolidTile(pos.below())) {
			var7 = true;
		}

		if (data == 2 && !level->isSolidTile(pos.east())) {
			var7 = true;
		}

		if (data == 3 && !level->isSolidTile(pos.west())) {
			var7 = true;
		}

		if (data == 4 && !level->isSolidTile(pos.north())) {
			var7 = true;
		}

		if (data == 5 && !level->isSolidTile(pos.south())) {
			var7 = true;
		}

		if (var7) {
			spawnResources(level, pos, level->getData(pos));
			level->setTile(pos, TILE_AIR);
		}
		else if (tile > 0 && Tile::tiles[tile]->isSignalSource() && !m_bIsPowered && (Rail(level, pos)).countPotentialConnections() == 3) {
			updateDir(level, pos, false);
		}

	}
}

bool RailTile::isSolidRender() const
{
	return false;
}

bool RailTile::isCubeShaped() const
{
	return false;
}

AABB* RailTile::getAABB(const Level*, const TilePos& pos)
{
	return nullptr;
}

int RailTile::getRenderShape() const
{
	return SHAPE_RAIL;
}

void RailTile::updateShape(const LevelSource* level, const TilePos& pos)
{
	int var5 = level->getData(pos);
	if (var5 >= 2 && var5 <= 5) {
		setShape(0.0F, 0.0F, 0.0F, 1.0F, 10.0F / 16.0F, 1.0F);
	}
	else {
		setShape(0.0F, 0.0F, 0.0F, 1.0F, 2.0F / 16.0F, 1.0F);
	}

}

void RailTile::onPlace(Level* level, const TilePos& pos)
{
	updateDir(level, pos, true);
}

void RailTile::setPlacedBy(Level* level, const TilePos& pos, Mob* mob, Facing::Name face)
{
	int rot = Mth::floor(0.5f + (mob->m_rot.y * 4.0f / 360.0f)) & 3;
	if (rot == 1 || rot == 3) {
		level->setData(pos, 1);
	}

	neighborChanged(level, pos, 0);
	updateDir(level, pos, true);
}

void RailTile::updateDir(Level* level, const TilePos& pos, bool updateNeighbors)
{
	if (!level->m_bIsOnline) Rail(level, pos).place(level->hasNeighborSignal(pos), updateNeighbors);
}

int RailTile::getTexture(Facing::Name face, int data) const
{
	if (m_bIsPowered) {
		if ((data & 8) != 0) {
			return m_TextureFrame + 16;
		}
	}
	else if (data >= 6) {
		return m_TextureFrame - 16;
	}

	return m_TextureFrame;

}
