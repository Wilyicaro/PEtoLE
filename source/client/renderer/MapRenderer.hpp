#pragma once

#include "Font.hpp"
#include <client/options/Options.hpp>
#include <world/entity/Player.hpp>
#include <world/level/saveddata/MapItemSavedData.hpp>

class MapRenderer
{
public:
	MapRenderer(Font*, Options*);

	void render(const std::shared_ptr<Player>&, Textures*, const std::shared_ptr<MapItemSavedData>&);

public:
	Font* m_pFont;
	Options* m_pOptions;

	uint32_t m_pixels[16384];
};