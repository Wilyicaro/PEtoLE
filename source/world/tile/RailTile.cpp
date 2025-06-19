#include "RailTile.hpp"
#include "world/level/Level.hpp"

RailTile::RailTile(int id, int texture, bool isPowered) : Tile(id, texture, Material::decoration)
{
	m_bIsPowered = isPowered;
	setShape(0.0F, 0.0F, 0.0F, 1.0F, 2.0F / 16.0F, 1.0F);
}

bool RailTile::mayPlace(const Level* level, const TilePos& pos) const
{
	return level->isSolidTile(pos.west()) ? true : (level->isSolidTile(pos.east()) ? true : (level->isSolidTile(pos.north()) ? true : (level->isSolidTile(pos.south()) ? true : level->isSolidTile(pos.below()))));
}

void RailTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
}

bool RailTile::isSolidRender() const
{
	return false;
}

bool RailTile::isCubeShaped() const
{
	return false;
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

void RailTile::setPlacedBy(Level* level, const TilePos& pos, Mob* mob, Facing::Name face)
{
	int rot = Mth::floor(0.5f + (mob->m_rot.y * 4.0f / 360.0f)) & 3;
	if (rot == 1 || rot == 3) {
		level->setData(pos, 1);
	}

	neighborChanged(level, pos, 0);
}

int RailTile::getTexture(Facing::Name face, int data) const
{
	if (m_bIsPowered) {
		if ((data & 8) != 0) {
			return data + 16;
		}
	}
	else if (data >= 6) {
		return m_TextureFrame - 16;
	}

	return m_TextureFrame;

}
