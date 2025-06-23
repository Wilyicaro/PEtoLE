#include "PumpkinTile.hpp"
#include "world/level/Level.hpp"

PumpkinTile::PumpkinTile(int id, bool lantern) : Tile(id, TEXTURE_PUMPKIN_TOP, Material::vegetable)
{
	this->lantern = lantern;
}

int PumpkinTile::getTexture(Facing::Name face, int data) const
{
	switch (face) {
	case Facing::UP: case Facing::DOWN: return m_TextureFrame;
	default: 
		return face == 2 && data == 2 || face == 5 && data == 3 || face == 3 && data == 0 || face == 4 && data == 1 ? m_TextureFrame + (lantern ? 18 : 17) : m_TextureFrame + 16;
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
	int rot = Mth::floor(2.5f + (mob->m_rot.y * 4.0f / 360.0f)) & 3;

	level->setData(pos, rot);
}
