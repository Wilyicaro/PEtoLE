#include "GrassColor.hpp"
#include "Tesselator.hpp"

Texture GrassColor::texture;

void GrassColor::init(Texture texture)
{
	GrassColor::texture = texture;
}

uint32_t GrassColor::get(double x, double y)
{
	y *= x;
	int bgr = GrassColor::texture.m_pixels[int((1.0 - y) * 255.0) << 8 | int((1.0 - x) * 255.0)];
	return GET_BLUE(bgr) << 16 | GET_GREEN(bgr) << 8 | GET_RED(bgr);
}

uint32_t GrassColor::get(const LevelSource* levelSource, const TilePos& pos)
{
	levelSource->getBiomeSource()->getBiome(pos);
	return get(levelSource->getBiomeSource()->m_temperatures[0], levelSource->getBiomeSource()->m_downfalls[0]);
}