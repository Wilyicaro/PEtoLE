#pragma once

#include <stdint.h>

#include "client/renderer/Texture.hpp"
#include <world/level/Level.hpp>

class GrassColor
{
public:

	static void init(Texture texture);

	static uint32_t get(double x, double y);

	static uint32_t get(const LevelSource* levelSource, const TilePos& pos);
	

private:
	static Texture texture;
};

