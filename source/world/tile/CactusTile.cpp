#include "CactusTile.hpp"
#include "world/level/Level.hpp"

CactusTile::CactusTile(int id, int texture) : Tile(id, texture, Material::cactus)
{
	setTicking(true);
}

bool CactusTile::mayPlace(const Level* level, const TilePos& pos) const
{
	return Tile::mayPlace(level, pos) && canSurvive(level, pos);
}

bool CactusTile::canSurvive(const Level* level, const TilePos& pos) const
{
	for (Facing::Name i : Facing::HORIZONTAL)
	{
		if (level->getMaterial(pos.relative(i))->isSolid())
		{
			return false;
		}
	}

	TileID tile = level->getTile(pos.below());

	return tile == Tile::sand->m_ID || tile == m_ID;
}


void CactusTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
	if (!canSurvive(level, pos))
	{
		spawnResources(level, pos, level->getData(pos));
		level->setTile(pos, TILE_AIR);
	}
}

bool CactusTile::isSolidRender() const
{
	return false;
}

bool CactusTile::isCubeShaped() const
{
	return false;
}

int CactusTile::getRenderShape() const
{
	return SHAPE_CACTUS;
}

void CactusTile::tick(Level* level, const TilePos& pos, Random* random) 
{
	TilePos above = pos.above();
	if (level->isEmptyTile(pos.above())) {
		int height;
		for (height = 1; level->getTile(pos.below(height)) == m_ID; ++height) {
		}

		if (height < 3) {
			int data = level->getData(pos);
			if (data == 15) {
				level->setTile(above, m_ID);
				level->setData(pos, 0);
			}
			else {
				level->setData(pos, data + 1);
			}
		}
	}
}

void CactusTile::updateShape(const LevelSource* level, const TilePos& pos)
{
	setShape(0.0625, 0, 0.0625, 0.9375, 1, 0.9375);
}

void CactusTile::entityInside(Level* level, const TilePos& pos, Entity* entity) const
{
	entity->hurt(nullptr, 1);
}

int CactusTile::getTexture(Facing::Name face) const
{
	return face == 1 ? m_TextureFrame - 1 : (face == 0 ? m_TextureFrame + 1 : m_TextureFrame);

}
