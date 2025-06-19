#pragma once

#include "Texture.hpp"

class TextureProcessor
{
public:
	virtual Texture* process(Texture& texture) = 0;
};
