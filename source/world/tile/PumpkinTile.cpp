#include "PumpkinTile.hpp"
#include "world/level/Level.hpp"

PumpkinTile::PumpkinTile(int id, bool lantern) : Tile(id, TEXTURE_PUMPKIN_TOP, Material::vegetable)
{
	this->lantern = lantern;
}

int PumpkinTile::getTexture(const LevelSource* level, const TilePos& pos, Facing::Name face) const
{
	switch (face) {
	case Facing::UP: case Facing::DOWN: return m_TextureFrame;
	default: 
		int meta = level->getData(pos);
		int dir = 4;

		switch (meta)
		{
		case 0: dir = 2; break;
		case 1: dir = 5; break;
		case 2: dir = 3; break;
		}
		return face != dir ? m_TextureFrame + 16 : m_TextureFrame + (lantern ? 18 : 17);
	}
}

int PumpkinTile::getTexture(Facing::Name face) const
{
	switch (face) {
	case Facing::UP: case Facing::DOWN: return m_TextureFrame;
	case Facing::SOUTH: return m_TextureFrame + 17;
	default: return m_TextureFrame + 16;
	}
}

void PumpkinTile::setPlacedBy(Level* level, const TilePos& pos, Mob* mob, Facing::Name face)
{
	int rot = Mth::floor(0.5f + (mob->m_rot.y * 4.0f / 360.0f)) & 3;

	level->setData(pos, rot);
}
