#include "SnowTile.hpp"
#include "world/level/Level.hpp"

SnowTile::SnowTile(int id, int texture) : Tile(id, texture, Material::snow)
{
	setTicking(true);
}

void SnowTile::tick(Level* level, const TilePos& pos, Random*)
{
	if (level->getBrightness(LightLayer::Block, pos) > 11) {
		spawnResources(level, pos, level->getData(pos));
		level->setTile(pos, 0);
	}
}

int SnowTile::getResource(int data, Random*) const
{
	return Item::snowBall->m_itemID;
}

int SnowTile::getResourceCount(Random* random) const
{
	return 4;
}