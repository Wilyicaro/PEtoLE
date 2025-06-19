#include "FoliageColor.hpp"
#include "Tesselator.hpp"

Texture FoliageColor::texture;

void FoliageColor::init(Texture texture)
{
	FoliageColor::texture = texture;
}

uint32_t FoliageColor::get(double x, double y)
{
	y *= x;
	int bgr = FoliageColor::texture.m_pixels[(int)((1.0 - y) * 255.0) << 8 | (int)((1.0 - x) * 255.0)];
	return GET_BLUE(bgr) << 16 | GET_GREEN(bgr) << 8 | GET_RED(bgr);;
}

uint32_t FoliageColor::getEvergreenColor()
{
	return 0x619961;
}

uint32_t FoliageColor::getBirchColor()
{
	return 0x80A755;
}

uint32_t FoliageColor::getDefaultColor()
{
	return 0x48B518;
}