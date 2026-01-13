#include "TallGrass.hpp"
#include "world/level/Level.hpp"
#include <client/renderer/GrassColor.hpp>
#include <client/renderer/FoliageColor.hpp>

TallGrass::TallGrass(int id, int texture) : Bush(id, texture)
{
	setShape(0.1F, 0.0F, 0.1F, 0.9F, 0.8F, 0.9F);
}

int TallGrass::getResource(int x, Random* random) const
{
	return random->nextInt(8) == 0 ? Item::seeds->m_itemID : 0;
}

int TallGrass::getColor(const LevelSource* levelSource, const TilePos& pos, Facing::Name facing, int texture) const
{
	int data = levelSource->getData(pos);
	return data == 0 ? 0xFFFFFF : GrassColor::get(levelSource, pos);
}

int TallGrass::getColor(int data, Facing::Name facing, int texture) const
{
	return data == 0 ? 0xFFFFFF : FoliageColor::getDefaultColor();
}

int TallGrass::getTexture(Facing::Name face, int data) const
{
	return data == 1 ? m_TextureFrame : (data == 2 ? m_TextureFrame + 16 + 1 : (data == 0 ? m_TextureFrame + 16 : m_TextureFrame));
}

int TallGrass::getRenderShape() const
{
	return SHAPE_RANDOM_CROSS;
}
